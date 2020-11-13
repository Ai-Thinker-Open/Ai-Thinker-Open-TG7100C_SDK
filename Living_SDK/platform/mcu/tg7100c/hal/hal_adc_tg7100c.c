
#include <stdio.h>
#include <stdint.h>
#include <hal/soc/soc.h>
#include <hal_adc.h>

//gpio_num or port //4,5,6,9,10,11,12,13,14,15
//for audio, freq 500HZ~16000HZ

typedef void (*hal_adc_cb_t)(void *arg);

hal_adc_cb_t user_adc_cb = NULL;

void adc_data_callback(int mode, uint32_t *ptr, uint32_t data_size, void *arg)
{
    int i;
    int sum = 0;

    for (i = 0; i < data_size; i++) {
        sum += (uint16_t)(0xFFFF & ptr[i]);
    }

    //printf("adc avg: %ld\r\n", sum/data_size);

    if (user_adc_cb) {
        user_adc_cb(ptr);
     }
}

int32_t hal_adc_init(adc_dev_t *adc)
{
    int res = 0;
    int mode;
    int freq;
    int data_num;
    int gpio_num;

    if (NULL == adc) {
        printf("arg error.\r\n");
        return -1;
    }

    mode = 1;// 1: audio
    freq = adc->config.freq;
    data_num = adc->config.data_count;
    gpio_num = (int)adc->port;

    hal_bladc_init(mode, freq, data_num, gpio_num);
    hal_adc_callback_register(adc_data_callback);
    if (adc->priv) {
        user_adc_cb = adc->priv;
    }
#if 0
    int sampling_ms = 10;//10~150 ms
    int gpio_num = 4;//4,5,6,9,10,11,12,13,14,15 ms

    if (NULL == adc) {
        printf("arg arror.\r\n");
        return -1;
    }

    gpio_num = (int)adc->port;
    sampling_ms = (int)adc->config.sampling_cycle;

    res = hal_bladc_init(sampling_ms);
    if (res) {
        printf("arg arror.\r\n");
        return -1;
    }
    res = hal_adc_add_channel(gpio_num);
    if (res) {
        printf("arg arror.\r\n");
        return -1;
    }
#endif
	return res;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
#if 0
    int gpio_num = 4;//4,5,6,9,10,11,12,13,14,15 ms

    if ((NULL == adc) || (NULL == output)) {
        printf("arg arror.\r\n");
        return -1;
    }

    gpio_num = (int)adc->port;

    *(uint32_t*)output = hal_adc_get_data(gpio_num);
#endif
	return 0;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    int ret;
    user_adc_cb = NULL;
    ret = hal_bladc_finalize();

	return ret;
}
