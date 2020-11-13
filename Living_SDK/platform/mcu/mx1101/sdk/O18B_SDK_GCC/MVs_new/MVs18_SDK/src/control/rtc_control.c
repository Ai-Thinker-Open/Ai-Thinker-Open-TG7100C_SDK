///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: rtc_control.c
// maintainer: Sam
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "os.h"
#include "rtc_control.h"
#include "rtc.h"
#include "sys_app.h"
#include "recorder_control.h"
#include "dev_detect_driver.h"
#include "sound_remind.h"
#include "nvm.h"
#include "sys_vol.h"

#ifdef FUNC_RTC_EN
RTC_CONTROL* sRtcControl = NULL;
NVM_RTC_INFO sNvmRtcInfo; 
static TIMER RtcAutoOutTimer;

#define RTC_AUTO_OUT_TIME	5000

////////////////////////////////////
#ifdef FUNC_RTC_LUNAR
static void DisplayLunarDate(void)
{
	//ũ���������
	const uint8_t LunarYearName[12][2] = {"��", "ţ", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��"};
	
	//ũ���·�����
	const uint8_t LunarMonthName[12][2] = {"��", "��", "��", "��", "��", "��", "��", "��", "��", "ʮ", "��", "��"};
	
	//ũ����������
	const uint8_t LunarDateName[30][4] = {"��һ", "����", "����", "����", "����", "����", "����", "����", "����", "��ʮ", 
									  "ʮһ", "ʮ��", "ʮ��", "ʮ��", "ʮ��", "ʮ��", "ʮ��", "ʮ��", "ʮ��", "��ʮ",
									  "إһ", "إ��", "إ��", "إ��", "إ��", "إ��", "إ��", "إ��", "إ��", "��ʮ"};
	
	//ũ���������
	const uint8_t HeavenlyStemName[10][2] = {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"};
	
	//ũ����֧����
	const uint8_t EarthlyBranchName[12][2] = {"��", "��", "��", "î", "��", "��", "��", "δ", "��", "��", "��", "��"};
	
	RTC_LUNAR_DATE LunarDate;

	SolarToLunar(&sRtcControl->DataTime, &LunarDate);	
	APP_DBG("ũ�� %d�� ", (uint32_t)LunarDate.Year);
	APP_DBG("%-.2s%-.2s�� ", HeavenlyStemName[GetHeavenlyStem(LunarDate.Year)], 
						  EarthlyBranchName[GetEarthlyBranch(LunarDate.Year)]);
	APP_DBG("%-.2s�� ", LunarYearName[GetEarthlyBranch(LunarDate.Year)]);
	if(LunarDate.IsLeapMonth)
	{
		APP_DBG("��");
	}
	APP_DBG("%-.2s��", LunarMonthName[LunarDate.Month - 1]);
	
	if(LunarDate.MonthDays == 29)
	{
		APP_DBG("(С)");
	}
	else
	{
		APP_DBG("(��)");
	}
	
	APP_DBG("%-.4s ", LunarDateName[LunarDate.Date - 1]);

	if((LunarDate.Month == 1) && (LunarDate.Date == 1))			//����
	{
		APP_DBG("����");
	}
	else if((LunarDate.Month == 1) && (LunarDate.Date == 15))	//Ԫ����
	{
		APP_DBG("Ԫ����");
	}
	else if((LunarDate.Month == 5) && (LunarDate.Date == 5))	//�����
	{
		APP_DBG("�����");
	}
	else if((LunarDate.Month == 7) && (LunarDate.Date == 7))	//��Ϧ���˽�
	{
		APP_DBG("��Ϧ���˽�");
	}
	else if((LunarDate.Month == 7) && (LunarDate.Date == 15))	//��Ԫ��
	{
		APP_DBG("��Ԫ��");
	}
	else if((LunarDate.Month == 8) && (LunarDate.Date == 15))	//�����
	{
		APP_DBG("�����");
	}
	else if((LunarDate.Month == 9) && (LunarDate.Date == 9))	//������
	{
   		APP_DBG("������");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == 8))	//���˽�
	{
	 	APP_DBG("���˽�");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == 23))	//С��
	{
		APP_DBG("С��");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == LunarDate.MonthDays))	//��Ϧ
	{
		APP_DBG("��Ϧ");
	}
}
#endif


void RtcTimerCB(uint32_t unused)
{
	RtcGetCurrTime(&sRtcControl->DataTime);
	
	if(RTC_STATE_SET_TIME == sRtcControl->State)
	{
		APP_DBG("RTC set Time....");
	}
	
	APP_DBG("RtcTime(%04d-%02d-%02d %02d:%02d:%02d) Week:%d ",
	        sRtcControl->DataTime.Year, sRtcControl->DataTime.Mon, sRtcControl->DataTime.Date,
	        sRtcControl->DataTime.Hour, sRtcControl->DataTime.Min, sRtcControl->DataTime.Sec,
	        sRtcControl->DataTime.WDay);
#ifdef FUNC_RTC_LUNAR
	DisplayLunarDate();
#endif
	APP_DBG("\n");
	
#ifdef FUNC_RTC_ALARM	
	if(RTC_STATE_SET_ALARM == sRtcControl->State)
	{
		APP_DBG("���Ӻ�:%d ", sRtcControl->AlarmNum);
		switch(sRtcControl->AlarmMode)
		{
				case ALARM_MODE_ONCE_ONLY:
					APP_DBG("����ģʽ:����(once only) ");
					APP_DBG("%04d-%02d-%02d (����%02d) ", 
					sRtcControl->AlarmTime.Year, 
					sRtcControl->AlarmTime.Mon, 
					sRtcControl->AlarmTime.Date, 
					sRtcControl->AlarmTime.WDay);
					break;
	
				case ALARM_MODE_PER_DAY:
					APP_DBG("����ģʽ:ÿ��һ��(every day)");
					break;
	
				case ALARM_MODE_PER_WEEK:
					APP_DBG("����ģʽ:ÿ��һ��(every week) ����%02d", sRtcControl->AlarmTime.WDay);
					break;
				case ALARM_MODE_WORKDAY:
					APP_DBG("����ģʽ:������(weekday)");
					break;
				case ALARM_MODE_USER_DEFINED:
					APP_DBG("����ģʽ:�ͻ��Զ��� ÿ��%02x", sRtcControl->AlarmData);
					break;
	
				default:
					APP_DBG(("ģʽ����(mode error)\n"));
					break;
		}	
			
		APP_DBG("AlarmTime(%02d:%02d:%02d) \n",
	        sRtcControl->AlarmTime.Hour, sRtcControl->AlarmTime.Min, sRtcControl->AlarmTime.Sec);	
	}
#endif
}

// RTC��ʼ��
bool RtcInitialize(void)
{
	APP_DBG("RtcInitialize\n");
#ifdef FUNC_RTC_ALARM
	NvmRead(RTC_NVM_START_ADDR, (uint8_t*)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
	RtcInit((void *)(sNvmRtcInfo.AlarmTimeList), sizeof(ALARM_TIME_INFO) * MAX_ALARM_NUM);//Ӳ����ʼ������Ҫɾ��
#else
	RtcInit(NULL, 0); // �ú����������
#endif	
	if(sRtcControl != NULL)
	{
		APP_DBG("Init fail: RTC is running\n");
		return FALSE;
	}

	sRtcControl = APP_MMM_MALLOC(sizeof(RTC_CONTROL), MMM_ALLOC_NORMAL);
	ASSERT(sRtcControl != NULL);
	memset(sRtcControl, 0, sizeof(RTC_CONTROL));

	// ��ȡeprom��nvram�еģ�ʱ�����ڵ���Ϣ
	RtcGetCurrTime(&sRtcControl->DataTime);
	
#ifdef FUNC_RTC_ALARM
	NVIC_EnableIRQ(RTC_IRQn);           // �����������ж�,������sRtcControl�ڴ����֮��
#endif
	
	// ��ʱ��
	InitTimer((SW_TIMER*)&sRtcControl->TimerHandle, 1000, (TIMER_CALLBACK)RtcTimerCB);
	StartTimer(&sRtcControl->TimerHandle);
#ifdef FUNC_RTC_ALARM	
	NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
	return TRUE;
}

// ȥ��ʼ��
bool RtcDeinitialize(void)
{
	APP_DBG("RtcDeinitialize\n");
	if(sRtcControl == NULL)
	{
		return FALSE;
	}

	DeinitTimer(&sRtcControl->TimerHandle);

	// ȡ��ʱ����������
	if(sRtcControl->State != RTC_STATE_IDLE)
	{

	}
	// ���������ӿڣ�ֹͣ��ع���

	// �ͷ���Դ
	APP_MMM_FREE(sRtcControl);
	sRtcControl = NULL;

	return TRUE;
}


void RtcSubStateDbg(void)
{
	switch(sRtcControl->SubState)
	{
		case RTC_SET_YEAR:
			APP_DBG("���á��ꡱ\n");
			break;
		case RTC_SET_MON:
			APP_DBG("���á��¡�\n");
			break;
		case RTC_SET_DAY:
			APP_DBG("���á��ա�\n");
			break;
#ifdef FUNC_RTC_ALARM
		case RTC_SET_WEEK:
			APP_DBG("���á��ܡ�\n");
			break;
#endif
		case RTC_SET_HOUR:
			APP_DBG("���á�ʱ��\n");
			break;
		case RTC_SET_MIN:
			APP_DBG("���á��֡�\n");
			break;
		case RTC_SET_SEC:
			APP_DBG("��֧�����á��롱\n");
			break;
#ifdef FUNC_RTC_ALARM
		case RTC_SET_ALARM_NUM:
			APP_DBG("���á����ӱ�š�\n");
			break;
		case RTC_SET_ALARM_MODE:
			APP_DBG("���á�����ģʽ��\n");
			break;
#endif
		default:
			break;
	}
}

// �л�ʱ��������
bool RtcSwitchNextSubState(void)
{
	switch(sRtcControl->State)
	{
		case RTC_STATE_IDLE:
			APP_DBG("Failed to switch focus\n");
			return FALSE;
		
		case RTC_STATE_SET_TIME:
			APP_DBG("Switch to next value\n");
			sRtcControl->SubState++;
			if(sRtcControl->SubState > RTC_SET_SEC)
			{
				sRtcControl->SubState = RTC_SET_YEAR;
			}
			else if(sRtcControl->SubState == RTC_SET_WEEK)//ʱ�����ò�֧����ʱ������
			{
				sRtcControl->SubState++;
			}
			APP_DBG("sRtcControl->SubState:%d\n", sRtcControl->SubState);
			break;
			
#ifdef FUNC_RTC_ALARM	
		case RTC_STATE_SET_ALARM:
			APP_DBG("Switch to next value\n");
			sRtcControl->SubState++;					
			if(sRtcControl->SubState > RTC_SET_ALARM_MODE)
			{
				sRtcControl->SubState = RTC_SET_YEAR;
			}
			
			if(sRtcControl->AlarmMode == ALARM_MODE_ONCE_ONLY)
			{
				if(sRtcControl->SubState == RTC_SET_WEEK)//�������Ӳ�֧����ʱ������
				{
					sRtcControl->SubState++;
				}
			}
			else if(sRtcControl->AlarmMode == ALARM_MODE_PER_DAY)//ÿ�����Ӳ�֧��������������
			{
				if(sRtcControl->SubState < RTC_SET_HOUR)
				{
					sRtcControl->SubState = RTC_SET_HOUR;
				}
			}
			else//�����Ӳ�֧������������
			{
				if(sRtcControl->SubState < RTC_SET_WEEK)
				{
					sRtcControl->SubState = RTC_SET_WEEK;
				}
			}
			
			APP_DBG("sRtcControl->SubState:%d\n", sRtcControl->SubState);
			break;
#endif
			
		default:
			break;
	}
	
	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
	return TRUE;
}

void RtcControlSetTime(void)
{
	if(sRtcControl->State == RTC_STATE_IDLE)
	{
		APP_DBG("Set Rtc Time Mode\n");
		TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
		sRtcControl->State = RTC_STATE_SET_TIME;
		sRtcControl->SubState = RTC_SET_YEAR;
		RtcSubStateDbg();
	}
	else if(sRtcControl->State == RTC_STATE_SET_TIME)
	{
		RtcSwitchNextSubState();
		RtcSubStateDbg();
	}
}

#ifdef FUNC_RTC_ALARM
void RtcControlSetAlarm(void)
{
	if(sRtcControl->State == RTC_STATE_IDLE)
	{
		APP_DBG("Set Alarm Time Mode\n");
		if((sRtcControl->AlarmNum == 0) || (sRtcControl->AlarmNum > MAX_ALARM_NUM))
		{
			sRtcControl->AlarmNum = 1;
		}
		RtcGetAlarmTime(sRtcControl->AlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		APP_DBG("Alarm Time Mode: %d\n", sRtcControl->AlarmMode);
		if((sRtcControl->AlarmMode < ALARM_MODE_ONCE_ONLY) || (sRtcControl->AlarmMode >= ALARM_MODE_MAX))
		{
			sRtcControl->AlarmMode = ALARM_MODE_ONCE_ONLY;
		}
		
		TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
		sRtcControl->State = RTC_STATE_SET_ALARM;
		sRtcControl->SubState = RTC_SET_ALARM_NUM;
		RtcSubStateDbg();
	}
	else if(sRtcControl->State == RTC_STATE_SET_ALARM)
	{
		RtcSwitchNextSubState();
		RtcSubStateDbg();
	}
}
#endif

void RtcTimeUp(RTC_DATE_TIME *Time)
{
	switch(sRtcControl->SubState)
	{
		case RTC_SET_YEAR:
			Time->Year++;
			if(Time->Year > 2099)
			{
				Time->Year = 1980;	
			}
			break;
		case RTC_SET_MON:
			Time->Mon++;
			if(Time->Mon > 12)
			{
				Time->Mon = 1;
			}
			if(Time->Date > RtcGetMonthDays(Time->Year, Time->Mon))
			{
				Time->Date = RtcGetMonthDays(Time->Year, Time->Mon);
			}
			break;
		case RTC_SET_WEEK:
			Time->WDay++;
			if(Time->WDay > 6)
			{
				Time->WDay = 0;	
			}
			break;
		case RTC_SET_DAY:	
			Time->Date++;
			if(Time->Date > RtcGetMonthDays(Time->Year, Time->Mon))
			{
				Time->Date = 1;
			}
			break;
		case RTC_SET_HOUR:
			Time->Hour++;
			Time->Sec = 0;
			if(Time->Hour > 23)
			{
				Time->Hour = 0;
			}
			break;
		case RTC_SET_MIN:
			Time->Min++;
			Time->Sec = 0;
			if(Time->Min > 59)
			{
				Time->Min = 0;
			}
			break;
		default:
			break;
	}
}

void RtcTimeDown(RTC_DATE_TIME *Time)
{
	switch(sRtcControl->SubState)
	{	
		case RTC_SET_YEAR:
			Time->Year--;
			if(Time->Year < 1980)
			{
				Time->Year = 2099;	
			}
			break;
		case RTC_SET_MON:
			Time->Mon--;
			if(Time->Mon == 0)
			{
				Time->Mon = 12;
			}
			if(Time->Date > RtcGetMonthDays(Time->Year, Time->Mon))
			{
				Time->Date = RtcGetMonthDays(Time->Year, Time->Mon);
			}
			break;
		case RTC_SET_WEEK:
			Time->WDay--;			
			if(Time->WDay > 6)
			{
				Time->WDay = 6;	
			}
			break;
		case RTC_SET_DAY:	
			Time->Date--;
			if(Time->Date == 0)
			{
				Time->Date = RtcGetMonthDays(Time->Year, Time->Mon);
			}
			break;
		case RTC_SET_HOUR:
			Time->Hour--;
			Time->Sec = 0;
			if(Time->Hour > 23)
			{
				Time->Hour = 23;
			}
			break;
		case RTC_SET_MIN:
			Time->Min--;
			Time->Sec = 0;
			if(Time->Min > 59)
			{
				Time->Min = 59;
			}
			break;
		default:
			break;
	}
}

// ϵͳʱ�䵱ǰ������ֵ����
void RtcSetTimeUp(void)
{
	APP_DBG("RtcSetValueUp(state = %d, substate = %d)\n", sRtcControl->State, sRtcControl->SubState);
	if(RTC_STATE_IDLE == sRtcControl->State)
	{
		return;
	}
	
	if(RTC_STATE_SET_TIME == sRtcControl->State)
	{
		RtcTimeUp(&sRtcControl->DataTime);
		RtcSetCurrTime(&sRtcControl->DataTime);
	}
#ifdef FUNC_RTC_ALARM
	else if(RTC_STATE_SET_ALARM == sRtcControl->State)
	{
		if(RTC_SET_ALARM_NUM == sRtcControl->SubState)
		{
			sRtcControl->AlarmNum++;
			if(sRtcControl->AlarmNum > MAX_ALARM_NUM)
			{
				sRtcControl->AlarmNum = 1;
			}
			RtcGetAlarmTime(sRtcControl->AlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		}
		else if(RTC_SET_ALARM_MODE == sRtcControl->SubState)
		{
			sRtcControl->AlarmMode++;
			if(sRtcControl->AlarmMode >= ALARM_MODE_MAX)
			{
				sRtcControl->AlarmMode = ALARM_MODE_ONCE_ONLY;
			}
			
			if(sRtcControl->AlarmMode == ALARM_MODE_USER_DEFINED)
			{
				sRtcControl->AlarmData = 0x4A; // �ͻ������Լ��޸�
			}
			else if(sRtcControl->AlarmMode == ALARM_MODE_WORKDAY)
			{
				sRtcControl->AlarmData = 0x3E;
			}
			else
			{
				sRtcControl->AlarmData = 0x0;
			}
		}
		else
		{
			RtcTimeUp(&sRtcControl->AlarmTime);
		}
		APP_DBG("+++sRtcControl->ModeData;%x\n", sRtcControl->AlarmData);
		RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
#ifdef FUNC_RTC_ALARM		
		NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
	}
#endif
	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
}

// ϵͳʱ�䵱ǰ������ֵ�ļ�
void RtcSetTimeDown(void)
{
	APP_DBG("RtcSetValueDown(state = %d, substate = %d)\n", sRtcControl->State, sRtcControl->SubState);

	if(RTC_STATE_IDLE == sRtcControl->State)
	{
		return;
	}
	
	if(RTC_STATE_SET_TIME == sRtcControl->State)
	{
		RtcTimeDown(&sRtcControl->DataTime);
		RtcSetCurrTime(&sRtcControl->DataTime);
	}
#ifdef FUNC_RTC_ALARM
	else if(RTC_STATE_SET_ALARM == sRtcControl->State)
	{	
		if(RTC_SET_ALARM_NUM == sRtcControl->SubState)
		{
			sRtcControl->AlarmNum--;
			if((sRtcControl->AlarmNum == 0) || (sRtcControl->AlarmNum > MAX_ALARM_NUM))
			{
				sRtcControl->AlarmNum = MAX_ALARM_NUM;
			}
			RtcGetAlarmTime(sRtcControl->AlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		}
		else if(RTC_SET_ALARM_MODE == sRtcControl->SubState)
		{
			sRtcControl->AlarmMode--;
			if((sRtcControl->AlarmMode < ALARM_MODE_ONCE_ONLY) || (sRtcControl->AlarmMode >= ALARM_MODE_MAX))
			{
				sRtcControl->AlarmMode = (ALARM_MODE_MAX - 1);
			}
			
			if(sRtcControl->AlarmMode == ALARM_MODE_USER_DEFINED)
			{
				sRtcControl->AlarmData = 0x4A; // �ͻ������Լ��޸�
			}
			else if(sRtcControl->AlarmMode == ALARM_MODE_WORKDAY)
			{
				sRtcControl->AlarmData = 0x3E;
			}
			else
			{
				sRtcControl->AlarmData = 0x0;
			}
		}
		else
		{
			RtcTimeDown(&sRtcControl->AlarmTime);
		}
		APP_DBG("---sRtcControl->ModeData;%x\n", sRtcControl->AlarmData);
		RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
#ifdef FUNC_RTC_ALARM		
		NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
	}
#endif
	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
}

bool RtcControl(void)
{
	uint16_t Msg  = 0;

	APP_DBG("Enter RTC\n");
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif
    
	//��ʼ��
	if(!RtcInitialize())
	{
		return FALSE;
	}
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(Msg != MSG_COMMON_CLOSE)
	{
		CheckTimer(&sRtcControl->TimerHandle);

		Msg = MsgRecv(20);
#ifdef FUNC_RTC_ALARM	
		if(sRtcControl->State == RTC_STATE_IDLE)
		{
			if(sRtcControl->CurAlarmNum)
			{
				APP_DBG("RTC ALARM(%d) COME!\n", sRtcControl->CurAlarmNum);
				RtcAlarmArrivedProcess();
				//���ᴦ��
				sRtcControl->CurAlarmNum = 0;
#ifdef FUNC_RTC_ALARM				
				NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
			}
		}
#endif
		switch(Msg)
		{
			case MSG_POWER:
				gSys.NextModuleID = MODULE_ID_STANDBY;
				Msg = MSG_COMMON_CLOSE;
				break;
			
			case MSG_MODE:
				APP_DBG("Exit RTC\n");
				Msg = MSG_COMMON_CLOSE;
				break;

			case MSG_RTC_SET_TIME:
				RtcControlSetTime();
				break;
			
#ifdef FUNC_RTC_ALARM			
			case MSG_RTC_SET_ALARM:
				RtcControlSetAlarm();
				break;
#endif						
				// �������ֵ����
			case MSG_VOL_UP:
				RtcSetTimeUp();
				break;
			
				// �������ֵ����
			case MSG_VOL_DW:
				RtcSetTimeDown();
				break;
			
#ifdef FUNC_RECORD_EN
			case MSG_REC:  //¼��
				gSys.ModuleIDBackUp = gSys.CurModuleID;
				gSys.NextModuleID = MODULE_ID_RECORDER;
				gSys.RecordState = RECORD_STATE_REC;
				gSys.RecordSource = RECORD_SOURCE_LINEIN;
				Msg = MSG_COMMON_CLOSE;
				break;
#endif
           
#ifdef FUNC_REC_PLAYBACK_EN
			case MSG_REC_PLAYBACK:
				EnterRecBackPlay(gSys.CurModuleID, IsUDiskLink() ? DEV_ID_USB : DEV_ID_SD, TRUE, 0, 0);
				break;
#endif			

			default:
				break;
		}

		if((sRtcControl->State != RTC_STATE_IDLE) && (IsTimeOut(&RtcAutoOutTimer)))
		{
			APP_DBG("Rtc Normer Mode\n");
			sRtcControl->State = RTC_STATE_IDLE;
		}
	}
	RtcDeinitialize();//ģʽ�л����˳�RTC
	return TRUE;
}


#ifdef FUNC_RTC_ALARM
__attribute__((section(".driver.isr"), weak)) void RtcInterrupt(void)
{
	sRtcControl->CurAlarmNum = RtcCheckAlarmFlag();
	RtcAlarmIntClear();// ������������ж�
}
#endif

#endif
