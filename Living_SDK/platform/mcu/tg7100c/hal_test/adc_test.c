/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#define BT_DBG_ENABLED 1

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <hal/soc/adc.h>
#include <hal/soc/soc.h>
#include "hal_test_entry.h"

#define ADC1_PORT_NUM 1
#define ADC1_PORT_NUM_1 5
#define TAG "TEST_HAL_ADC"
/* define dev */
adc_dev_t adc1 = {
    ADC1_PORT_NUM,
    {0,900},
    NULL,
};

adc_dev_t adc2 = {
    ADC1_PORT_NUM,
    {0,8000, ADC_CONTINUE},
    NULL,
};

void adc_cb(adc_event_t event, void *data, uint32_t size)
{
#if 0
    int i;

    printf("evnet:%d\r\n", event);

    if (NULL != data) {
        for (i = 0; i < size; i++) {
            printf("adc_data:%u\r\n", ((uint16_t *)data)[i]);
        }
    }
#endif
    static volatile int i = 0;
    printf("%d\r\n", i++);
}

int aos_hal_adc_mic_init_test()
{
    int ret   = -1;

    /* adc port set */
    adc1.port = ADC1_PORT_NUM;

    /* set sampling_cycle */
    adc1.config.sampling_cycle = 1000;

    ret = hal_adc_init(&adc2);

    hal_adc_sample_cb_reg(&adc2, adc_cb);
    
    if (ret != 0) {
        AT_BACK_ERR();
    }

    AT_BACK_OK();
}

int aos_hal_adc_mic_start_test()
{
    int  ret = -1;
    uint16_t data[100];

    ret = hal_adc_start(&adc2, data, 100);
    
    if (ret != 0) {
        AT_BACK_ERR();
    }
    
    aos_msleep(1000);
    
    ret = hal_adc_stop(&adc2);
    
    if (ret != 0) {
        AT_BACK_ERR();
    }
   
    AT_BACK_OK();
}

int aos_hal_adc_mic_stop_test()
{
    int  ret = -1;
   
    ret = hal_adc_finalize(&adc2);

    if (ret != 0) {
        AT_BACK_ERR();
    }

    AT_BACK_OK();
}

int aos_hal_adc_api_test()
{
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    int32_t ret4 = -1;
    int32_t ret5 = -1;
    int value = 0;
    int i = 0;

    /* adc port set */
    adc1.port = ADC1_PORT_NUM;

    /* set sampling_cycle */
    adc1.config.sampling_cycle = 100;
 
    /* init adc1 with the given settings */
    ret1 = hal_adc_init(NULL);

    ret2 = hal_adc_init(&adc1);

    ret3 = hal_adc_value_get(NULL, (uint32_t *)&value, HAL_WAIT_FOREVER);

    ret4 = hal_adc_value_get(&adc1, NULL, HAL_WAIT_FOREVER);

    /* finalize adc1 */

    ret5 = hal_adc_finalize(&adc1);

    printf("%d \n%d \n%d \n%d \n%d \n",(int)ret1,(int)ret2,(int)ret3,(int)ret4,(int)ret5);
    if (ret1 != 0 && ret2 == 0 && ret3 != 0 && ret4 != 0 && ret5 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

int aos_hal_adc_test()
{
    int ret   = -1;
    int value = 0;
    int i = 0;

    /* adc port set */
    adc1.port = ADC1_PORT_NUM;

    /* set sampling_cycle */
    adc1.config.sampling_cycle = 100;

    /* init adc1 with the given settings */
    ret = hal_adc_init(NULL);

    ret = hal_adc_init(&adc1);

    if (ret != 0) {
        AT_BACK_ERR();
    }

    for (i=0; i<100; i++){
        /* get adc value */
        ret = hal_adc_value_get(&adc1, (uint32_t *)&value, HAL_WAIT_FOREVER);

        if (ret != 0) {
            AT_BACK_ERR();
        }
        else{
            if (value!=0)
                AT_BACK_OK();
        }    
    }

    ret = hal_adc_value_get(&adc1, (uint32_t *)&value,0);
    if (value!=0)
        AT_BACK_ERR();

    /* finalize adc1 */

    hal_adc_finalize(&adc1);
    ret = hal_adc_value_get(&adc1, (uint32_t *)&value,0);
    AT_BACK_OK();

}

int aos_hal_adc_read_test()
{
    int ret   = -1;
    int value = 0;
    int i = 0;

    /* adc port set */
    adc1.port = ADC1_PORT_NUM;

    /* set sampling_cycle */
    adc1.config.sampling_cycle = 100;

    /* init adc1 with the given settings */
    ret = hal_adc_init(NULL);

    ret = hal_adc_init(&adc1);

    if (ret != 0) {
        AT_BACK_ERR();
    }

    for (i=0; i<100; i++){
        /* get adc value */
        ret = hal_adc_value_get(&adc1, (uint32_t *)&value, HAL_WAIT_FOREVER);

        if (ret != 0) {
            AT_BACK_ERR();
        }
        else{
            printf("adc data is = %d \r\n",value);
        }    
    }
    /* finalize adc1 */

    hal_adc_finalize(&adc1);
    AT_BACK_OK();
}
    
void test_hal_adc(char *cmd, int type, char *data)
{
    if (strcmp((const char *)data, "'READ'\0") == 0) {
        aos_hal_adc_read_test();
    }else if (strcmp((const char *)data, "'API'\0") == 0) {
        aos_hal_adc_api_test();
    }else if (strcmp((const char *)data, "'TEST'\0") == 0) {
        aos_hal_adc_test();
    }else if (strcmp((const char *)data, "TEST") == 0) {
        aos_hal_adc_read_test();
        aos_hal_adc_api_test();
        aos_hal_adc_test();
    }else if (strcmp((const char *)data, "init") == 0) {
        aos_hal_adc_mic_init_test();
    }else if (strcmp((const char *)data, "start") == 0) {
        aos_hal_adc_mic_start_test();
    }else if (strcmp((const char *)data, "stop") == 0) {
        aos_hal_adc_mic_stop_test();
    }
}
