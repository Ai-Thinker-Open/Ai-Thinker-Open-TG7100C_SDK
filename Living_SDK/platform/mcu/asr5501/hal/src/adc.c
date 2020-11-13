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


int32_t hal_adc_init(adc_dev_t *adc)
{
    int32_t ret = 0;
    if(NULL == adc){
        return -1;
    }
    ret = lega_adc_init((lega_adc_dev_t *)adc);
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
    ret = lega_adc_finalize((lega_adc_dev_t *)adc);
    return ret;

}

#endif
