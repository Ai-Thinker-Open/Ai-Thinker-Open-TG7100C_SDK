/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "soc_init.h"
#include "hal.h"
#include "stm32l4xx_hal.h"
#include "hal_timer_stm32l4.h"
#include "stm32l4xx_hal_adc.h"

/* Init and deInit function for adc1 */
static int32_t timer3_init(timer_dev_t *tim);

/* function used to transform hal para to stm32l4 para */
int32_t timer_reload_mode_transform(uint8_t reload_mode_hal, uint8_t *reload_mode_stm32l4);

/* handle for adc */
TIM_HandleTypeDef timer3_handle;
uint32_t uwPrescalerValue = 0;
hal_timer_cb_t pFun_timer3 = NULL;
void * arg_timer3 = NULL;

void TIM3_IRQHandler(void)
{
    krhino_intrpt_enter();
    HAL_TIM_IRQHandler(&timer3_handle);
    krhino_intrpt_exit();
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    int32_t ret = -1;

    if (tim == NULL) {
       return -1;
    }

    /*init adc handle*/
    memset(&timer3_handle, 0, sizeof(timer3_handle));

    switch (tim->port) {
        case PORT_TIMER3:
      	    tim->priv = &timer3_handle;
            ret = timer3_init(tim);
            break;

    /* if other timer exist add init code here */

        default:
            break;
    }

    return ret;

}

int32_t timer3_init(timer_dev_t *tim)
{
    int32_t ret = -1;

    pFun_timer3 = tim->config.cb;
    arg_timer3 = tim->config.arg;	

    /* Set TIMx instance */
    timer3_handle.Instance = TIM3;

  	/* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
    uwPrescalerValue = (uint32_t)(SystemCoreClock / 10000) - 1;
    timer3_handle.Init.Period            = ((tim->config.period / 1000000) * 10000) - 1;
    timer3_handle.Init.Prescaler         = uwPrescalerValue;
    timer3_handle.Init.ClockDivision     = 0;
    timer3_handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    timer3_handle.Init.RepetitionCounter = 0;
    ret = timer_reload_mode_transform(tim->config.reload_mode, 
	                             (uint8_t *)&timer3_handle.Init.AutoReloadPreload);
	
    if (ret == 0) {
        ret = HAL_TIM_Base_Init(&timer3_handle);
    }

		return ret;
}

int32_t hal_timer_start(timer_dev_t *tim)
{
    int32_t ret = -1;

    if (tim != NULL)
		{
		    ret = HAL_TIM_Base_Start_IT((TIM_HandleTypeDef *)tim->priv);
		}

    return ret;
}

void hal_timer_stop(timer_dev_t *tim)
{
    if (tim != NULL)
		{
		    HAL_TIM_Base_Stop_IT((TIM_HandleTypeDef *)tim->priv);
		}
}

int32_t hal_timer_finalize(timer_dev_t *tim)
{
    int32_t ret = -1;

    if (tim != NULL)
    {
        ret = HAL_TIM_Base_DeInit((TIM_HandleTypeDef *)tim->priv);
    }

    return ret;
}

int32_t timer_reload_mode_transform(uint8_t reload_mode_hal,
        uint8_t *reload_mode_stm32l4)
{
    uint32_t reload_mode = 0;
    int32_t	ret = 0;

    if(reload_mode_hal == TIMER_RELOAD_AUTO)
    {
        reload_mode = TIM_AUTORELOAD_PRELOAD_ENABLE;
    }
    else if(reload_mode_hal == TIMER_RELOAD_MANU)
    {
        reload_mode = TIM_AUTORELOAD_PRELOAD_DISABLE;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *reload_mode_stm32l4 = reload_mode;
    }

    return ret;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3) {
        pFun_timer3(arg_timer3);
		}
}
