#include "hal/soc/soc.h"
#include "rtos_pub.h"
#include "drv_model_pub.h"
#include "saradc_pub.h"

#define ADC_RECV_BUFFER 20

static saradc_desc_t adc_desc;
static DD_HANDLE adc_hdl;
static uint16_t adc_data[ADC_RECV_BUFFER];
static uint16_t adc_output;

static hal_adc_cb_t g_adc_callback_handler = NULL;
static adc_data_t hal_adc_data;
static adc_data_t user_adc_data;
static adc_event_t adc_event = ADC_INT_DMA_TRC;

static void hal_adc_handler(void)
{
	uint8_t read_cnt;

	if(adc_desc.has_data)
	{
		read_cnt = adc_desc.current_sample_data_cnt - 1;
		adc_output = adc_desc.pData[read_cnt];
		adc_desc.has_data = 0;
		adc_desc.current_sample_data_cnt = 0;
		if (g_adc_callback_handler) {
            ((uint16_t*)(user_adc_data.data))[user_adc_data.size++] = adc_output;
            if (user_adc_data.size == (hal_adc_data.size>>1)) {
                user_adc_data.size = 0;
				if (adc_event == ADC_INT_DMA_TRC) {
					adc_event = ADC_INT_DMA_TRH;
					user_adc_data.data = hal_adc_data.data;
				} else {
					adc_event = ADC_INT_DMA_TRC;
					user_adc_data.data = (uint16_t*)(hal_adc_data.data) + (hal_adc_data.size>>1);
				}
                g_adc_callback_handler(adc_event, user_adc_data.data, (hal_adc_data.size>>1));
            }
        }
	}
}

int32_t hal_adc_init(adc_dev_t *adc)
{
	if(adc == NULL)
	{
		return -1;
	}

    memset(&adc_desc, 0x00, sizeof(saradc_desc_t));
	adc_desc.channel = adc->port;
	/* BK: period=(16 + sampling_cycle)*adc_clk */
	adc_desc.samp_rate = (1000000/adc->config.sampling_freq) - 16;
	adc_desc.pre_div = 0x0C << 2;//0x05
	adc_desc.pData = &adc_data[0];
	adc_desc.data_buff_size = ADC_RECV_BUFFER;
	adc_desc.mode = 0x03;
    adc_desc.p_Int_Handler = hal_adc_handler;

	return 0;
}

int32_t hal_adc_sample_cb_reg(adc_dev_t *adc, hal_adc_cb_t cb)
{
	g_adc_callback_handler = cb;
	return 0;
}

int32_t hal_adc_start(adc_dev_t *adc, void *data, uint32_t size)
{
	uint32_t status;

	if(adc == NULL)
	{
		return -1;
	}
	if (adc_desc.channel != adc->port)
	{
		return -2;
	}
	hal_adc_data.size = size;
	hal_adc_data.data = data;
	user_adc_data.data = hal_adc_data.data;
	user_adc_data.size = 0;
	adc_hdl = ddev_open(SARADC_DEV_NAME, &status, (uint32_t)&adc_desc);
	if(status)
	{
		return -2;
	}	
	return 0;
}

int32_t hal_adc_stop(adc_dev_t *adc)
{
	uint8_t param = 0;

	if(adc == NULL)
	{
		return -1;
	}
	if (adc_desc.channel != adc->port)
	{
		return -2;
	}
	ddev_control(adc_hdl, SARADC_CMD_RUN_OR_STOP_ADC, (void *)&param);
	ddev_close(adc_hdl);
	return 0;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
	*(uint16_t*)output = adc_output;

	return 0;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
	uint8_t param;

	param = 0;
	ddev_control(adc_hdl, SARADC_CMD_RUN_OR_STOP_ADC, (void *)&param);
	ddev_close(adc_hdl);
	g_adc_callback_handler = NULL;
	hal_adc_data.data = NULL;
	hal_adc_data.size = 0;
	user_adc_data.data = NULL;
	user_adc_data.size = 0;
	return 0;
}

