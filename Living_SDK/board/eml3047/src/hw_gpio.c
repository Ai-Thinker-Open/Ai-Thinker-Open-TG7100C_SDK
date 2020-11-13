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
 * @file    hw_gpio.c
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   driver for GPIO
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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define EXTI_MODE ((uint32_t)0x10000000U)

/* Private variables ---------------------------------------------------------*/
static GpioIrqHandler *GpioIrq[16] = { NULL };

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief Get the position of the bit set in the GPIO_Pin
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @retval the position of the bit
 */
static uint8_t HW_GPIO_GetBitPos( uint16_t GPIO_Pin );

/*
 * @brief Helper to initialize a single pin, given its position from 0 to 16.
 *
 * @param  GPIOx: where x can be (A..E and H)
 * @param  position: position (from 0 to 15) of the single pin t
 * @param  initStruct:  GPIO_InitTypeDef init structure
 * @retval none
 * @note   The supported modes ares:
 *     GPIO_MODE_ANALOG
 *     GPIO_MODE_INPUT
 *     GPIO_MODE_IT_RISING
 *     GPIO_MODE_OUTPUT_PP
 *     GPIO_MODE_AF_PP
 */
static void HW_GPIO_Init_singlepin( GPIO_TypeDef *GPIOx, int position, GPIO_InitTypeDef *GPIO_Init );

/**
 * @brief  Initializes the given GPIO object, similarly to the HAL function HAL_GPIO_Init()
 *
 * @param  GPIOx: where x can be (A..E and H)
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @param  [IN] initStruct  GPIO_InitTypeDef intit structure
 * @retval none
 * @note   The supported modes ares:
 *     GPIO_MODE_ANALOG
 *     GPIO_MODE_INPUT
 *     GPIO_MODE_IT_RISING
 *     GPIO_MODE_OUTPUT_PP
 *     GPIO_MODE_AF_PP
 */
static void HW_GPIO_Init_HAL( GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_InitTypeDef *GPIO_Init );

/**
 * @brief  Deinitialize the given GPIO object given its single pin position
 * @param  GPIOx: where x can be (A..E and H)
 * @param  position: the position (0..15) of the pin to uninitialize
 * @retval none
 */
static void HW_GPIO_DeInit_singlepin( GPIO_TypeDef *GPIOx, int position );

/**
 * @brief  Gets IRQ number as a function of the GPIO_Pin.
 * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
 * @retval IRQ number
 */
static IRQn_Type GetIRQn( uint16_t GPIO_Pin );

/* Exported functions ---------------------------------------------------------*/

void HW_GPIO_Init( GPIO_TypeDef *port, uint16_t GPIO_Pin, GPIO_InitTypeDef *initStruct )
{

    RCC_GPIO_CLK_ENABLE( (uint32_t )port );
    HW_GPIO_Init_HAL( port, GPIO_Pin, initStruct );
}

void HW_GPIO_SetIrq( GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t prio, GpioIrqHandler *irqHandler )
{
    IRQn_Type IRQnb;

    uint32_t BitPos = HW_GPIO_GetBitPos( GPIO_Pin );

    if ( irqHandler != NULL )
    {
        GpioIrq[BitPos] = irqHandler;

        IRQnb = GetIRQn( GPIO_Pin );

        NVIC_SetPriority( IRQnb, prio );

        NVIC_EnableIRQ( IRQnb );
    }
}

void HW_GPIO_IrqHandler( uint16_t GPIO_Pin )
{
    uint32_t BitPos = HW_GPIO_GetBitPos( GPIO_Pin );

    if ( GpioIrq[BitPos] != NULL )
    {
        GpioIrq[BitPos]( );
    }
}

uint32_t HW_GPIO_Read( GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin )
{
    uint32_t pin_value;

    pin_value = LL_GPIO_ReadInputPort( GPIOx );
    if ( (pin_value & GPIO_Pin) != (uint32_t) RESET )
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

void HW_GPIO_DeInit( GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin )
{
    int position;
    assert_param( IS_GPIO_PIN_AVAILABLE(GPIOx, (GPIO_Pin)) );

    position = 0;
    while ( GPIO_Pin != 0 )
    {
        if ( GPIO_Pin & 0x1 )
        {
            HW_GPIO_DeInit_singlepin( GPIOx, position );
        }
        position++;
        GPIO_Pin = GPIO_Pin >> 1;
    }
}

void HW_GPIO_EXTI_IRQHandler( uint16_t GPIO_Pin )
{
    /* EXTI line interrupt detected */
    if ( LL_EXTI_ReadFlag_0_31( GPIO_Pin ) != RESET )
    {
        LL_EXTI_ClearFlag_0_31( GPIO_Pin );
        HW_GPIO_IrqHandler( GPIO_Pin );
    }
}

/* Private functions ---------------------------------------------------------*/

static uint8_t HW_GPIO_GetBitPos( uint16_t GPIO_Pin )
{
    uint8_t PinPos = 0;

    if ( (GPIO_Pin & 0xFF00) != 0 )
    {
        PinPos |= 0x8;
    }
    if ( (GPIO_Pin & 0xF0F0) != 0 )
    {
        PinPos |= 0x4;
    }
    if ( (GPIO_Pin & 0xCCCC) != 0 )
    {
        PinPos |= 0x2;
    }
    if ( (GPIO_Pin & 0xAAAA) != 0 )
    {
        PinPos |= 0x1;
    }

    return PinPos;
}

static void HW_GPIO_Init_singlepin( GPIO_TypeDef *GPIOx, int position, GPIO_InitTypeDef *GPIO_Init )
{
    uint16_t GPIO_Pin;
    uint32_t temp;

    /* Check for specific constraints on this implementation */
    assert_param( (GPIO_Init->Mode == GPIO_MODE_ANALOG) ||
                  (GPIO_Init->Mode == GPIO_MODE_INPUT) ||
                  (GPIO_Init->Mode == GPIO_MODE_IT_RISING) ||
                  (GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) ||
                  (GPIO_Init->Mode == GPIO_MODE_AF_PP) );

    GPIO_Pin = 1 << position;

    /* In case of Alternate function mode selection */
    if ( GPIO_Init->Mode == GPIO_MODE_AF_PP )
    {
        /* Check if the Alternate function is compliant with the GPIO in use */
        assert_param( IS_GPIO_AF_AVAILABLE(GPIOx, (GPIO_Init->Alternate)) );

        /* Configure Alternate function mapped with the current IO */
        /* the following code could be used to use LL functions, but the code size
         * is then much higher
         *     if (position <= 7)  LL_GPIO_SetAFPin_0_7(GPIOx, GPIO_Pin, GPIO_Init->Alternate);
         *     else                LL_GPIO_SetAFPin_8_15(GPIOx, GPIO_Pin, GPIO_Init->Alternate);
         */
        temp = GPIOx->AFR[position >> 3U];
        temp &= ~((uint32_t) 0xFU << ((uint32_t) (position & (uint32_t) 0x07U) * 4U));
        temp |= ((uint32_t) (GPIO_Init->Alternate) << (((uint32_t) position & (uint32_t) 0x07U) * 4U));
        GPIOx->AFR[position >> 3U] = temp;
    }

    /* In case of Output or Alternate function mode selection */
    if ( (GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) )
    {
        /* Check the Speed parameter */
        assert_param( IS_GPIO_SPEED(GPIO_Init->Speed) );

        /* Configure the IO Speed */
        LL_GPIO_SetPinSpeed( GPIOx, GPIO_Pin, GPIO_Init->Speed );

        /* Configure the IO Output Type */
        LL_GPIO_SetPinOutputType( GPIOx, GPIO_Pin, LL_GPIO_OUTPUT_PUSHPULL );
    }

    /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
    LL_GPIO_SetPinMode( GPIOx, GPIO_Pin, GPIO_Init->Mode );

    /* Activate the Pull-up or Pull down resistor for the current IO */
    LL_GPIO_SetPinPull( GPIOx, GPIO_Pin, GPIO_Init->Pull );

    /*--------------------- EXTI Mode Configuration ------------------------*/
    /* Configure the External Interrupt or event for the current IO */
    if ( (GPIO_Init->Mode & EXTI_MODE) == EXTI_MODE )
    {
        /* Enable SYSCFG Clock */
        LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_SYSCFG );

        LL_SYSCFG_SetEXTISource( GPIO_GET_INDEX( GPIOx ), (((4U * (position & 0x03U)) << 16U) | (position >> 2U)) );

        /* Enable Raising IT, disable event and falling */
        LL_EXTI_EnableIT_0_31( GPIO_Pin );
        LL_EXTI_DisableEvent_0_31( GPIO_Pin );
        LL_EXTI_EnableRisingTrig_0_31( GPIO_Pin );
        LL_EXTI_DisableFallingTrig_0_31( GPIO_Pin );
    }
}

static void HW_GPIO_Init_HAL( GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_InitTypeDef *GPIO_Init )
{
    int position;

    /* Check the parameters */
    assert_param( IS_GPIO_MODE(GPIO_Init->Mode) );
    assert_param( IS_GPIO_PULL(GPIO_Init->Pull) );
    assert_param( IS_GPIO_PIN_AVAILABLE(GPIOx, GPIO_Pin) );

    position = 0;
    while ( GPIO_Pin != 0 )
    {
        if ( GPIO_Pin & 0x1 )
        {
            HW_GPIO_Init_singlepin( GPIOx, position, GPIO_Init );
        }
        position++;
        GPIO_Pin = GPIO_Pin >> 1;
    }
}

static void HW_GPIO_DeInit_singlepin( GPIO_TypeDef *GPIOx, int position )
{
    uint16_t GPIO_Pin = 1 << position;
    uint32_t tmp;

    /* Configure the port pins */
    /*------------------------- GPIO Mode Configuration --------------------*/
    /* Configure IO Direction in Input Floting Mode */
    GPIOx->MODER &= ~(GPIO_MODER_MODE0 << (position * 2U));

    /* Configure the default Alternate Function in current IO */
    GPIOx->AFR[position >> 3U] &= ~((uint32_t) 0xFU << ((uint32_t) (position & (uint32_t) 0x07U) * 4U));

    /* Configure the default value for IO Speed */
    GPIOx->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEED0 << (position * 2U));

    /* Configure the default value IO Output Type */
    GPIOx->OTYPER &= ~(GPIO_OTYPER_OT_0 << position);

    /* Deactivate the Pull-up oand Pull-down resistor for the current IO */
    GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPD0 << (position * 2U));

    /*------------------------- EXTI Mode Configuration --------------------*/
    /* Clear the External Interrupt or Event for the current IO */

    tmp = SYSCFG->EXTICR[position >> 2U];
    tmp &= (((uint32_t) 0x0FU) << (4U * (position & 0x03U)));
    if ( tmp == (GPIO_GET_INDEX(GPIOx) << (4U * (position & 0x03U))) )
    {
        tmp = ((uint32_t) 0x0FU) << (4U * (position & 0x03U));
        SYSCFG->EXTICR[position >> 2U] &= ~tmp;

        /* Clear EXTI line configuration */
        LL_EXTI_DisableIT_0_31( GPIO_Pin );
        LL_EXTI_DisableEvent_0_31( GPIO_Pin );

        /* Clear Rising Falling edge configuration */
        LL_EXTI_DisableRisingTrig_0_31( GPIO_Pin );
        LL_EXTI_DisableFallingTrig_0_31( GPIO_Pin );
    }
}

static IRQn_Type GetIRQn( uint16_t GPIO_Pin )
{
    switch ( GPIO_Pin )
    {
        case GPIO_PIN_0:
            case GPIO_PIN_1:
            return EXTI0_1_IRQn;
        case GPIO_PIN_2:
            case GPIO_PIN_3:
            return EXTI2_3_IRQn;
        case GPIO_PIN_4:
            case GPIO_PIN_5:
            case GPIO_PIN_6:
            case GPIO_PIN_7:
            case GPIO_PIN_8:
            case GPIO_PIN_9:
            case GPIO_PIN_10:
            case GPIO_PIN_11:
            case GPIO_PIN_12:
            case GPIO_PIN_13:
            case GPIO_PIN_14:
            case GPIO_PIN_15:
            default:
            return EXTI4_15_IRQn;
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
