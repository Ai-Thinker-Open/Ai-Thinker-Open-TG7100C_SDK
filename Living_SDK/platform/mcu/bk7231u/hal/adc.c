#include "hal/soc/soc.h"
#include "rtos_pub.h"
#include "drv_model_pub.h"
#include "saradc_pub.h"

#define ADC_RECV_BUFFER 20

static saradc_desc_t adc_desc;
static DD_HANDLE adc_hdl;
static uint16_t adc_data[ADC_RECV_BUFFER];

volatile uint16_t adc_output;

typedef void (*hal_adc_cb_t)(void *arg);

hal_adc_cb_t user_adc_cb = NULL;

static void hal_adc_handler(void)
{
	uint8_t read_cnt;
	if(adc_desc.has_data)
	{
		read_cnt = adc_desc.current_sample_data_cnt - 1;
		adc_output = adc_desc.pData[read_cnt];
		adc_desc.has_data = 0;
		adc_desc.current_sample_data_cnt = 0;
		if (user_adc_cb) {
			user_adc_cb(&adc_output);
        }
	}
}

int32_t hal_adc_init(adc_dev_t *adc)
{
	uint32_t status;

	if(adc == NULL)
	{
		return -1;
	}

    memset(&adc_desc, 0x00, sizeof(saradc_desc_t));
	adc_desc.channel = adc->port;
	adc_desc.samp_rate = adc->config.sampling_cycle;
	adc_desc.pre_div = 0x0C << 2;//0x05
	adc_desc.pData = &adc_data[0];
	adc_desc.data_buff_size = ADC_RECV_BUFFER;
	adc_desc.mode = 0x03;
	if (adc->priv) {
		user_adc_cb = adc->priv;
	}
    adc_desc.p_Int_Handler = hal_adc_handler;

	adc_hdl = ddev_open(SARADC_DEV_NAME, &status, (uint32_t)&adc_desc);
	if(status)
	{
		return -2;
	}

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
	user_adc_cb = NULL;
	return 0;
}

