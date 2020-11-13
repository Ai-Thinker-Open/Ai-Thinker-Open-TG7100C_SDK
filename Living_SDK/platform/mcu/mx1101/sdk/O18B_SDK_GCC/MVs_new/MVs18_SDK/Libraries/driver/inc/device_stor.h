/**
 *****************************************************************************
 * @file     device_stor.h
 * @author   Orson
 * @version  V1.0.0
 * @date     24-June-2013
 * @brief    device mass-storage module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __DEVICE_STOR_H__
#define	__DEVICE_STOR_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//���������ַ���ָ������ָ�������豸����������ʾ�Ĵ�������������
extern uint8_t* gDeviceString_MassStor_Vendor;			//max length: 8bytes
extern uint8_t* gDeviceString_MassStor_Product;		//max length: 16bytes
extern uint8_t* gDeviceString_MassStor_Ver;			//max length: 4bytes


/**
 * @brief  ���������ܳ�ʼ��
 * @param  NONE
 * @return NONE
 */
void DeviceStorInit(void);

/**
 * @brief  ������ģʽ����������
 * @param  NONE
 * @return NONE
 */
void DeviceStorProcess(void);

/**
 * @brief  �жϵ�ǰ�Ƿ��ڱ���״̬
 * @param  NONE
 * @return 1-������0-�Ǳ���
 */
bool DeviceStorIsPrevent(void);

/**
 * @brief  �ж�PC���Ƿ�ִ���ˡ�����������
 * @param  NONE
 * @return 1-�ѵ�����0-δ����
 */
bool DeviceStorIsStopped(void);

uint8_t GetSdReaderState(void);

void SetSdReaderState(uint8_t State);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
