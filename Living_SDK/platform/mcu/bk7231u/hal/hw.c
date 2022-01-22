/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <k_api.h>
#include <aos/aos.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <hal/wifi.h>

#include "include.h"
#include "uart_pub.h"
#include "bk_timer_pub.h"
#include "drv_model_pub.h"

#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

static void _timer_cb(void *timer, void *arg)
{
    timer_dev_t *tmr = arg;
    tmr->config.cb(tmr->config.arg);
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), us2tick(tim->config.period), tim, 0);
    }
    else {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), 0, tim, 0);
    }
}

int32_t hal_timer_start(timer_dev_t *tim)
{
    krhino_timer_start((ktimer_t **)&tim->priv);
}

void hal_timer_stop(timer_dev_t *tmr)
{
    krhino_timer_stop(tmr->priv);
    krhino_timer_dyn_del(tmr->priv);
    tmr->priv = NULL;
}

#if 1
static timer_dev_t g_tmr;

static void _timer_us_cb(uint8_t param)
{
	if(g_tmr.config.reload_mode == TIMER_RELOAD_MANU)
	{
		sddev_control(TIMER_DEV_NAME, CMD_TIMER_UNIT_DISABLE, &g_tmr.port);
	}
	if(g_tmr.config.cb)
	{
		g_tmr.config.cb(g_tmr.config.arg);
	}
}

int32_t hal_timer_us_init(timer_dev_t *tim)
{
	timer_param_t param;

	if(tim == NULL)
	{
		os_printf("timer is NULL.\r\n");
		return -1;
	}
	
	if(tim->port > 2)
	{
		os_printf("timer port error: %d.\r\n", tim->port);
		return -2;
	}

	g_tmr.port = tim->port;
	g_tmr.config.reload_mode = tim->config.reload_mode;
	g_tmr.config.cb = tim->config.cb;
	g_tmr.config.arg = tim->config.arg;
	
	param.channel = tim->port;
	param.div = 1;
	param.period = tim->config.period;
	param.t_Int_Handler= _timer_us_cb;
        
	sddev_control(TIMER_DEV_NAME, CMD_TIMER_INIT_PARAM_US, &param);    

	return 0;
}

int32_t hal_timer_us_start(timer_dev_t *tim)
{
	uint32_t chan;
	
	if(tim == NULL)
	{
		os_printf("timer is NULL.\r\n");
		return -1;
	}
	
	if(tim->port > 2)
	{
		os_printf("timer port error: %d.\r\n", tim->port);
		return -2;
	}

	chan = tim->port;
	sddev_control(TIMER_DEV_NAME, CMD_TIMER_UNIT_ENABLE, &chan);
	return 0;
}

void hal_timer_us_stop(timer_dev_t *tim)
{
	uint32_t chan;
	
	if(tim == NULL)
	{
		os_printf("timer is NULL.\r\n");
		return -1;
	}
	
	if(tim->port > 2)
	{
		os_printf("timer port error: %d.\r\n", tim->port);
		return -2;
	}

	chan = tim->port;
	sddev_control(TIMER_DEV_NAME, CMD_TIMER_UNIT_DISABLE, &chan);
}
#endif

extern hal_wifi_module_t sim_aos_wifi_beken;
void hw_start_hal(void)
{
    printf("start-----------hal\n");
    hal_wifi_register_module(&sim_aos_wifi_beken);
#ifdef CONFIG_AOS_MESH
    extern void beken_wifi_mesh_register(void);
    beken_wifi_mesh_register();
#endif
}
