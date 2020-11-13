////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:app_msg.h
//
//            maintainer: Halley
//
//		Description	:
//					�û������޸ĸ��ļ��е���Ϣ������Ϣ
//					��Ϣ������Ϣֵ�ĸ�6Bits��ͬ��һ����Ϣ�ļ���
//					ÿ����Ϣ��������255����Ϣ
///////////////////////////////////////////////////////////////////////////////

#ifndef	__APP_MSG_H__
#define __APP_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

	
// ����ģ����Ϣ������
enum
{	
	MSG_NONE				= 0x0000,	
	
	////������Ϣ������ģʽ�µİ���������Ϣ///////////////////////////////////////
	MSG_MAIN_CLASS			= 0x1000, 	
	MSG_COMMON_CLOSE,				// ͨ��ģ�������˳���Ϣ��������WIN32��WM_CLOSE
	MSG_STOP,						// ֹͣ
    MSG_EQ,               			// EQ�л�
	MSG_REPEAT,
    MSG_FOLDER_MODE,				// �򿪡��ر��ļ��в���ģʽ
    MSG_FOLDER_NEXT,				// ��һ���ļ���
    MSG_FOLDER_PRE,					// ��һ���ļ���
    MSG_BROWSE,						// �ļ����
    MSG_VOL_UP,
    MSG_VOL_DW,
    MSG_PLAY_PAUSE,
    MSG_FF_START,
    MSG_FB_START,
	MSG_FF_FB_END,
    MSG_PRE,
    MSG_NEXT,
    MSG_REC,
    MSG_REC_PLAYBACK,
    MSG_REPEAT_AB,
    MSG_MUTE,
    MSG_DISK,
    MSG_POWER,
    MSG_REMIND,
	MSG_DRC,
    MSG_3D,
    MSG_VB,    
    MSG_LANG,
    MSG_MODE,
	MSG_REC_FILE_DEL,	
	MSG_MENU,	
	MSG_MIC_VOL_UP,
	MSG_MIC_VOL_DW,
	MSG_ECHO_DELAY_UP,
	MSG_ECHO_DELAY_DW,
	MSG_ECHO_DEPTH_UP,
	MSG_ECHO_DEPTH_DW,
	MSG_TREB_UP,
	MSG_TREB_DW,
	MSG_BASS_UP,
	MSG_BASS_DW,
	
	MSG_NUM_0,	
	MSG_NUM_1,	
	MSG_NUM_2,	
	MSG_NUM_3,	
	MSG_NUM_4,	
	MSG_NUM_5,	
	MSG_NUM_6,	
	MSG_NUM_7,	
	MSG_NUM_8,	
	MSG_NUM_9,	
	
	// RTC
	MSG_RTC_SET_TIME,
	MSG_RTC_SET_ALARM,
	
	//PowerKey
	MSG_PWR_KEY_SP,

	// play control(�ǰ�����������Ϣ)
	MSG_NEXT_SONG,		
	MSG_PRE_SONG,
	
	////�豸�����Ϣ/////////////////////////////////////////////////////////////
	MSG_DEV_CLASS			= 0x1100,
    MSG_USB_PLUGIN,
    MSG_USB_PLUGOUT,

	MSG_RTC_START_ALARM_REMIND,
	MSG_RTC_STOP_ALARM_REMIND,

    MSG_SD_PLUGIN,
    MSG_SD_PLUGOUT,

    MSG_LINEIN_PLUGIN,
    MSG_LINEIN_PLUGOUT,
	
    MSG_MIC_PLUGIN,
    MSG_MIC_PLUGOUT,
	
    MSG_PC_PLUGIN,
    MSG_PC_PLUGOUT,

	////DECODER////////////////////////////////////////////////////////////////
	MSG_DECODER_INT_CLASS	= 0x1200,
    MSG_DECODER_XR_DONE,			// decoder XR_DONE�ź�
	MSG_SBC_DATA_READY,

	////USB_DEVICE��Ϣ/////////////////////////////////////////////////////////////
	MSG_USB_DEVICE_CLASS	= 0x1300,
	MSG_USB_DEVICE_INTERRUPT_CB,

	////����Э��ջ��Ϣ/////////////////////////////////////////////////////////////
	MSG_BT_CLASS			= 0x1400,  
    MSG_BT_HF_INTO_MODE, 		// ��������ģʽ
    MSG_BT_HF_OUT_MODE,  		// �˳�����ģʽ

	MSG_BT_CONNECTED,
	MSG_BT_DISCONNECTED,

	MSG_BT_PHONE_CALL_INCOMING,

	MSG_DECODR_BT_CLASS		= 0x1500,
	MSG_NEED_MORE_SBC_DATA,	
	MSG_BT_STACK_RUN,
};

#pragma pack()

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__APP_MSG_H__
