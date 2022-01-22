/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "hal/soc/soc.h"
#include "hal_hwtimer.h"

#define TAG "hw"

#define TIMER_MAX_NUM   5

static hw_timer_t *handle[TIMER_MAX_NUM];

int32_t hal_timer_init(timer_dev_t *tim)
{
    if (tim == NULL || tim->port >= TIMER_MAX_NUM) {
        return -1;
    }
    
    return hal_hwtimer_init();
}

int32_t hal_timer_start(timer_dev_t *tim)
{
    int repeat;
    if (tim == NULL || tim->port >= TIMER_MAX_NUM || tim->config.period < 1000) {
        return -1;
    }
   
    if (tim->config.reload_mode == TIMER_RELOAD_MANU) {
        repeat = 0;
    } else if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        repeat = 1;
    }

    handle[tim->port] = hal_hwtimer_create(tim->config.period/1000, tim->config.cb, tim->config.arg, repeat);
    return 0;
}

void hal_timer_stop(timer_dev_t *tim)
{
   if (tim == NULL || tim->port >= TIMER_MAX_NUM) {
        return;
   }
   
   hal_hwtimer_delete(handle[tim->port]);
   return;
}

int32_t hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    if (tim == NULL || tim->port >= TIMER_MAX_NUM || para.period < 1000) {
        return -1;
    }

    return hal_hwtimer_change_period(handle[tim->port], para.period/1000);
}

int32_t hal_timer_finalize(timer_dev_t *tim)
{
   if (tim == NULL || tim->port >= TIMER_MAX_NUM) {
        return -1;
   }
 
   return 0;
}
