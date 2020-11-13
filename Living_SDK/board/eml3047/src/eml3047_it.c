/*
 / _____)             _              | |
 ( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
 (______/|_____)_|_|_| \__)_____)\____)_| |_|
 (C)2013 Semtech

 Description: Bleeper board GPIO driver implementation

 License: Revised BSD License, see LICENSE.TXT file include in the project

 Maintainer: Miguel Luis and Gregory Cristian
 */
/******************************************************************************
 * @file    eml3047_it.c
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   manages interupt
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "eml3047_it.h"
#include "low_power.h"
#include "vcom.h"

#include <k_api.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

void RTC_IRQHandler( void )
{
    HW_RTC_IrqHandler( );
}

void EXTI0_1_IRQHandler( void )
{
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_0 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_1 );
}

void EXTI2_3_IRQHandler( void )
{
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_2 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_3 );
}

void EXTI4_15_IRQHandler( void )
{
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_4 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_5 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_6 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_7 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_8 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_9 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_10 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_11 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_12 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_13 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_14 );
    HW_GPIO_EXTI_IRQHandler( GPIO_PIN_15 );
}

void UARTX_IRQHandler( void )
{
    vcom_IRQHandler( );
}

void USART4_5_IRQHandler(void)
{
    int rx_ready = 0;
    char rx;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    if ( LL_USART_IsActiveFlag_RXNE(USART4) && (LL_USART_IsEnabledIT_RXNE(USART4 ) != RESET) )
    {
        /* no need to clear the RXNE flag because it is auto cleared by reading the data*/
        rx = LL_USART_ReceiveData8( USART4 );
        rx_ready = 1;
    }
    if (rx_ready) {
#ifdef CONFIG_LINKWAN_AT
        extern void linkwan_serial_input(uint8_t cmd);
        linkwan_serial_input(rx);
#endif
    }

    RHINO_CPU_INTRPT_ENABLE();
}

int linkwan_serial_output(uint8_t *buffer, int len)
{
    int index;

    for (index = 0; index < len; index++ ) {
        LL_USART_ClearFlag_TC(USART4);
        LL_USART_TransmitData8(USART4, buffer[index]);
        while (LL_USART_IsActiveFlag_TC(USART4) != SET) {
            ;
        }
    }
    LL_USART_ClearFlag_TC(USART4);
    return len;
}

/* Private functions ---------------------------------------------------------*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
