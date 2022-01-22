
#include <stdio.h>
#include <stdint.h>
#include <hal/soc/soc.h>
#include <hal_adc.h>
#include <bl_adc.h>

#define HAL_ADC_DATA_BUF_SIZE 64

static hal_adc_cb_t g_cb = NULL;
static const int adc_port2pin[12] = {
    
    12,     // ch 0   pin 12
    4,      // ch 1   pin 4
    14,     // ch 2   pin 14
    13,     // ch 3   pin 13
    5,      // ch 4   pin 5
    6,      // ch 5   pin 6
    9,      // ch 6   pin 9
    9,      // ch 7   pin 9
    10,     // ch 8   pin 10
    10,     // ch 9   pin 10
    11,     // ch 10  pin 11
    15,     // ch 11  pin 15
};

//gpio_num or port //4,5,6,9,10,11,12,13,14,15
//for audio, freq 500HZ~16000HZ
#if 0
static void __adc_data_callback(int mode, uint32_t *ptr, uint32_t data_size, void *arg)// arg == app_cb
{
    int i;
    adc_dev_t *adc = (adc_dev_t *)arg;
    uint16_t *data_buf = adc->priv;

    for (i = 0; i < data_size; i++) {
        data_buf[i] = (uint16_t)(0xFFFF & ptr[i]);
    }
}
#endif
int32_t hal_adc_init(adc_dev_t *adc)
{
    int res = 0;
    int mode;
    int freq;
    int gpio_num;
    uint32_t len;

    if (NULL == adc || (adc->config.mode != ADC_ONE_SHOT && adc->config.mode != ADC_CONTINUE)) {
        return -1;
    }

    //mode = 0;// 1: audio
    mode = adc->config.mode;
    freq = adc->config.sampling_freq;
    //data_num = adc->config.data_count;
    gpio_num = adc_port2pin[adc->port];

    if (mode == ADC_ONE_SHOT) {
        len = ADC_CHANNEL_MAX;
        hal_bladc_init(mode, freq, len, gpio_num);
    }

	return res;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    int gpio_num = 4;//4,5,6,9,10,11,12,13,14,15 ms
    int ret;

    if ((NULL == adc) || (NULL == output)) {
        printf("arg arror.\r\n");
        return -1;
    }

    gpio_num = adc_port2pin[adc->port];

    while ((ret = hal_adc_get_data(gpio_num, 0)) == -1) {
        if (timeout-- == 0) {
            return -1;
        }
        aos_msleep(1);
    }
    *(uint32_t*)output = ret;
	return 0;
}

int32_t hal_adc_start_DMA(adc_dev_t *adc, hal_adc_cb_t *cb, uint32_t *data, uint32_t size)
{
    int ret;
    int oneshot;

    if (adc == NULL || cb == NULL || data == NULL) {

        return -1;
    }

    oneshot = adc->config.mode;
    ret = hal_adc_callback_register((bl_adc_cb_t)cb, (uint32_t *)data, size, oneshot); 

    return ret;
}

int32_t hal_adc_sample_cb_reg(adc_dev_t *adc, hal_adc_cb_t cb)
{
    if (adc == NULL) {
        return -1;
    }

    g_cb = cb;

    return 0;
}

int32_t hal_adc_calibration(adc_dev_t *adc)
{
    if (adc == NULL) {
        return -1;
    }

    return 0;
}

int32_t hal_adc_start(adc_dev_t *adc, void *data, uint32_t size)
{
    int ret;
    int mode;
    int freq;
    int gpio_num;
    adc_ctx_t *ctx;
    
    if (adc == NULL || data == NULL) {
        return -1;
    } 

    mode = adc->config.mode;
    freq = adc->config.sampling_freq;
    gpio_num = adc_port2pin[adc->port];
    
    if (mode == ADC_CONTINUE) {
        hal_bladc_init(mode, freq, size, gpio_num);
    } 

    ctx = (adc_ctx_t *)bl_dma_find_ctx_by_channel(ADC_DMA_CHANNEL);
    ctx->adc_cb = g_cb;
    ctx->arg = data;

    ret = bl_adc_start();

    return ret;
}

int32_t hal_adc_stop(adc_dev_t *adc)
{
    int ret;

    if (adc == NULL) {
        return -1;
    }

    ret = bl_adc_deinit();

    return ret;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    int ret;

    if (adc == NULL) {
        return -1;
    }

    ret = hal_bladc_finalize();

	return ret;
}
