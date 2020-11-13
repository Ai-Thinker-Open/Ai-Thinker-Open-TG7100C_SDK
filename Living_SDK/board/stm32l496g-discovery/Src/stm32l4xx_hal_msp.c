/**
  ******************************************************************************
  * File Name          : stm32l4xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

extern void _Error_Handler(char *, int);
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

static uint32_t HAL_RCC_ADC_CLK_ENABLED=0;

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* Peripheral clock enable */
    HAL_RCC_ADC_CLK_ENABLED++;
    if(HAL_RCC_ADC_CLK_ENABLED==1){
      __HAL_RCC_ADC_CLK_ENABLE();
    }
  
    /**ADC1 GPIO Configuration    
    PC4     ------> ADC1_IN13
    PC3     ------> ADC1_IN4 
    */
    GPIO_InitStruct.Pin = ARD_A0_Pin|ARD_A2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
  else if(hadc->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspInit 0 */

  /* USER CODE END ADC2_MspInit 0 */
    /* Peripheral clock enable */
    HAL_RCC_ADC_CLK_ENABLED++;
    if(HAL_RCC_ADC_CLK_ENABLED==1){
      __HAL_RCC_ADC_CLK_ENABLE();
    }
  
    /**ADC2 GPIO Configuration    
    PC0     ------> ADC2_IN1
    PC1     ------> ADC2_IN2
    PA4     ------> ADC2_IN9
    PA1     ------> ADC2_IN6 
    */
    GPIO_InitStruct.Pin = ARD_A5_Pin|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = STMOD_ADC_Pin|ARD_A4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC2_MspInit 1 */

  /* USER CODE END ADC2_MspInit 1 */
  }
  else if(hadc->Instance==ADC3)
  {
  /* USER CODE BEGIN ADC3_MspInit 0 */

  /* USER CODE END ADC3_MspInit 0 */
    /* Peripheral clock enable */
    HAL_RCC_ADC_CLK_ENABLED++;
    if(HAL_RCC_ADC_CLK_ENABLED==1){
      __HAL_RCC_ADC_CLK_ENABLE();
    }
  
    /**ADC3 GPIO Configuration    
    PF10     ------> ADC3_IN13 
    */
    GPIO_InitStruct.Pin = ARD_A3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ARD_A3_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC3_MspInit 1 */

  /* USER CODE END ADC3_MspInit 1 */
  }

}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{

  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    /* Be sure that all peripheral instances that share the same clock need to be disabled */
    /**  HAL_RCC_ADC_CLK_ENABLED--;
    *  if(HAL_RCC_ADC_CLK_ENABLED==0){
    *    __HAL_RCC_ADC_CLK_DISABLE();
    **/
  
    /**ADC1 GPIO Configuration    
    PC4     ------> ADC1_IN13
    PC3     ------> ADC1_IN4 
    */
    HAL_GPIO_DeInit(GPIOC, ARD_A0_Pin|ARD_A2_Pin);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
  else if(hadc->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspDeInit 0 */

  /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    /* Be sure that all peripheral instances that share the same clock need to be disabled */
    /**  HAL_RCC_ADC_CLK_ENABLED--;
    *  if(HAL_RCC_ADC_CLK_ENABLED==0){
    *    __HAL_RCC_ADC_CLK_DISABLE();
    **/
  
    /**ADC2 GPIO Configuration    
    PC0     ------> ADC2_IN1
    PC1     ------> ADC2_IN2
    PA4     ------> ADC2_IN9
    PA1     ------> ADC2_IN6 
    */
    HAL_GPIO_DeInit(GPIOC, ARD_A5_Pin|GPIO_PIN_1);

    HAL_GPIO_DeInit(GPIOA, STMOD_ADC_Pin|ARD_A4_Pin);

  /* USER CODE BEGIN ADC2_MspDeInit 1 */

  /* USER CODE END ADC2_MspDeInit 1 */
  }
  else if(hadc->Instance==ADC3)
  {
  /* USER CODE BEGIN ADC3_MspDeInit 0 */

  /* USER CODE END ADC3_MspDeInit 0 */
    /* Peripheral clock disable */
    /* Be sure that all peripheral instances that share the same clock need to be disabled */
    /**  HAL_RCC_ADC_CLK_ENABLED--;
    *  if(HAL_RCC_ADC_CLK_ENABLED==0){
    *    __HAL_RCC_ADC_CLK_DISABLE();
    **/
  
    /**ADC3 GPIO Configuration    
    PF10     ------> ADC3_IN13 
    */
    HAL_GPIO_DeInit(ARD_A3_GPIO_Port, ARD_A3_Pin);

  /* USER CODE BEGIN ADC3_MspDeInit 1 */

  /* USER CODE END ADC3_MspDeInit 1 */
  }

}

void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hdcmi->Instance==DCMI)
  {
  /* USER CODE BEGIN DCMI_MspInit 0 */

  /* USER CODE END DCMI_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_DCMI_CLK_ENABLE();
  
    /**DCMI GPIO Configuration    
    PH14     ------> DCMI_D4
    PI7     ------> DCMI_D7
    PH12     ------> DCMI_D3
    PI4     ------> DCMI_D5
    PH9     ------> DCMI_D0
    PE5     ------> DCMI_D6
    PI5     ------> DCMI_VSYNC
    PH5     ------> DCMI_PIXCLK
    PH10     ------> DCMI_D1
    PH11     ------> DCMI_D2
    PH8     ------> DCMI_HSYNC 
    */
    GPIO_InitStruct.Pin = DCMI_D4_Pin|DCMI_D3_Pin|DCMI_D0_Pin|DCMI_PIXCK_Pin 
                          |DCMI_D1_Pin|DCMI_D2_Pin|DCMI_HSYNC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF10_DCMI;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DCMI_D7_Pin|DCMI_D5_Pin|DCMI_VSYNC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF10_DCMI;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DCMI_D6_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF10_DCMI;
    HAL_GPIO_Init(DCMI_D6_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN DCMI_MspInit 1 */

  /* USER CODE END DCMI_MspInit 1 */
  }

}

void HAL_DCMI_MspDeInit(DCMI_HandleTypeDef* hdcmi)
{

  if(hdcmi->Instance==DCMI)
  {
  /* USER CODE BEGIN DCMI_MspDeInit 0 */

  /* USER CODE END DCMI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DCMI_CLK_DISABLE();
  
    /**DCMI GPIO Configuration    
    PH14     ------> DCMI_D4
    PI7     ------> DCMI_D7
    PH12     ------> DCMI_D3
    PI4     ------> DCMI_D5
    PH9     ------> DCMI_D0
    PE5     ------> DCMI_D6
    PI5     ------> DCMI_VSYNC
    PH5     ------> DCMI_PIXCLK
    PH10     ------> DCMI_D1
    PH11     ------> DCMI_D2
    PH8     ------> DCMI_HSYNC 
    */
    HAL_GPIO_DeInit(GPIOH, DCMI_D4_Pin|DCMI_D3_Pin|DCMI_D0_Pin|DCMI_PIXCK_Pin 
                          |DCMI_D1_Pin|DCMI_D2_Pin|DCMI_HSYNC_Pin);

    HAL_GPIO_DeInit(GPIOI, DCMI_D7_Pin|DCMI_D5_Pin|DCMI_VSYNC_Pin);

    HAL_GPIO_DeInit(DCMI_D6_GPIO_Port, DCMI_D6_Pin);

  /* USER CODE BEGIN DCMI_MspDeInit 1 */

  /* USER CODE END DCMI_MspDeInit 1 */
  }

}

static uint32_t DFSDM1_Init = 0;

void HAL_DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef* hdfsdm_channel)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(DFSDM1_Init == 0)
  {
  /* USER CODE BEGIN DFSDM1_MspInit 0 */

  /* USER CODE END DFSDM1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_DFSDM1_CLK_ENABLE();
  
    /**DFSDM1 GPIO Configuration    
    PC7     ------> DFSDM1_DATIN3
    PC2     ------> DFSDM1_CKOUT
    PB12     ------> DFSDM1_DATIN1 
    */
    GPIO_InitStruct.Pin = DATIN3_Pin|DF_CKOUT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DFDATIN1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(DFDATIN1_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN DFSDM1_MspInit 1 */

  /* USER CODE END DFSDM1_MspInit 1 */
  }

}

void HAL_DFSDM_ChannelMspDeInit(DFSDM_Channel_HandleTypeDef* hdfsdm_channel)
{

  DFSDM1_Init-- ;
  if(DFSDM1_Init == 0)
    {
  /* USER CODE BEGIN DFSDM1_MspDeInit 0 */

  /* USER CODE END DFSDM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DFSDM1_CLK_DISABLE();
  
    /**DFSDM1 GPIO Configuration    
    PC7     ------> DFSDM1_DATIN3
    PC2     ------> DFSDM1_CKOUT
    PB12     ------> DFSDM1_DATIN1 
    */
    HAL_GPIO_DeInit(GPIOC, DATIN3_Pin|DF_CKOUT_Pin);

    HAL_GPIO_DeInit(DFDATIN1_GPIO_Port, DFDATIN1_Pin);

  /* USER CODE BEGIN DFSDM1_MspDeInit 1 */

  /* USER CODE END DFSDM1_MspDeInit 1 */
  }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */
  
    /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = I2C1_SCL_Pin|I2C1_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
  else if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */
  
    /**I2C2 GPIO Configuration    
    PH4     ------> I2C2_SCL
    PB14     ------> I2C2_SDA 
    */
    GPIO_InitStruct.Pin = I2C2_SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(I2C2_SCL_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = I2C2_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(I2C2_SDA_GPIO_Port, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{

  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, I2C1_SCL_Pin|I2C1_SDA_Pin);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
  else if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();
  
    /**I2C2 GPIO Configuration    
    PH4     ------> I2C2_SCL
    PB14     ------> I2C2_SDA 
    */
    HAL_GPIO_DeInit(I2C2_SCL_GPIO_Port, I2C2_SCL_Pin);

    HAL_GPIO_DeInit(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin);

  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspInit 0 */

  /* USER CODE END LPUART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_LPUART1_CLK_ENABLE();
  
    /**LPUART1 GPIO Configuration    
    PG8     ------> LPUART1_RX
    PG7     ------> LPUART1_TX 
    */
    GPIO_InitStruct.Pin = ARD_D0_Pin|ARD_D1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN LPUART1_MspInit 1 */

  /* USER CODE END LPUART1_MspInit 1 */
  }
  else if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PB6     ------> USART1_TX
    PG10     ------> USART1_RX
    PG11     ------> USART1_CTS
    PG12     ------> USART1_RTS 
    */
    GPIO_InitStruct.Pin = USART1_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(USART1_TX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = UART1_RX_Pin|UART1_CTS_Pin|UART1_RTS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    /**USART2 GPIO Configuration    
    PD6     ------> USART2_RX
    PA2     ------> USART2_TX 
    */
    GPIO_InitStruct.Pin = USART2_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(USART2_RX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USART2_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(USART2_TX_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspDeInit 0 */

  /* USER CODE END LPUART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPUART1_CLK_DISABLE();
  
    /**LPUART1 GPIO Configuration    
    PG8     ------> LPUART1_RX
    PG7     ------> LPUART1_TX 
    */
    HAL_GPIO_DeInit(GPIOG, ARD_D0_Pin|ARD_D1_Pin);

  /* USER CODE BEGIN LPUART1_MspDeInit 1 */

  /* USER CODE END LPUART1_MspDeInit 1 */
  }
  else if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PB6     ------> USART1_TX
    PG10     ------> USART1_RX
    PG11     ------> USART1_CTS
    PG12     ------> USART1_RTS 
    */
    HAL_GPIO_DeInit(USART1_TX_GPIO_Port, USART1_TX_Pin);

    HAL_GPIO_DeInit(GPIOG, UART1_RX_Pin|UART1_CTS_Pin|UART1_RTS_Pin);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PD6     ------> USART2_RX
    PA2     ------> USART2_TX 
    */
    HAL_GPIO_DeInit(USART2_RX_GPIO_Port, USART2_RX_Pin);

    HAL_GPIO_DeInit(USART2_TX_GPIO_Port, USART2_TX_Pin);

  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }

}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* hqspi)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hqspi->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();
  
    /**QUADSPI GPIO Configuration    
    PB11     ------> QUADSPI_BK1_NCS
    PB0     ------> QUADSPI_BK1_IO1
    PA7     ------> QUADSPI_BK1_IO2
    PB1     ------> QUADSPI_BK1_IO0
    PA3     ------> QUADSPI_CLK
    PA6     ------> QUADSPI_BK1_IO3 
    */
    GPIO_InitStruct.Pin = QSPI_BK1_NCS_Pin|QSPI_BK1_IO1_Pin|QSPI_BK1_IO0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = QSPI_BK1_IO2_Pin|QSPI_CLK_Pin|QSPI_BK1_IO3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }

}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* hqspi)
{

  if(hqspi->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();
  
    /**QUADSPI GPIO Configuration    
    PB11     ------> QUADSPI_BK1_NCS
    PB0     ------> QUADSPI_BK1_IO1
    PA7     ------> QUADSPI_BK1_IO2
    PB1     ------> QUADSPI_BK1_IO0
    PA3     ------> QUADSPI_CLK
    PA6     ------> QUADSPI_BK1_IO3 
    */
    HAL_GPIO_DeInit(GPIOB, QSPI_BK1_NCS_Pin|QSPI_BK1_IO1_Pin|QSPI_BK1_IO0_Pin);

    HAL_GPIO_DeInit(GPIOA, QSPI_BK1_IO2_Pin|QSPI_CLK_Pin|QSPI_BK1_IO3_Pin);

  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }

}

void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hsd->Instance==SDMMC1)
  {
  /* USER CODE BEGIN SDMMC1_MspInit 0 */

  /* USER CODE END SDMMC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SDMMC1_CLK_ENABLE();
  
    /**SDMMC1 GPIO Configuration    
    PD2     ------> SDMMC1_CMD
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PC9     ------> SDMMC1_D1
    PC8     ------> SDMMC1_D0 
    */
    GPIO_InitStruct.Pin = uSD_CMD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
    HAL_GPIO_Init(uSD_CMD_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = uSD_D2_Pin|uSD_D3_Pin|uSD_CLK_Pin|uSD_D1_Pin 
                          |uSD_D0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN SDMMC1_MspInit 1 */

  /* USER CODE END SDMMC1_MspInit 1 */
  }

}

void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{

  if(hsd->Instance==SDMMC1)
  {
  /* USER CODE BEGIN SDMMC1_MspDeInit 0 */

  /* USER CODE END SDMMC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDMMC1_CLK_DISABLE();
  
    /**SDMMC1 GPIO Configuration    
    PD2     ------> SDMMC1_CMD
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PC9     ------> SDMMC1_D1
    PC8     ------> SDMMC1_D0 
    */
    HAL_GPIO_DeInit(uSD_CMD_GPIO_Port, uSD_CMD_Pin);

    HAL_GPIO_DeInit(GPIOC, uSD_D2_Pin|uSD_D3_Pin|uSD_CLK_Pin|uSD_D1_Pin 
                          |uSD_D0_Pin);

  /* USER CODE BEGIN SDMMC1_MspDeInit 1 */

  /* USER CODE END SDMMC1_MspDeInit 1 */
  }

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    /**SPI1 GPIO Configuration    
    PB4 (NJTRST)     ------> SPI1_MISO
    PA15 (JTDI)     ------> SPI1_NSS
    PB5     ------> SPI1_MOSI
    PA5     ------> SPI1_SCK 
    */
    GPIO_InitStruct.Pin = ARD_D12_Pin|ARD_D11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ARD_D10_Pin|ARD_D13_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
  else if(hspi->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspInit 0 */

  /* USER CODE END SPI2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI2_CLK_ENABLE();
  
    /**SPI2 GPIO Configuration    
    PI2     ------> SPI2_MISO
    PI1     ------> SPI2_SCK
    PB15     ------> SPI2_MOSI 
    */
    GPIO_InitStruct.Pin = SPI2_MISO_Pin|SPI2_CLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI2_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(SPI2_MOSI_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI2_MspInit 1 */

  /* USER CODE END SPI2_MspInit 1 */
  }

}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PB4 (NJTRST)     ------> SPI1_MISO
    PA15 (JTDI)     ------> SPI1_NSS
    PB5     ------> SPI1_MOSI
    PA5     ------> SPI1_SCK 
    */
    HAL_GPIO_DeInit(GPIOB, ARD_D12_Pin|ARD_D11_Pin);

    HAL_GPIO_DeInit(GPIOA, ARD_D10_Pin|ARD_D13_Pin);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
  else if(hspi->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspDeInit 0 */

  /* USER CODE END SPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();
  
    /**SPI2 GPIO Configuration    
    PI2     ------> SPI2_MISO
    PI1     ------> SPI2_SCK
    PB15     ------> SPI2_MOSI 
    */
    HAL_GPIO_DeInit(GPIOI, SPI2_MISO_Pin|SPI2_CLK_Pin);

    HAL_GPIO_DeInit(SPI2_MOSI_GPIO_Port, SPI2_MOSI_Pin);

  /* USER CODE BEGIN SPI2_MspDeInit 1 */

  /* USER CODE END SPI2_MspDeInit 1 */
  }

}

void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hpcd->Instance==USB_OTG_FS)
  {
  /* USER CODE BEGIN USB_OTG_FS_MspInit 0 */

  /* USER CODE END USB_OTG_FS_MspInit 0 */
  
    /**USB_OTG_FS GPIO Configuration    
    PA12     ------> USB_OTG_FS_DP
    PA11     ------> USB_OTG_FS_DM
    PA10     ------> USB_OTG_FS_ID 
    */
    GPIO_InitStruct.Pin = USB_OTGFS_DP_Pin|USB_OTGFS_DM_Pin|USB_OTGFS_ID_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    /* Enable VDDUSB */
    if(__HAL_RCC_PWR_IS_CLK_DISABLED())
    {
      __HAL_RCC_PWR_CLK_ENABLE();
      HAL_PWREx_EnableVddUSB();
      __HAL_RCC_PWR_CLK_DISABLE();
    }
    else
    {
      HAL_PWREx_EnableVddUSB();
    }
    /* USB_OTG_FS interrupt Init */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
  /* USER CODE BEGIN USB_OTG_FS_MspInit 1 */

  /* USER CODE END USB_OTG_FS_MspInit 1 */
  }

}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef* hpcd)
{

  if(hpcd->Instance==USB_OTG_FS)
  {
  /* USER CODE BEGIN USB_OTG_FS_MspDeInit 0 */

  /* USER CODE END USB_OTG_FS_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
  
    /**USB_OTG_FS GPIO Configuration    
    PA12     ------> USB_OTG_FS_DP
    PA11     ------> USB_OTG_FS_DM
    PA10     ------> USB_OTG_FS_ID 
    */
    HAL_GPIO_DeInit(GPIOA, USB_OTGFS_DP_Pin|USB_OTGFS_DM_Pin|USB_OTGFS_ID_Pin);

    /* Disable VDDUSB */
    if(__HAL_RCC_PWR_IS_CLK_DISABLED())
    {
      __HAL_RCC_PWR_CLK_ENABLE();
      HAL_PWREx_DisableVddUSB();
      __HAL_RCC_PWR_CLK_DISABLE();
    }
    else
    {
      HAL_PWREx_DisableVddUSB();
    }

    /* USB_OTG_FS interrupt DeInit */
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
  /* USER CODE BEGIN USB_OTG_FS_MspDeInit 1 */

  /* USER CODE END USB_OTG_FS_MspDeInit 1 */
  }

}

static uint32_t FMC_Initialized = 0;

static void HAL_FMC_MspInit(void){
  /* USER CODE BEGIN FMC_MspInit 0 */

  /* USER CODE END FMC_MspInit 0 */
  GPIO_InitTypeDef GPIO_InitStruct;
  if (FMC_Initialized) {
    return;
  }
  FMC_Initialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_ENABLE();
  
  /** FMC GPIO Configuration  
  PE0   ------> FMC_NBL0
  PE1   ------> FMC_NBL1
  PG9   ------> FMC_NE2
  PD0   ------> FMC_D2
  PD4   ------> FMC_NOE
  PD1   ------> FMC_D3
  PD5   ------> FMC_NWE
  PF2   ------> FMC_A2
  PF1   ------> FMC_A1
  PF0   ------> FMC_A0
  PD7   ------> FMC_NE1
  PF3   ------> FMC_A3
  PF4   ------> FMC_A4
  PF5   ------> FMC_A5
  PG1   ------> FMC_A11
  PE10   ------> FMC_D7
  PD15   ------> FMC_D1
  PG0   ------> FMC_A10
  PE9   ------> FMC_D6
  PE15   ------> FMC_D12
  PG5   ------> FMC_A15
  PG4   ------> FMC_A14
  PG3   ------> FMC_A13
  PG2   ------> FMC_A12
  PD10   ------> FMC_D15
  PF15   ------> FMC_A9
  PE8   ------> FMC_D5
  PE14   ------> FMC_D11
  PD14   ------> FMC_D0
  PD12   ------> FMC_A17
  PD11   ------> FMC_A16
  PD13   ------> FMC_A18
  PF14   ------> FMC_A8
  PE7   ------> FMC_D4
  PE13   ------> FMC_D10
  PD9   ------> FMC_D14
  PD8   ------> FMC_D13
  PF13   ------> FMC_A7
  PE12   ------> FMC_D9
  PF12   ------> FMC_A6
  PE11   ------> FMC_D8
  */
  GPIO_InitStruct.Pin = PSRAM_NBL0_Pin|PSRAM_NBL1_Pin|D7_Pin|D6_Pin 
                          |D12_Pin|D5_Pin|D11_Pin|D4_Pin 
                          |D10_Pin|D9_Pin|D8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PSRAM_NE_Pin|PSRAM_A11_Pin|PSRAM_A10_Pin|PSRAM_A15_Pin 
                          |PSRAM_A14_Pin|PSRAM_A13_Pin|PSRAM_A12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = D2_Pin|OE_Pin|D3_Pin|WE_Pin 
                          |LCD_NE_Pin|D1_Pin|D15_Pin|D0_Pin 
                          |PSRAM_A17_Pin|PSRAM_A16_Pin|PSRAM_A18_LCD_RS_Pin|D14_Pin 
                          |D13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PSRAM_A2_Pin|PSRAM_A1_Pin|PSRAM_A0_Pin|PSRAM_A3_Pin 
                          |PSRAM_A4_Pin|PSRAM_A5_Pin|PSRAM_A9_Pin|PSRAM_A8_Pin 
                          |PSRAM_A7_Pin|PSRAM_A6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* USER CODE BEGIN FMC_MspInit 1 */

  /* USER CODE END FMC_MspInit 1 */
}

void HAL_SRAM_MspInit(SRAM_HandleTypeDef* hsram){
  /* USER CODE BEGIN SRAM_MspInit 0 */

  /* USER CODE END SRAM_MspInit 0 */
  HAL_FMC_MspInit();
  /* USER CODE BEGIN SRAM_MspInit 1 */

  /* USER CODE END SRAM_MspInit 1 */
}

static uint32_t FMC_DeInitialized = 0;

static void HAL_FMC_MspDeInit(void){
  /* USER CODE BEGIN FMC_MspDeInit 0 */

  /* USER CODE END FMC_MspDeInit 0 */
  if (FMC_DeInitialized) {
    return;
  }
  FMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_DISABLE();
  
  /** FMC GPIO Configuration  
  PE0   ------> FMC_NBL0
  PE1   ------> FMC_NBL1
  PG9   ------> FMC_NE2
  PD0   ------> FMC_D2
  PD4   ------> FMC_NOE
  PD1   ------> FMC_D3
  PD5   ------> FMC_NWE
  PF2   ------> FMC_A2
  PF1   ------> FMC_A1
  PF0   ------> FMC_A0
  PD7   ------> FMC_NE1
  PF3   ------> FMC_A3
  PF4   ------> FMC_A4
  PF5   ------> FMC_A5
  PG1   ------> FMC_A11
  PE10   ------> FMC_D7
  PD15   ------> FMC_D1
  PG0   ------> FMC_A10
  PE9   ------> FMC_D6
  PE15   ------> FMC_D12
  PG5   ------> FMC_A15
  PG4   ------> FMC_A14
  PG3   ------> FMC_A13
  PG2   ------> FMC_A12
  PD10   ------> FMC_D15
  PF15   ------> FMC_A9
  PE8   ------> FMC_D5
  PE14   ------> FMC_D11
  PD14   ------> FMC_D0
  PD12   ------> FMC_A17
  PD11   ------> FMC_A16
  PD13   ------> FMC_A18
  PF14   ------> FMC_A8
  PE7   ------> FMC_D4
  PE13   ------> FMC_D10
  PD9   ------> FMC_D14
  PD8   ------> FMC_D13
  PF13   ------> FMC_A7
  PE12   ------> FMC_D9
  PF12   ------> FMC_A6
  PE11   ------> FMC_D8
  */
  HAL_GPIO_DeInit(GPIOE, PSRAM_NBL0_Pin|PSRAM_NBL1_Pin|D7_Pin|D6_Pin 
                          |D12_Pin|D5_Pin|D11_Pin|D4_Pin 
                          |D10_Pin|D9_Pin|D8_Pin);

  HAL_GPIO_DeInit(GPIOG, PSRAM_NE_Pin|PSRAM_A11_Pin|PSRAM_A10_Pin|PSRAM_A15_Pin 
                          |PSRAM_A14_Pin|PSRAM_A13_Pin|PSRAM_A12_Pin);

  HAL_GPIO_DeInit(GPIOD, D2_Pin|OE_Pin|D3_Pin|WE_Pin 
                          |LCD_NE_Pin|D1_Pin|D15_Pin|D0_Pin 
                          |PSRAM_A17_Pin|PSRAM_A16_Pin|PSRAM_A18_LCD_RS_Pin|D14_Pin 
                          |D13_Pin);

  HAL_GPIO_DeInit(GPIOF, PSRAM_A2_Pin|PSRAM_A1_Pin|PSRAM_A0_Pin|PSRAM_A3_Pin 
                          |PSRAM_A4_Pin|PSRAM_A5_Pin|PSRAM_A9_Pin|PSRAM_A8_Pin 
                          |PSRAM_A7_Pin|PSRAM_A6_Pin);

  /* USER CODE BEGIN FMC_MspDeInit 1 */

  /* USER CODE END FMC_MspDeInit 1 */
}

void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef* hsram){
  /* USER CODE BEGIN SRAM_MspDeInit 0 */

  /* USER CODE END SRAM_MspDeInit 0 */
  HAL_FMC_MspDeInit();
  /* USER CODE BEGIN SRAM_MspDeInit 1 */

  /* USER CODE END SRAM_MspDeInit 1 */
}

static uint32_t SAI1_client =0;

void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai)
{

  GPIO_InitTypeDef GPIO_InitStruct;
/* SAI1 */
    if(hsai->Instance==SAI1_Block_A)
    {
    /* Peripheral clock enable */
    if (SAI1_client == 0)
    {
       __HAL_RCC_SAI1_CLK_ENABLE();
    }
    SAI1_client ++;
    
    /**SAI1_A_Block_A GPIO Configuration    
    PE4     ------> SAI1_FS_A
    PE2     ------> SAI1_MCLK_A
    PE6     ------> SAI1_SD_A
    PB10     ------> SAI1_SCK_A 
    */
    GPIO_InitStruct.Pin = SAI1_FSA_Pin|SAI1_MCKA_Pin|SAI1_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    }
    if(hsai->Instance==SAI1_Block_B)
    {
      /* Peripheral clock enable */
      if (SAI1_client == 0)
      {
       __HAL_RCC_SAI1_CLK_ENABLE();
      }
    SAI1_client ++;
    
    /**SAI1_B_Block_B GPIO Configuration    
    PE3     ------> SAI1_SD_B 
    */
    GPIO_InitStruct.Pin = SAI1_SDB_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
    HAL_GPIO_Init(SAI1_SDB_GPIO_Port, &GPIO_InitStruct);

    }
}

void HAL_SAI_MspDeInit(SAI_HandleTypeDef* hsai)
{

/* SAI1 */
    if(hsai->Instance==SAI1_Block_A)
    {
    SAI1_client --;
    if (SAI1_client == 0)
      {
      /* Peripheral clock disable */ 
       __HAL_RCC_SAI1_CLK_DISABLE();
      }
    
    /**SAI1_A_Block_A GPIO Configuration    
    PE4     ------> SAI1_FS_A
    PE2     ------> SAI1_MCLK_A
    PE6     ------> SAI1_SD_A
    PB10     ------> SAI1_SCK_A 
    */
    HAL_GPIO_DeInit(GPIOE, SAI1_FSA_Pin|SAI1_MCKA_Pin|SAI1_SDA_Pin);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);

    }
    if(hsai->Instance==SAI1_Block_B)
    {
    SAI1_client --;
      if (SAI1_client == 0)
      {
      /* Peripheral clock disable */
      __HAL_RCC_SAI1_CLK_DISABLE();
      }
    
    /**SAI1_B_Block_B GPIO Configuration    
    PE3     ------> SAI1_SD_B 
    */
    HAL_GPIO_DeInit(SAI1_SDB_GPIO_Port, SAI1_SDB_Pin);

    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
