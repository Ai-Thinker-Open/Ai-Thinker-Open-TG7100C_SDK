////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:dev_detect_driver.c
//
//            maintainer: Halley
//
//		Description	:
//					a device detect driver(realtime to check sd/usb/linein/.. plugin or plugout event)
//
//		Changelog	:
//					2013-06-13	Created Source File
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "app_config.h"
#include "dev_detect_driver.h"
#include "bt_app_func.h"
//#include "player_control.h"
#include "sound_remind.h"
#include "recorder_control.h"
#include "breakpoint.h"
#include "sys_app.h"
#include "sys_vol.h"

typedef struct _DEV_APP_MODE_MAP
{
	uint8_t  ModeId;			//ģʽID
	uint16_t DevPlugInMsgId;	//�豸�����¼�
	uint16_t DevPlugOutMsgId;	//�豸�γ��¼�
	bool (*IsDevLink)(void);	//�豸���Ӽ�⺯��ָ��
	
} DEV_APP_MODE_MAP;

static const DEV_APP_MODE_MAP DevAppModeMap[] =
{
#ifdef FUNC_BT_HF_EN
	//{MODULE_ID_BT_HF,			MSG_BT_HF_INTO_MODE,	MSG_BT_HF_OUT_MODE,		IsHfModeEvnOK},
#endif

#ifdef FUNC_USB_EN
	{MODULE_ID_PLAYER_USB,		MSG_USB_PLUGIN,			MSG_USB_PLUGOUT,		IsUDiskLink},
#endif

#ifdef FUNC_CARD_EN
	{MODULE_ID_PLAYER_SD, 		MSG_SD_PLUGIN,			MSG_SD_PLUGOUT,			IsCardLink},
#endif

#ifdef FUNC_LINEIN_EN
	{MODULE_ID_LINEIN, 			MSG_LINEIN_PLUGIN,		MSG_LINEIN_PLUGOUT,		IsLineInLink},
#endif

#ifdef FUNC_I2SIN_EN
    {MODULE_ID_I2SIN,           MSG_NONE,               MSG_NONE,               IsI2sInLink},
#endif

#ifdef FUNC_RADIO_EN
	{MODULE_ID_RADIO, 			MSG_NONE,				MSG_NONE,				IsRadioLink},
#endif

#ifdef FUNC_USB_AUDIO_READER_EN
	{MODULE_ID_USB_AUDIO_READER, MSG_PC_PLUGIN,			MSG_PC_PLUGOUT,			IsPcLink},
#endif

#ifdef FUNC_USB_AUDIO_EN
	{MODULE_ID_USB_AUDIO,		MSG_PC_PLUGIN,			MSG_PC_PLUGOUT,			IsPcLink},
#endif

#ifdef FUNC_USB_READER_EN
	{MODULE_ID_USB_READER,		MSG_PC_PLUGIN,			MSG_PC_PLUGOUT,			IsPcLink},
#endif

#ifdef FUNC_BT_EN
	//{MODULE_ID_BLUETOOTH,		MSG_NONE,				MSG_NONE,				IsBtLink},
#endif
	
#ifdef FUNC_RTC_EN
	{MODULE_ID_RTC, 			MSG_NONE,				MSG_NONE,				NULL},
#endif
};

#define MODE_CNT	(sizeof(DevAppModeMap) / sizeof(DevAppModeMap[0]))	


//ͨ���豸�����Ϣ��ȡ��Ӧ��ģʽID��
static uint32_t GetModeId(uint16_t Msg)
{
	uint32_t i;

	for(i = 0; i < MODE_CNT; i++)
	{
		if((Msg == DevAppModeMap[i].DevPlugInMsgId) || (Msg == DevAppModeMap[i].DevPlugOutMsgId))
		{
			return DevAppModeMap[i].ModeId;
		}
	}

	return MODULE_ID_UNKNOWN;
}

bool IsModeEnvOk(uint32_t ModeId)
{
	uint32_t i;

	for(i = 0; i < MODE_CNT; i++)
	{
		if(ModeId == DevAppModeMap[i].ModeId)
		{
			if(DevAppModeMap[i].IsDevLink == NULL)
			{
				return TRUE;
			}
			return DevAppModeMap[i].IsDevLink();
		}
	}
	return FALSE;
}

// �ӵ�ǰģʽ��ʼ��˳���л���״̬������һ��ģʽ
uint32_t GetNextModeId(uint32_t CurModeId)
{
	uint32_t i;
	uint32_t j;

	for(i = 0; i < MODE_CNT; i++)
	{
		if(CurModeId == DevAppModeMap[i].ModeId)
		{
			i++;
			break;
		}
	}
	
	for(j = 0; j < MODE_CNT; j++)
	{
		if((DevAppModeMap[(i + j) % MODE_CNT].IsDevLink == NULL) || DevAppModeMap[(i + j) % MODE_CNT].IsDevLink())
		{
			return DevAppModeMap[(i + j) % MODE_CNT].ModeId;
		}
	}
	return MODULE_ID_UNKNOWN;
}

// �����ȼ�˳�򣬴�������ȼ�ģʽ��ʼ���ҵ�һ����Чģʽ
uint32_t GetNextModeIdByProi(uint32_t CurModeId)
{
	uint32_t i;

	for(i = 0; i < MODE_CNT; i++)
	{
		if(CurModeId == DevAppModeMap[i].ModeId)
		{
			continue;
		}

		if((DevAppModeMap[i].IsDevLink == NULL) || DevAppModeMap[i].IsDevLink())
		{
			return DevAppModeMap[i].ModeId;
		}
	}
	return MODULE_ID_UNKNOWN;
}

static bool GotoNextMode(void)
{
	if(gSys.NextModuleID == MODULE_ID_PLAYER_SD)
	{
		gSys.DiskType = SD_TYPE;
	}
	else if(gSys.NextModuleID == MODULE_ID_PLAYER_USB)
	{
		gSys.DiskType = USB_TYPE;
	}

	MsgSend(MSG_COMMON_CLOSE);
	return TRUE;	
}


//�豸״̬��Ϣ(Plugin��PlugOut)�����ɹ����� ����TRUE�����򷵻�FALSE
bool SysDeviceMsgCtrl(uint16_t Msg)
{
//	uint32_t CurMode;
	uint32_t DevModuleId;

	switch(Msg)
	{
		case MSG_SD_PLUGIN:
			if((gSys.CurModuleID == MODULE_ID_USB_READER) || (gSys.CurModuleID == MODULE_ID_USB_AUDIO_READER))
            {
                return TRUE;
            }
		case MSG_USB_PLUGIN:
		case MSG_PC_PLUGIN:
		case MSG_LINEIN_PLUGIN:
		case MSG_RTC_START_ALARM_REMIND:
            if((gSys.CurModuleID == MODULE_ID_RECORDER) && (gSys.RecordState == RECORD_STATE_REC))
            {
                return TRUE;
            }

			gSys.NextModuleID = GetModeId(Msg);
			return GotoNextMode();	

		case MSG_SD_PLUGOUT:
			if(!IsCardLink())
			{
				MsgClear(MSG_SD_PLUGIN);
			}
		case MSG_USB_PLUGOUT:
			if(!IsUDiskLink())
			{
				MsgClear(MSG_USB_PLUGIN);
			}
		case MSG_PC_PLUGOUT:
			if(!IsPcLink())
			{
				MsgClear(MSG_PC_PLUGIN);
			}
		case MSG_LINEIN_PLUGOUT:
			if(!IsLineInLink())
			{
				MsgClear(MSG_LINEIN_PLUGIN);
			}
		case MSG_RTC_STOP_ALARM_REMIND:
			MsgClear(MSG_RTC_START_ALARM_REMIND);

			if(Msg == MSG_SD_PLUGOUT)
			{
				FSDeInit(DEV_ID_SD);
			}
			else if(Msg == MSG_USB_PLUGOUT)
			{
				FSDeInit(DEV_ID_USB);
			}
	
			DevModuleId = GetModeId(Msg);
			if(DevModuleId != gSys.CurModuleID)
			{
				//�γ��Ĳ��ǵ�ǰģʽ��Ӧ���豸������Ը��¼�
				return FALSE;
			}
			//�γ��ĵ�ǰģʽ��Ӧ���豸����������˳���е���һ��ģʽ
			gSys.NextModuleID = GetNextModeIdByProi(MODULE_ID_UNKNOWN);
			return GotoNextMode();			
			
		case MSG_BT_HF_INTO_MODE:
			if(gSys.NextModuleID == MODULE_ID_BT_HF || gSys.CurModuleID == MODULE_ID_BT_HF)
				return FALSE;
			gSys.NextModuleID = MODULE_ID_BT_HF;
			return GotoNextMode();
		case MSG_BT_HF_OUT_MODE:
			APP_DBG("MSG_BT_HF_OUT_MODE SetNextMode:%d\n", gSys.NextModuleID);
			return GotoNextMode();

		// ������ʾ��
		case MSG_BT_CONNECTED:
			//SoundRemind(SOUND_BT_CONNECT);
			return FALSE;

		case MSG_BT_DISCONNECTED:
			//SoundRemind(SOUND_BT_DISCONNECT);
			return FALSE;
		case MSG_BT_PHONE_CALL_INCOMING:
			return FALSE;


        case MSG_MIC_PLUGIN:
			if(gSys.CurModuleID != MODULE_ID_RECORDER)
			{
				//SoundRemind(SOUND_MIC_INSERT);
			}

			if(gSys.CurModuleID == MODULE_ID_LINEIN)
			{
				AudioAnaSetChannel(AUDIO_CH_MIC_LINEIN);
			}
			else if(gSys.CurModuleID == MODULE_ID_RADIO)
			{
				AudioAnaSetChannel(AUDIO_CH_MIC_FM);
			}
			else if(gSys.CurModuleID == MODULE_ID_RECORDER)
			{
				if(gSys.RecordSource == RECORD_SOURCE_LINEIN)
				{
					AudioAnaSetChannel(AUDIO_CH_MIC_LINEIN);
				}
				else if(gSys.RecordSource == RECORD_SOURCE_FM)
				{
					AudioAnaSetChannel(AUDIO_CH_MIC_FM);
				}
			}
			else 
			{
				AudioAnaSetChannel(AUDIO_CH_MIC);
			}	

			return FALSE;
			
		case MSG_MIC_PLUGOUT:
#ifdef FUNC_MIC_DET_EN
			if(!IsMicInLink())
			{
				MsgClear(MSG_MIC_PLUGIN);
				//return FALSE;
			}
#endif
			if(gSys.CurModuleID != MODULE_ID_RECORDER)
			{
				SoundRemind(SOUND_MIC_PULLOUT);  
			}

			if(gSys.CurModuleID == MODULE_ID_LINEIN)
			{
				AudioAnaSetChannel(AUDIO_CH_LINEIN);
			}
			else if(gSys.CurModuleID == MODULE_ID_RADIO)
			{
				AudioAnaSetChannel(AUDIO_CH_FM);
			}
			else if(gSys.CurModuleID == MODULE_ID_RECORDER)
			{
				if(gSys.RecordSource == RECORD_SOURCE_LINEIN)
				{
					AudioAnaSetChannel(AUDIO_CH_LINEIN);
				}
				else if(gSys.RecordSource == RECORD_SOURCE_FM)
				{
					AudioAnaSetChannel(AUDIO_CH_FM);
				}
			}
			else 
			{
				AudioAnaSetChannel(AUDIO_CH_NONE);
			}	
			return FALSE;
			
		default:
			break;
	}

	return FALSE;
}

