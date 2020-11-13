/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			bt_device_type.h
* Description:		bt_device_type function h file.
* maintainer: Halley
******************************************************************************/
#ifndef __BT_DEVICE_TYPE_H__
#define __BT_DEVICE_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
enum BtDeviceType
{
    BtNoneDevice = 0,
    BTUsbDevice,
    BTUartDeviceRda5875,
    BTUartDeviceCsrBC6,
    BTUartDeviceBcm20702,
    BTUartDeviceMTK662X,
    BTUartDeviceRTL8761,
};

typedef enum
{
    GPIO_PORT_A = 1, /**< Use GPIOA for LDOEN/RST */
    GPIO_PORT_B,	 /**< Use GPIOB for LDOEN/RST */
    GPIO_PORT_C	     /**< Use GPIOC for LDOEN/RST */
} GPIO_PORT_SEL;

//#define  BUART_RX_FIFO_SIZE   (1024*6)
//#define  BUART_TX_FIFO_SIZE   (1024)


//
//��������оƬ��GPIO���ù�ϵ
//
void BTDevicePinCFG(void);

//
//��ʼ�������豸��BtDeviceType Ϊ����оƬ����ö��ֵ
//
bool BTDeviceInit(uint8_t BTDeviceType, int8_t* BdAdd/*6Bytes, LE*/);

//
//�ر�����Ӳ��ģ��
//
void BTDevicePowerOff(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
