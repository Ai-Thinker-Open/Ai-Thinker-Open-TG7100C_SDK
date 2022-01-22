/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <aos/kernel.h>
#include <aos/aos.h>

#include "hal/soc/soc.h"
#include "hal_test_entry.h"


#define TAG "TEST_HAL_RTC"

int32_t g_timer_cb_flag;
uint32_t time_sum_1 = 0;
uint32_t time_sum_2 = 0;
struct timeval start_1,start_2;
struct timeval end;

void timer_callback_func(void *arg)
{
    // int32_t callbacktime = 0;
    //g_timer_cb_flag = 1;
    //gettimeofday(&end, NULL);
    // callbacktime = 1000000 * end.tv_sec + end.tv_usec;
    // printf("%d us, enter callback function\n\n",callbacktime);
    printf("enter callback function\n\n");
}

static void aos_hal_timer_handle1(){
    uint32_t tmpcnt1 = 0;
    int32_t ret1 = -1;
    int32_t ret3 = -1;
    int32_t ret3_result = -1;
    timer_dev_t timer_handle1={
        .port                 = 0,
        .config={
            .cb            = timer_callback_func,
            .period        = 0,         
            .reload_mode   = TIMER_RELOAD_MANU
        }
    };
    ret1 = hal_timer_init(&timer_handle1);


    timer_handle1.config.period = 1000000 + 500000;
    ret3 = hal_timer_para_chg(&timer_handle1, timer_handle1.config);
    int32_t ret2 = -1;
    ret2 = hal_timer_start(&timer_handle1);


    g_timer_cb_flag = 0;
    //gettimeofday(&start_2, NULL);
    LOGI(TAG, "<2>start timeout is %d us\n", timer_handle1.config.period);
    // int32_t start_2time = -1;
    // start_2time = 1000000 * start_2.tv_sec + start_2.tv_usec;
    // printf("start timeout is %d us\n",start_2time);
    tmpcnt1 = 0;
    do {
        if (tmpcnt1++ > 600) {
            LOGI(TAG, "60s timeout, not enter callback function\n");
            break;
        }

        aos_msleep(100);
    } while (0 == g_timer_cb_flag);
    hal_timer_stop(&timer_handle1);
    time_sum_2 = 1000000 * (end.tv_sec - start_2.tv_sec) + (end.tv_usec - start_2.tv_usec);
    printf("end - start = %lu us\n",time_sum_2);
    if (time_sum_2 >= 1000000 && time_sum_2 <= 2000000){
        ret3_result = 0;
    }
    //
    end.tv_sec = 0;
    end.tv_usec = 0;


    int32_t ret4 = -1;
    ret4 = hal_timer_finalize(&timer_handle1);

    if (ret1 == 0 && ret2 == 0 && ret3 == 0 && ret3_result == 0 && ret4 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_timer_handle2(){
    uint32_t tmpcnt = 0;
    int32_t ret1 = -1;
    timer_dev_t timer_handle2={
        .port                 = 1,
        .config={
            .cb            = timer_callback_func,
            .period        = 1000,    
            .reload_mode   = TIMER_RELOAD_MANU
        }
    };
    ret1 = hal_timer_init(&timer_handle2);


    int32_t ret2 = -1;
    int32_t ret2_result = -1;
    g_timer_cb_flag = 0;
    ret2 = hal_timer_start(&timer_handle2);
    gettimeofday(&start_1, NULL);
    LOGI(TAG, "<1>start timeout is %d us\n", timer_handle2.config.period);
    // int32_t start_1time = -1;
    // start_1time = 1000000 * start_1.tv_sec + start_1.tv_usec;
    // printf("start timeout is %d us\n",start_1time);
    tmpcnt = 0;
    do {
        if (tmpcnt++ > 600) {
            LOGI(TAG, "60s timeout, not enter callback function\n");
            break;
        }

        aos_msleep(100);
    } while (0 == g_timer_cb_flag);
    time_sum_1 = 1000000 * (end.tv_sec - start_1.tv_sec) + (end.tv_usec - start_1.tv_usec);
    printf("end - start = %lu us\n",time_sum_1);
    if (time_sum_1 >= 500 && time_sum_1 <= 1500){
        ret2_result = 0;
    }
    end.tv_sec = 0;
    end.tv_usec = 0;


    int32_t ret3 = -1;
    ret3 = hal_timer_finalize(&timer_handle2);

    if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_timer_handle3(){
    uint32_t tmpcnt = 0;
    uint32_t tmpcnt1 = 0;
    int32_t ret1 = -1;
    timer_dev_t timer_handle3={
        .port                 = 2,
        .config={
            .cb            = timer_callback_func,
            .period        = 2000000,         
            .reload_mode   = TIMER_RELOAD_AUTO
        }
    };
    ret1 = hal_timer_init(&timer_handle3);


    int32_t ret2 = -1;
    int32_t ret2_result = -1;
    g_timer_cb_flag = 0;
    ret2 = hal_timer_start(&timer_handle3);
    gettimeofday(&start_1, NULL);
    LOGI(TAG, "<1>start timeout is %d us\n", timer_handle3.config.period);
    // int32_t start_1time = -1;
    // start_1time = 1000000 * start_1.tv_sec + start_1.tv_usec;
    // printf("start timeout is %d us\n",start_1time);
    tmpcnt = 0;
    do {
        if (tmpcnt++ > 600) {
            LOGI(TAG, "60s timeout, not enter callback function\n");
            break;
        }

        aos_msleep(100);
    } while (0 == g_timer_cb_flag);
    time_sum_1 = 1000000 * (end.tv_sec - start_1.tv_sec) + (end.tv_usec - start_1.tv_usec);
    printf("end - start = %lu us\n",time_sum_1);
    if (time_sum_1 >= 1500000 && time_sum_1 <= 2500000){
        ret2_result = 0;
    }
    end.tv_sec = 0;
    end.tv_usec = 0;


    int32_t ret3 = -1;
    int32_t ret3_result = -1;
    g_timer_cb_flag = 0;
    timer_handle3.config.period += 40000000;
    ret3 = hal_timer_para_chg(&timer_handle3, timer_handle3.config);
    gettimeofday(&start_2, NULL);
    LOGI(TAG, "<2>start timeout is %d us\n", timer_handle3.config.period);
    // int32_t start_2time = -1;
    // start_2time = 1000000 * start_2.tv_sec + start_2.tv_usec;
    // printf("start timeout is %d us\n",start_2time);
    tmpcnt1 = 0;
    do {
        if (tmpcnt1++ > 600) {
            LOGI(TAG, "60s timeout, not enter callback function\n");
            break;
        }

        aos_msleep(100);
    } while (0 == g_timer_cb_flag);
    hal_timer_stop(&timer_handle3);
    time_sum_2 = 1000000 * (end.tv_sec - start_2.tv_sec) + (end.tv_usec - start_2.tv_usec);
    printf("end - start = %lu us\n",time_sum_2);
    if (time_sum_2 >= 41500000 && time_sum_2 <= 42500000){
        ret3_result = 0;
    }
    end.tv_sec = 0;
    end.tv_usec = 0;


    int32_t ret4 = -1;
    ret4 = hal_timer_finalize(&timer_handle3);

    if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0 && ret3_result == 0 && ret4 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_timer_handle4(){
    uint32_t tmpcnt = 0;
    int32_t ret1 = -1;
    timer_dev_t timer_handle4={
        .port                 = 3,
        .config={
            .cb            = timer_callback_func,
            .period        = 20000000,         
            .reload_mode   = TIMER_RELOAD_AUTO
        }
    };
    ret1 = hal_timer_init(&timer_handle4);


    int32_t ret2 = -1;
    int32_t ret2_result = -1;
    g_timer_cb_flag = 0;
    ret2 = hal_timer_start(&timer_handle4);
    gettimeofday(&start_1, NULL);
    LOGI(TAG, "<1>start timeout is %d us\n", timer_handle4.config.period);
    // int32_t start_1time = -1;
    // start_1time = 1000000 * start_1.tv_sec + start_1.tv_usec;
    // printf("start timeout is %d us\n",start_1time);
    tmpcnt = 0;
    do {
        if (tmpcnt++ > 600) {
            LOGI(TAG, "60s timeout, not enter callback function\n");
            break;
        }

        aos_msleep(100);
    } while (0 == g_timer_cb_flag);
    time_sum_1 = 1000000 * (end.tv_sec - start_1.tv_sec) + (end.tv_usec - start_1.tv_usec);
    printf("end - start = %lu us\n",time_sum_1);
    if (time_sum_1 >= 15000000 && time_sum_1 <= 25000000){
        ret2_result = 0;
    }
    end.tv_sec = 0;
    end.tv_usec = 0;


    int32_t ret3 = -1;
    ret3 = hal_timer_finalize(&timer_handle4);

    if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_timer_handle5(){
    uint32_t tmpcnt = 0;
    uint32_t tmpcnt1 = 0;
    int32_t ret1 = -1;
    timer_dev_t timer_handle5={
        .port                 = 3,
        .config={
            .cb            = timer_callback_func,
            .period        = 5000000,         
            .reload_mode   = TIMER_RELOAD_MANU
        }
    };
    ret1 = hal_timer_init(&timer_handle5);


    int32_t ret2 = -1;
    int32_t ret2_result = -1;
    g_timer_cb_flag = 0;
    ret2 = hal_timer_start(&timer_handle5);
    gettimeofday(&start_1, NULL);
    LOGI(TAG, "<1>start timeout is %d us\n", timer_handle5.config.period);
    // int32_t start_1time = -1;
    // start_1time = 1000000 * start_1.tv_sec + start_1.tv_usec;
    // printf("start timeout is %d us\n",start_1time);
    tmpcnt = 0;
    do {
        if (tmpcnt++ > 600) {
            LOGI(TAG, "60s timeout, not enter callback function\n");
            break;
        }

        aos_msleep(100);
    } while (0 == g_timer_cb_flag);
    time_sum_1 = 1000000 * (end.tv_sec - start_1.tv_sec) + (end.tv_usec - start_1.tv_usec);
    printf("end - start = %lu us\n",time_sum_1);
    if (time_sum_1 >= 4500000 && time_sum_1 <= 5500000){
        ret2_result = 0;
    }
    end.tv_sec = 0;
    end.tv_usec = 0;


    int32_t ret3 = -1;
    int32_t ret3_result = -1;
    g_timer_cb_flag = 0;
    timer_handle5.config.period += 4500000;
    ret3 = hal_timer_para_chg(&timer_handle5, timer_handle5.config);
    gettimeofday(&start_2, NULL);
    LOGI(TAG, "<2>start timeout is %d us\n", timer_handle5.config.period);
    // int32_t start_2time = -1;
    // start_2time = 1000000 * start_2.tv_sec + start_2.tv_usec;
    // printf("start timeout is %d us\n",start_2time);
    tmpcnt1 = 0;
    do {
        if (tmpcnt1++ > 600) {
            LOGI(TAG, "60s timeout, not enter callback function\n");
            break;
        }

        aos_msleep(100);
    } while (0 == g_timer_cb_flag);
    hal_timer_stop(&timer_handle5);
    time_sum_2 = 1000000 * (end.tv_sec - start_2.tv_sec) + (end.tv_usec - start_2.tv_usec);
    printf("end - start = %lu us\n",time_sum_2);
    if (time_sum_2 >= 9000000 && time_sum_2 <= 10000000){
        ret3_result = 0;
    }
    end.tv_sec = 0;
    end.tv_usec = 0;


    int32_t ret4 = -1;
    ret4 = hal_timer_finalize(&timer_handle5);

    if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0 && ret3_result == 0 && ret4 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_timer_errorcase(){
    int32_t ret1 = -1;
    timer_dev_t timer_handle1={
        .port                 = 0,
        .config={
            .cb            = timer_callback_func,
            .period        = 0,         
            .reload_mode   = TIMER_RELOAD_MANU
        }
    };
    ret1 = hal_timer_init(NULL);
    
    int32_t ret2 = -1;
    ret2 = hal_timer_start(NULL);
    
    int32_t ret3 = -1;
    ret3 = hal_timer_para_chg(NULL, timer_handle1.config);

    int32_t ret4 = -1;
    ret4 = hal_timer_finalize(NULL);

    if (ret1 != 0 && ret2 != 0 && ret3 != 0 && ret4 != 0) {
            AT_BACK_ERR();
    }
    else{
            AT_BACK_OK();
    }
}

void test_hal_timer(char *cmd, int type, char *data)
{
    if (strcmp((const char *)data, "handle1") == 0) {
        aos_hal_timer_handle1();
    }else if (strcmp((const char *)data, "handle2") == 0) {
        aos_hal_timer_handle2();
    }else if (strcmp((const char *)data, "handle3") == 0) {
        aos_hal_timer_handle3();
    }else if (strcmp((const char *)data, "handle4") == 0) {
        aos_hal_timer_handle4();
    }else if (strcmp((const char *)data, "handle5") == 0) {
        aos_hal_timer_handle5();
    }else if (strcmp((const char *)data, "error") == 0) {
        aos_hal_timer_errorcase();
    }else {
        printf("error\r\n");
    }
}
