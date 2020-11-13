/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdlib.h>

#include "hal/soc/soc.h"
#include "drv/tmr/hal_tm.h"

timer_dev_t *sv6266_timer_handler_ptr[6];

/*
 * The range of timers for sv6266 is [0, 65535], ID 0/1/2 is a microsecond timer, and 5/6/7 is a millisecond timer.
 */
const static TM_ID_E TM_ID_MAP[] = {
    1,                ///< ID for microsecond timer 0.
    2,                ///< ID for microsecond timer 1.
    3,                ///< ID for microsecond timer 2.
    5,                ///< ID for millisecond timer 3.
    6,                ///< ID for millisecond timer 4.
    7                 ///< ID for millisecond timer 5.
};

static void sv6266_timer_handler_0(void *arg)
{
    drv_tmr_clear_interrupt_status((TM_ID_E) TM_ID_MAP[sv6266_timer_handler_ptr[0]->port]);
    sv6266_timer_handler_ptr[0]->config.cb(sv6266_timer_handler_ptr[0]->config.arg);
}

static void sv6266_timer_handler_1(void *arg)
{
    drv_tmr_clear_interrupt_status((TM_ID_E) TM_ID_MAP[sv6266_timer_handler_ptr[1]->port]);
    sv6266_timer_handler_ptr[1]->config.cb(sv6266_timer_handler_ptr[1]->config.arg);
}

static void sv6266_timer_handler_2(void *arg)
{
    drv_tmr_clear_interrupt_status((TM_ID_E) TM_ID_MAP[sv6266_timer_handler_ptr[2]->port]);
    sv6266_timer_handler_ptr[2]->config.cb(sv6266_timer_handler_ptr[2]->config.arg);
}

static void sv6266_timer_handler_3(void *arg)
{
    drv_tmr_clear_interrupt_status((TM_ID_E) TM_ID_MAP[sv6266_timer_handler_ptr[3]->port]);
    sv6266_timer_handler_ptr[3]->config.cb(sv6266_timer_handler_ptr[3]->config.arg);
}

static void sv6266_timer_handler_4(void *arg)
{
    drv_tmr_clear_interrupt_status((TM_ID_E) TM_ID_MAP[sv6266_timer_handler_ptr[4]->port]);
    sv6266_timer_handler_ptr[4]->config.cb(sv6266_timer_handler_ptr[4]->config.arg);
}

static void sv6266_timer_handler_5(void *arg)
{
    drv_tmr_clear_interrupt_status((TM_ID_E) TM_ID_MAP[sv6266_timer_handler_ptr[5]->port]);
    sv6266_timer_handler_ptr[5]->config.cb(sv6266_timer_handler_ptr[5]->config.arg);
}

static isr_func sv6266_timer_handler[] = {
    sv6266_timer_handler_0,sv6266_timer_handler_1,sv6266_timer_handler_2,
    sv6266_timer_handler_3,sv6266_timer_handler_4,sv6266_timer_handler_5
};

int32_t hal_timer_init(timer_dev_t *tim)
{
    int32_t retval;

    if (tim == NULL) {
        return -1;
    }

    if (tim->port < 0 || tim->port > 5) {
        return -1;
    }

    retval = hal_tm_init((TM_ID_E) TM_ID_MAP[tim->port]);
    if (retval != 0) {
        return retval;
    }

    retval = hal_tm_sw_rst((TM_ID_E) TM_ID_MAP[tim->port]);
    if (retval != 0) {
        return retval;
    }

    if (tim->config.cb != NULL) {
        sv6266_timer_handler_ptr[tim->port] = tim;
        retval = hal_tm_register_irq_handler((TM_ID_E) TM_ID_MAP[tim->port], (isr_func) sv6266_timer_handler[tim->port]);
    }

    return retval;
}

int32_t hal_timer_start (timer_dev_t *tim)
{
    TM_MODE_E reload = 0;

    if (tim == NULL) {
        return -1;
    }

    if (tim->port < 0 || tim->port > 5) {
        return -1;
    }

    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        reload = (TM_MODE_E) 1;
    } else {
        reload = (TM_MODE_E) 0;
    }

    return hal_tm_enable ((TM_ID_E) TM_ID_MAP[tim->port], reload, tim->config.period);
}

int32_t hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    TM_MODE_E reload = 0;

    if (tim == NULL) {
        return -1;
    }

    if (tim->port < 0 || tim->port > 5) {
        return -1;
    }

    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        reload = (TM_MODE_E) 1;
    } else {
        reload = (TM_MODE_E) 0;
    }

    hal_tm_disable ((TM_ID_E) TM_ID_MAP[tim->port]);

    tim->config.period = para.period;
    tim->config.reload_mode = para.reload_mode;
    tim->config.arg = para.arg;

    if (tim->port < 0 || tim->port > 5) {
        return -1;
    }

    return hal_tm_enable ((TM_ID_E) TM_ID_MAP[tim->port], reload, tim->config.period);
}

void hal_timer_stop(timer_dev_t *tim)
{
    if (tim == NULL) {
        return;
    }

    if (tim->port < 0 || tim->port > 5) {
        return;
    }

    hal_tm_disable ((TM_ID_E) TM_ID_MAP[tim->port]);
}

int32_t hal_timer_finalize(timer_dev_t *tim)
{
    if (tim == NULL) {
        return -1;
    }

    if (tim->port < 0 || tim->port > 5) {
        return -1;
    }

    return hal_tm_deinit ((TM_ID_E) TM_ID_MAP[tim->port]);
}
