///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: radio_driver_simulator.h
//  maintainer: Sam
///////////////////////////////////////////////////////////////////////////////

#ifndef __RADIO_DRV_H__
#define __RADIO_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"
#include "radio_control.h"


// �����������ֵ
typedef enum _RADIO_SEEK_RESULT
{
    RADIO_SEEK_NONE = 0,
    RADIO_SEEK_FALSE,
    RADIO_SEEK_STC,
    RADIO_SEEK_VALIDSTATION,
    RADIO_SEEK_BANDLIMIT
} RADIO_SEEK_RESULT;


//// �����ӿڣ�����FM�豸�ͺŲ��죬��ͬ���豸���������ڲ�����////
// ��ʼ��FM�豸
bool RadioInit(void);
bool RadioValidCheck(void);
// ��FM�豸
bool RadioPowerOn(void);
// �ر�FM�豸
bool RadioPowerOff(void);
// ���FM�豸״̬(TRUE - ����״̬��FALSE - ����״̬)
bool IsRadioWorking(void);
// ���õ�ǰƵ��
bool RadioSetFreq(uint16_t Freq);
// ָ��Ƶ������
void RadioSearchSet(uint16_t Freq);
// �����л�
bool RadioMute(bool MuteFlag);
// �ж��ź�ǿ�ȣ�������̨�����ѯ���������TRUE�������ѵ��µĵ�̨������δ�ѵ�
bool RadioRSSIRead(uint16_t Freq);
// FMͨ���������
void RadioLinkOutputHIZ(bool HizFg);
// �Ƿ�������
bool RadioIsStereo(void);
//	����������
void RadioSetStereo(bool StereoFlag);
////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*__RADIO_DRV_SIMULATOR_H_*/





