///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: radio_control.h
//  maintainer: Sam
///////////////////////////////////////////////////////////////////////////////
#ifndef __RADIO_CONTROL_H__
#define __RADIO_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"
#include "../fm_driver/radio.h"

#ifdef FUNC_RADIO_EN

#define FM_AREA_CHINA  0x00 // 00B��87.5~108MHz (US/Europe, China)     
#define FM_AREA_JAPAN  0x01 // 01B��76~90MHz (Japan)
#define FM_AREA_RUSSIA 0x02 // 10B��65.8~73MHz (Russia)                
#define FM_AREA_OTHER  0x03 // 11B��60~76MHz

#define CHINA_BASE_FREQ  875 // China/US/EuropeƵ�� ��׼Ƶ��
#define JAPAN_BASE_FREQ  760 // JapanƵ�� ��׼Ƶ��
#define RUSSIA_BASE_FREQ 650 // RussiaƵ�� ��׼Ƶ��
#define OTHER_BASE_FREQ  600 // ����Ƶ�� ��׼Ƶ��
	
	
#define FM_CHANNLE_STEP  100 // 100KHZ��FM��̨����

#pragma pack(1)
// ֧������FM�豸
typedef enum _Radio_NAME
{
    RADIO_NONE,
    RADIO_RDA5807P,
    RADIO_SILI4702,
    RADIO_CS2010,
    RADIO_CL6017X,
    RADIO_KT0830E,
    RADIO_AR1000F,
    RADIO_BK1080,
    RADIO_QN8035,
    RADIO_RDA5807H
} RADIO_NAME;

typedef enum _RADIO_STATUS_
{
    RADIO_STATUS_IDLE = 0,  // ������
    RADIO_STATUS_SEEKING,   // ��̨��
    RADIO_STATUS_PLAYING,   // ������
    RADIO_STATUS_PREVIEW,   // ��̨Ԥ��

} RADIO_STATUS;
#pragma pack()


#define RADIO_DEF_MIN_FREQ_VALUE 875  // FMƵ����С��Χ(US/Europe, China)
#define RADIO_DEF_MAX_FREQ_VALUE 1080 // FMƵ�����Χ(US/Europe, China)

#define RADIO_JAP_MIN_FREQ_VALUE 760  // FMƵ����С��Χ(Japan)
#define RADIO_JAP_MAX_FREQ_VALUE 900  // FMƵ�����Χ(Japan)

#define RADIO_RUS_MIN_FREQ_VALUE 658  // FMƵ����С��Χ(Russia)
#define RADIO_RUS_MAX_FREQ_VALUE 730  // FMƵ�����Χ(Russia)

#define RADIO_OTH_MIN_FREQ_VALUE 600  // FMƵ����С��Χ(����)
#define RADIO_OTH_MAX_FREQ_VALUE 760  // FMƵ�����Χ(����)

#pragma pack(1)
typedef struct _RADIO_CONTROL_
{
	uint8_t   Channel[MAX_RADIO_CHANNEL_NUM]; // ��̨�б� ??? �Ƿ���Ը�Ϊָ�룬ֱ��ָ�� NVM�е����ݣ���ʡ�ڴ�
	uint16_t  Freq;                           // ��ǰƵ��
	uint16_t  MinFreq;
	uint16_t  MaxFreq;
	uint8_t  ChlCount;                       // ���е�̨����
	uint8_t  CurStaIdx;                      // ��ǰ��̨������
	uint8_t  State;                          // FM Radio��ǰ״̬
	uint8_t  CurFreqArea;                    // ��ǰƵ��(�й����ձ���ŷ��...)

	SW_TIMER TimerHandle;
#ifdef RADIO_DELAY_SWITCH_CHANNEL
	SW_TIMER DelayDoTimer; // ��̨�л��ӳ���תTimer
#endif

	uint16_t InputNumber; // �������ְ���ѡ̨
	SW_TIMER NumKeyTimer;

} RADIO_CONTROL;
#pragma pack()

extern RADIO_CONTROL* sRadioControl;
extern RADIO_NAME Radio_Name;	//����FM���ͺ�

bool RadioDeinitialize(void);
bool RadioInitialize(void);
bool RadioControl(void);
void RadioPlay(void);
bool RadioSwitchChannel(uint32_t Msg);

#endif/*FUNC_RADIO_EN*/

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
