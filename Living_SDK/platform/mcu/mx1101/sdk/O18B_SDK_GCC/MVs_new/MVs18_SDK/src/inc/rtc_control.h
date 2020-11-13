///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: rtc_control.h
// maintainer: Sam
///////////////////////////////////////////////////////////////////////////////

#ifndef __RTC_CONTROL_H__
#define __RTC_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"
#include "rtc.h"
#include "breakpoint.h"
//// ��������
//#define INTER_RING_TYPE 0 // ��������
//#define USB_RING_TYPE   1 // U������
//#define SD_RING_TYPE    2 // SD������

//// �������ѷ�ʽ
//#define PLAYER_REMIND 0// ������ʱ��ֱ�ӽ��벥��ģʽ������������
//#define FM_REMIND     1// ������ʱ��ֱ�ӽ���FMģʽ
//#define POPUP_REMIND  2// ������ʱ�������ض����������Ѵ���

////////////////////////////////////////////////////

typedef enum _RTC_STATE_
{
	RTC_STATE_IDLE = 0,
	RTC_STATE_SET_TIME,
	RTC_STATE_SET_ALARM,
	
} RTC_STATE;

typedef enum _RTC_SUB_STATE_
{
    RTC_SET_IDLE = 0,	// ������
	RTC_SET_YEAR,    	// �������
    RTC_SET_MON,     	// �����·�
    RTC_SET_DAY,     	// ��������
	RTC_SET_WEEK,		// ������
    RTC_SET_HOUR,    	// ����Сʱ
    RTC_SET_MIN,     	// ���÷���
    RTC_SET_SEC,     	// ��������
	RTC_SET_ALARM_NUM,
	RTC_SET_ALARM_MODE,
//    RTC_SET_MAX, 		//
	
} RTC_SUB_STATE;


typedef enum _RTC_WEEK_
{
    SUNDAY,  
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,

} RTC_WEEK;

typedef struct _RTC_CONTROL_
{
	uint8_t  		State;          // RTC��ǰ״̬,IDLE, Set Time, Set Alarm
	uint8_t  		SubState;       // RTC��ǰ��״̬,set year/mon/...
	
	RTC_DATE_TIME 	DataTime;  		// ʱ������	
	
	uint8_t	 		AlarmNum;		// ���ӱ��
	uint8_t	 		AlarmMode;		// ����ģʽ
	RTC_DATE_TIME 	AlarmTime; 		// ����ʱ��
	uint8_t  		AlarmData;		// �Զ�������ģʽ�£�����ʱ��ѡ��bit0�������죬bit1������һ��...,bit6��������,��λ��ʾѡ��  
	uint8_t  		CurAlarmNum;	// ��ǰ��Ӧ�����ӱ��
	
	RTC_LUNAR_DATE	LuarDate; 		// ũ��

	SW_TIMER TimerHandle;
} RTC_CONTROL;


///////////////////////////////////////////////////////////
//���������أ��ò��ִ������������NVM�����ᱣ����Flash
//��־SDKδȥʵ���������崦���ò��ִ����ɿͻ��Լ�������
typedef struct _NVM_ALARM_INFO_
{	
	uint8_t RingType : 3; // �������� INTER_RING_TYPE - ����������USB_RING_TYPE - U��������SD_RING_TYPE - SD������
	uint8_t Duration : 2; // �������ʱ�� 0 - 30s��1 - 1���ӡ�2 - 2���ӡ�3 - 3����
	uint8_t RepeatCnt: 3; // �ظ�����
	uint32_t FileAddr;    // �ļ�������
	uint8_t CRC8;         // �ļ���У����
	
} ALARM_RING_INFO;

// Ĭ�� �Զ�����3�Ρ�������5����
typedef struct _NVM_RTC_INFO_
{
	uint8_t AlarmVolume;        // ������������
	ALARM_RING_INFO AlarmRingList[MAX_ALARM_NUM]; // ����������Ϣ���ⲿ���ɿͻ��������
	ALARM_TIME_INFO AlarmTimeList[MAX_ALARM_NUM];// ����ʱ����Ϣ
	
} NVM_RTC_INFO;


#ifdef FUNC_BREAKPOINT_EN
#define RTC_NVM_START_ADDR		(sizeof(BP_INFO))
#else 
#define RTC_NVM_START_ADDR		0
#endif
///////////////////////////////////////////

bool RtcControl(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif



