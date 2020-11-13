///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: sound_remind.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#include "app_config.h"
#include "sound_remind.h"
#include "task_decoder.h"
#include "dac.h"
#include "clk.h"
#include "bt_app_func.h"
#include "pcm_fifo.h"
#include "spi_flash.h"
#include "mixer.h"
#include "sys_app.h"
#include "recorder_control.h"

//#ifdef FUNC_AUDIO_VB_EFFECT_EN
//extern bool gVbEffectInited; 
//#endif

#ifdef FUNC_SOUND_REMIND
//extern const uint16_t gDecVolArr[MAX_VOLUME + 1];
extern bool UsbAudioPlayFlag;	//������ʾ��ʱ��ֹ���������� 
extern xTaskHandle AudioProcessHandle;
#ifdef FUNC_RADIO_DIV_CLK_EN
extern void SysClkDivison(char NewClkDiv);
#endif
//#ifdef FUNC_RECORD_EN
//extern uint8_t RecordState;
//#endif

bool IsMsgInQueue(void);

bool InterruptSoundRemind = FALSE;

//��Ӣ��˫����ʾ��
typedef struct _SOUND_TABLE_ITEM
{
 	uint16_t Id;
	uint32_t IdChn;
	uint32_t IdEng;

} SOUND_TABLE_ITEM;

#define STR_TO_UINT(s)	((s[0] << 0) | (s[1] << 8) | (s[2] << 16) | (s[3] << 24))	
static const SOUND_TABLE_ITEM SoundTable[] = 
{
//   ID                                 ����ID                  Ӣ��ID
	0,0,0
};

typedef struct _SOUND_REMIND_CONTROL
{
	bool     IsRunning;

	uint32_t ConstDataAddr;
	uint32_t ConstDataSize;
	uint32_t ConstDataOffset;

} SOUND_REMIND_CONTROL;

static SOUND_REMIND_CONTROL SoundRemindControl;
static MemHandle SoundRemindFile;		//��ʾ���ļ�
static uint8_t SoundRemindFileBuf[256];	//��ʾ���ļ�������

void SoundRemindInit(void)
{
	memset(&SoundRemindControl, 0x00, sizeof(SoundRemindControl));
	memset(&SoundRemindFile, 0x00, sizeof(SoundRemindFile));
}

uint32_t SoundRemindFillStreamCallback(void *buffer, uint32_t length)
{
    int32_t RemainBytes = SoundRemindControl.ConstDataSize - SoundRemindControl.ConstDataOffset;
    int32_t ReadBytes   = length > RemainBytes ? RemainBytes : length;

    if(ReadBytes == 0)
	{
		return 0;	//�˴β���������
	}
	
    if(SpiFlashConstDataRead((uint8_t*)buffer,
	                         ReadBytes,
	                         SoundRemindControl.ConstDataAddr + SoundRemindControl.ConstDataOffset) < 0)
	{
		ReadBytes = 0;
        APP_DBG("read const data error!\r\n");
	}

	SoundRemindControl.ConstDataOffset += ReadBytes;

	return ReadBytes;
}

/////////////////////////////////////////////////////////////////////////
// stream ����������䣨decoder taskÿ�ν�֡ʱ���ã�
// return -1 -- stream over; 0 -- no fill; other value -- filled bytes
int32_t SoundRemindFillStream(void)
{
	int32_t FillBytes;	//����Ҫ���ص����ݳ���
	int32_t LeftBytes;	//�ļ������������е����ݳ���

	LeftBytes = (SoundRemindFile.mem_len - SoundRemindFile.p);
	FillBytes = SoundRemindControl.ConstDataSize - SoundRemindControl.ConstDataOffset;
	if(FillBytes > SoundRemindFile.mem_capacity - LeftBytes)
	{
		FillBytes = SoundRemindFile.mem_capacity - LeftBytes;
	}

	if((LeftBytes > 0) && (SoundRemindFile.p == 0))
	{
		return 0;	//�������ݣ���������û������
	} 

	if(FillBytes == 0)
	{
		return -1;	//���Ž���
	}

	memcpy(SoundRemindFile.addr, SoundRemindFile.addr + SoundRemindFile.p, LeftBytes);
	if(SpiFlashConstDataRead((uint8_t*)(SoundRemindFile.addr + LeftBytes),
	                         FillBytes,
	                         SoundRemindControl.ConstDataAddr + SoundRemindControl.ConstDataOffset) < 0)
	{
		APP_DBG("read const data error!\n");
	}
		
	SoundRemindFile.p = 0;
	SoundRemindFile.mem_len = LeftBytes + FillBytes;
	SoundRemindControl.ConstDataOffset += FillBytes;

	return FillBytes;
}

//����������ʾ��
//ע�⣺�ú���������main task�е��ã����������������
void SoundRemind(uint16_t SoundId)
{     
	uint16_t i;
	uint32_t ConstDataId;
	
	uint8_t DivFreqFlag = 0;//����Ƶ���
	uint8_t SysClkSel,SysClkDivNum;
	
	if(!gSys.SoundRemindOn)
	{
		return;
	}
#ifdef FUNC_RECORD_EN
	if(gSys.CurModuleID == MODULE_ID_RECORDER && (RecordState == RECORD_STATE_REC || RecordState == RECORD_STATE_REC_PAUSE))
	{
		return;
	}
#endif
	if(SoundRemindControl.IsRunning)
	{
		return;
	}
	APP_DBG("SoundRemind id : %d\n", SoundId);

	SoundRemindControl.IsRunning = 1;
	SoundRemindControl.ConstDataSize = 0;
	SoundRemindControl.ConstDataAddr = 0;
	SoundRemindControl.ConstDataOffset = 0;
	
	SoundRemindFile.addr = SoundRemindFileBuf;
	SoundRemindFile.mem_capacity = sizeof(SoundRemindFileBuf);    // �����ݻ����С
	SoundRemindFile.mem_len = 0;
	SoundRemindFile.p = 0;

	//ͨ��SoundId���Ҷ�Ӧ��ConstDataId
	for(i = 0; i < sizeof(SoundTable)/sizeof(SOUND_TABLE_ITEM); i++)
	{
		if(SoundTable[i].Id == SoundId)
		{
			ConstDataId = (gSys.LanguageMode == LANG_ZH) ? SoundTable[i].IdChn : SoundTable[i].IdEng; 
			break;	//�ҵ�
		}
	}	
	if(i >= sizeof(SoundTable)/sizeof(SOUND_TABLE_ITEM))
	{
		APP_DBG("Can not get the SoundId : %d\n", SoundId);
		SoundRemindControl.IsRunning = 0;
		return;		//δ�ҵ�
	}
	
	if(SpiFlashConstGetInfo(ConstDataId, &SoundRemindControl.ConstDataSize, &SoundRemindControl.ConstDataAddr) < 0)
	{
		APP_DBG("Get const data info error!\n");
		SoundRemindControl.IsRunning = 0;
		return;
	}

#ifdef FUNC_BT_HF_EN
	//if(GetHfModeState() == BT_HF_MODE_STATE_ENTERING)
	{
		SoundRemindControl.IsRunning = 0;
		return;
	}
#endif

#ifdef FUNC_RADIO_DIV_CLK_EN
	if(0 == gSys.IsSysClk96MHz)
	{
		OSTaskSuspend(AudioProcessHandle);
		SysClkSel = ClkSysClkSelGet();//1:80M ,  0:96M  //?????RC?????
		SysClkDivNum = ClkModuleDivGet();
		ClkSysClkSel(0); // �л���96M
		SysClkDivison(1);	//ljg, call directly
        gSys.IsSysClk96MHz = 1; 
		DivFreqFlag = 1;
	}
#endif
	SoundRemindFillStream();

	// ����Ƿ���ָ������Ϣ
	if(IsMsgInQueue())
	{
		SoundRemindControl.IsRunning = 0;
        //resume the audio task before returning to deal message -Robert 20150204
        OSTaskResume(AudioProcessHandle);
		return;
	}

	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		//BTAudioPlayStop();
		//DecoderTaskSyncPlay(&SoundRemindFile, IO_TYPE_MEMORY);
		//BTAudioPlayStart();
	}
	else
	{
#if defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_READER_EN) || defined(FUNC_USB_AUDIO_READER_EN)
		//UsbAudioPlayFlag = FALSE;
#endif
	 	MixerMute(MIXER_SOURCE_ANA_MONO);
		MixerMute(MIXER_SOURCE_ANA_STERO);
		//DecoderTaskSyncPlay(&SoundRemindFile, IO_TYPE_MEMORY);
		MixerUnmute(MIXER_SOURCE_ANA_MONO);
		MixerUnmute(MIXER_SOURCE_ANA_STERO);
#if defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_READER_EN) || defined(FUNC_USB_AUDIO_READER_EN)		
		UsbAudioPlayFlag = TRUE;
#endif
	}

#ifdef FUNC_RADIO_DIV_CLK_EN
	if(DivFreqFlag)//�ָ�ԭƵ��
	{
		if(1 == SysClkSel)
		{
			ClkSysClkSel(1); 
		}
		if(SysClkDivNum > 1)
		{
			SysClkDivison(SysClkDivNum);	//ljg, call directly
		}
		gSys.IsSysClk96MHz = 0;  
		OSTaskResume(AudioProcessHandle);
	}
#endif
	SoundRemindControl.IsRunning = 0;

#ifdef FUNC_AUDIO_VB_EFFECT_EN
	if(gSys.AudioVbEffectFlag)
	{
		gVbEffectInited = FALSE;
	}
#endif
}

bool IsSoundRemindPlaying(void)
{
	return SoundRemindControl.IsRunning;
}

void StopSoundRemind(void)
{
	APP_DBG("StopSoundRemind---\n");
	InterruptSoundRemind = TRUE;
}

void ResetSoundRemindStopFlag(void)
{
	InterruptSoundRemind = FALSE;
}

bool IsMsgInQueue(void)
{
	bool	ret = FALSE;
	
	if( MsgCheck(MSG_BT_HF_OUT_MODE) 
		|| MsgCheck(MSG_BT_HF_INTO_MODE)
		|| MsgCheck(MSG_USB_PLUGIN)
		|| MsgCheck(MSG_USB_PLUGOUT)
		|| MsgCheck(MSG_SD_PLUGIN)
		|| MsgCheck(MSG_SD_PLUGOUT)
		|| MsgCheck(MSG_LINEIN_PLUGIN)
		|| MsgCheck(MSG_LINEIN_PLUGOUT)
		|| MsgCheck(MSG_MIC_PLUGIN)
		|| MsgCheck(MSG_MIC_PLUGOUT)
		|| MsgCheck(MSG_PC_PLUGIN)
		|| MsgCheck(MSG_PC_PLUGOUT)	)
	{
		ret = TRUE;
	}

	return ret;
}
#endif


