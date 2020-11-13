/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <k_api.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <aos/aos.h>
#include <hal/hal.h>
#include "board.h"
#include "fsl_gpio.h"
#include "pin_mux.h"


#define AOS_START_STACK 256

extern int application_start(int argc, char **argv);
extern int vfs_init(void);
extern int vfs_device_init(void);
extern uart_dev_t uart_0;

ktask_t *g_aos_app;
static void sys_init(void)
{

#ifdef AOS_LOOP
    vfs_init();
    vfs_device_init();
    aos_loop_init();
#endif
    application_start(0, NULL);	
}

static void platform_init(void)
{
    BOARD_InitPins();
    BOARD_InitDEBUG_UART();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
}


#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)


void hal_reboot(void)
{
    NVIC_SystemReset();
}

int main(void)
{
   uint32_t core_frequency = 0;
   platform_init();
	
   aos_init();
   hal_uart_init(&uart_0);
   krhino_task_dyn_create(&g_aos_app, "aos-init", 0, 10, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);
   core_frequency = CLOCK_GetCoreSysClkFreq();

   SysTick_Config(core_frequency / RHINO_CONFIG_TICKS_PER_SECOND);
   aos_start();
   return 0;
}
