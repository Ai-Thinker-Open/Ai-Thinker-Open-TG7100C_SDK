/**
 *****************************************************************************
 * @file     ir.h
 * @author   Yancy
 * @version  V1.0.0
 * @date     19-June-2013
 * @maintainer Fenston
 * @brief    ir module driver head file
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __IR_H__
#define __IR_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * IR IOѡ��
 */
#define IR_USE_GPIOA10 0   /**< ʹ��GPIO A10��ΪIr���� */
#define IR_USE_GPIOB7  1   /**< ʹ��GPIO B7 ��ΪIr���� */
#define IR_USE_GPIOC2  2   /**< ʹ��GPIO C2 ��ΪIr���� */

/**
 * @brief  ѡ��IR���ù�ϵ
 * @param  IrSel��ָ��IO��
 * @arg    IR_USE_GPIOA10
 * @arg    IR_USE_GPIOB7
 * @arg    IR_USE_GPIOC2
 * @return NONE
 */
void IrGpioSel(uint8_t IrSel);

/**
 * @brief  ��ʼ��IRģ��
 * @param  NONE
 * @return NONE
 */
void IrInit(void);

/**
 * @brief  �ж��Ƿ���յ��˰���
 * @param  NONE
 * @return TRUE�����յ��� FALSE:û�н��յ�
 */
bool IrIsKeyCome(void);

/**
 * @brief  ��ȡ������ֵ
 * @param  NONE
 * @return ���յ��İ�����ֵ
 */
uint32_t IrGetKeyCode(void);

/**
 * @brief  �ж��Ƿ��ڳ���״̬
 * @param  NONE
 * @return TRUE���ڳ���״̬  FALSE��û���ڳ���״̬
 */
bool IrIsContinuePrs(void);

/**
 * @brief  ���ü��ģʽ
 * @param  IsIgnore
 * @arg    TRUE: ����Ϊ��ͷ���ģʽ
 * @arg    FALSE: ����Ϊһ���ģʽ
 * @return NONE
 */
void IrIgnoreLeadHeader(bool IsIgnore);



#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__IR_KEY_H__
