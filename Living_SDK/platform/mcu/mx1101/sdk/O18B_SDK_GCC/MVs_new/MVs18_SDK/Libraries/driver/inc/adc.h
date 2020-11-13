/**
*************************************************************************************
* @file	sradc.h
* @author	Yancy
* @version	v1.0.0
* @date    2013/05/29
* @brief	Sar ADC driver header file
* Copyright (C) Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
*************************************************************************************
*/
/**************************************************************************************
 *@maintainer Fenston
 *@date 2014/05/08
 **************************************************************************************
 **/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * ADC ͨ��ѡ��
 * Note: ��Ҫ�������º궨��!
 */
#define     ADC_CHANNEL_LDOIN          0	 /**< ADC channel: LDOIN*/
#define     ADC_CHANNEL_B5             2	 /**< ADC channel: GPIO B5*/
#define     ADC_CHANNEL_B6             3	 /**< ADC channel: GPIO B6 */
#define     ADC_CHANNEL_B7             4	 /**< ADC channel: GPIO B7 */
#define     ADC_CHANNEL_B22            5	 /**< ADC channel: GPIO B22*/
#define     ADC_CHANNEL_B23            6	 /**< ADC channel: GPIO B23*/
#define     ADC_CHANNEL_B24            7	 /**< ADC channel: GPIO B24*/
#define     ADC_CHANNEL_B25            8	 /**< ADC channel: GPIO B25*/
#define     ADC_CHANNEL_C7             9	 /**< ADC channel: GPIO C7*/
#define     ADC_CHANNEL_C8            10	 /**< ADC channel: GPIO C8 */
#define     ADC_CHANNEL_C9            11	 /**< ADC channel: GPIO C9*/

/**
  * @brief  ����gpioΪsar adcͨ��
  * @param  AdcChannel: ��Ҫ���õ�adc channel
  *     @arg ADC_CHANNEL_B5
  *     @arg ADC_CHANNEL_B6
  *     @arg ADC_CHANNEL_B7
  *     @arg ADC_CHANNEL_B22
  *     @arg ADC_CHANNEL_B23
  *     @arg ADC_CHANNEL_B24
  *     @arg ADC_CHANNEL_B25
  *     @arg ADC_CHANNEL_C7
  *     @arg ADC_CHANNEL_C8
  *     @arg ADC_CHANNEL_C9
  * @return void
  */
void SarAdcGpioSel(uint8_t AdcChannel);

/**
  * @brief  ��ȡADCͨ���Ĳ���ֵ���ο���ѹΪLDO3V3
  * @param  Channel: ADC channel ID
  *     @arg ADC_CHANNEL_LDOIN
  *     @arg ADC_CHANNEL_B5
  *     @arg ADC_CHANNEL_B6
  *     @arg ADC_CHANNEL_B7
  *     @arg ADC_CHANNEL_B22
  *     @arg ADC_CHANNEL_B23
  *     @arg ADC_CHANNEL_B24
  *     @arg ADC_CHANNEL_B25
  *     @arg ADC_CHANNEL_C7
  *     @arg ADC_CHANNEL_C8
  *     @arg ADC_CHANNEL_C9
  * @return ��ȡ�Ĳ���ֵ
  */
uint16_t SarAdcChannelGetValue(uint8_t Channel);


/**
  * @brief  5V�ȶ���Դ����ϵͳʹ��SarAdc��LDO3V3�ٴ�Trim��
  * @param  void
  * @return void
  */
void SarAdcTrimLdo3V3(void);


/**
  * @brief  ����SarAdcGetLdoinVoltageǰ�ĳ�ʼ����ֻ�����һ�Ρ�
  * @param  void
  * @return void
  */
void SarAdcLdoinVolInit(void);

/**
  * @brief  ��ȡLDOIN�ĵ�ѹֵ
  * @param  NONE
  * @return �Ժ���Ϊ��λ�ĵ�ѹֵ
  * @note   �ڵ���SarAdcGetLdoinVoltage����֮ǰ�������SarAdcLdoinVolInit���г�ʼ��
  */
uint16_t SarAdcGetLdoinVoltage(void);

/**
  * @brief  ��ȡGPIO�ĵ�ѹֵ
  * @param  Channel: ADC channel ID
  * @return �Ժ���Ϊ��λ�ĵ�ѹֵ
  */
uint16_t SarAdcGetGpioVoltage(uint8_t Channel);

/**
  * @brief  ��ȡ﮵��ʣ�����
  * @param  None
  * @return ʣ������İٷֱȣ�����ֵΪ0~100
  */
uint8_t SarAdcGetRemainBatteryCapacity(void);


/**
  * @brief  PowerDown ADC for low power
  * @param  void
  * @return void
  * @note   �ڽ�������ʱͨ��powerdown ADC���͹���
  */
void SarAdcPowerDown(void);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

