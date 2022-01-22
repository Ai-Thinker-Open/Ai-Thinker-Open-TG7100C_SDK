/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include "lega_rhino.h"
#include "hal/soc/adc.h"
#include "lega_adc.h"
#include "board.h"


#if defined ADC_ENABLE
/*
typedef struct adc_obj{
    uint32_t port;
    adc_channel_t channel;
}adc_obj_t;


static  adc_obj_t obj[]={

    {GPIO4_INDEX, ADC_CHANNEL_NUM0},
    {GPIO5_INDEX, ADC_CHANNEL_NUM1},
    {GPIO6_INDEX, ADC_CHANNEL_NUM2},
    {GPIO7_INDEX, ADC_CHANNEL_NUM3},
    {GPIO8_INDEX, ADC_CHANNEL_NUM4},
    {GPIO9_INDEX, ADC_CHANNEL_NUM5},
    {GPIO10_INDEX, ADC_CHANNEL_NUM6},
    {GPIO11_INDEX, ADC_CHANNEL_NUM7},
};

*/

extern adc_data_t hal_adc_data;
extern adc_data_t user_adc_data;

int32_t hal_adc_init(adc_dev_t *adc)
{
    int32_t ret = 0;
    if(NULL == adc){
        return -1;
    }
    return ret;
}

int32_t hal_adc_sample_cb_reg(adc_dev_t *adc, hal_adc_cb_t cb)
{
    if(NULL == adc){
        return -1;
    }
	adc->priv = cb;
	return 0;
}

int32_t hal_adc_start(adc_dev_t *adc, void *data, uint32_t size)
{
	int32_t ret = 0;
    lega_adc_dev_t lega_adc_dev;

    if(NULL == adc){
        return -1;
    }
    hal_adc_data.data = data;
    hal_adc_data.size = size;
    user_adc_data.data = hal_adc_data.data;
    user_adc_data.size = 0;
    lega_adc_dev.priv = adc->priv;
    lega_adc_dev.port = adc->port;
    ret = lega_adc_init(&lega_adc_dev);
	return ret;
}

int32_t hal_adc_stop(adc_dev_t *adc)
{
    int32_t ret = 0;
    lega_adc_dev_t lega_adc_dev;

    if(NULL == adc){
        return -1;
    }
    lega_adc_dev.priv = adc->priv;
    lega_adc_dev.port = adc->port;
    ret = lega_adc_finalize(&lega_adc_dev);
    return ret;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    if(adc->port > 8){
        *(int32_t*)(output) = -1;
        return(-1);
    }
    *(int32_t*)(output) = lega_adc_get((lega_adc_dev_t *)adc);
    return (0);
}


int32_t hal_adc_finalize(adc_dev_t *adc)
{
    int32_t ret = 0;
    if(NULL == adc){
        return -1;
    }
    hal_adc_data.data = NULL;
    hal_adc_data.size = 0;
    user_adc_data.data = NULL;
    user_adc_data.size = 0;
    ret = lega_adc_finalize((lega_adc_dev_t *)adc);
    return ret;

}

#endif
