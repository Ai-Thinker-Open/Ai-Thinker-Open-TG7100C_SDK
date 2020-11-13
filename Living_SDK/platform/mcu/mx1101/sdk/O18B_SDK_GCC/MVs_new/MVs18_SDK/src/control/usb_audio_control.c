///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: card_reader_control.c
//  maintainer: Orson
///////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "type.h"
#include "app_config.h"
#include "usb_audio_control.h"
#include "device_hcd.h"
#include "device_audio.h"
#include "device_stor.h"
#include "audio_adc.h"
#include "nvm.h"
#include "host_hcd.h"
#include "dev_detect_driver.h"
#include "dac.h"
#include "audio_path.h"
#include "clk.h"
#include "pcm_fifo.h"
#include "device_hcd.h"
#include "mixer.h"
#include "sys_vol.h"
#include "breakpoint.h"
#include "sys_app.h"
#include "eq_params.h"
#include "micro_adjust.h"
#include "recorder_control.h"
#include "device_stor.h"
#include "sound_remind.h"

#if defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_READER_EN) || defined(FUNC_USB_AUDIO_READER_EN)
//#ifdef FUNC_AUDIO_3D_EFFECT_EN
//extern bool	g3dEffectInited;
//#endif

//#ifdef FUNC_AUDIO_VB_EFFECT_EN
//extern bool gVbEffectInited;
//#endif

bool UsbAudioPlayFlag = TRUE;	//������ʾ��ʱ��ֹ����������  
//extern uint8_t mixer_pcm_format;

//����ʹ��
void DeviceAudioMuteEn(void)
{
	APP_DBG("DeviceAudioMuteEn()\n");
	MixerMute(MIXER_SOURCE_USB);
}


//����ȡ��
void DeviceAudioMuteDis(void)
{
	APP_DBG("DeviceAudioMuteDis()\n");
	MixerUnmute(MIXER_SOURCE_USB);
}

//��������
void DeviceAudioSetVolume(void)
{
	SetSysVol();
}

static bool UsbIntFlag = FALSE;
//���յ�PC������USB���ݰ�
void DeviceAudioOnCmd(void)
{
	if(!UsbIntFlag)
	{
		UsbIntFlag = TRUE;
		MsgSend(MSG_USB_DEVICE_INTERRUPT_CB); 
	}
}

//���صײ�LIB�е�USB ISO OUT�жϴ�������ÿ1ms�ᱻִ��һ�Σ� ÿ��������192�ֽڡ�
uint8_t TempBuf[192];
//ISO OUT�ϵ��жϴ���������PC����PCM����
void OnDeviceAudioRcvIsoPacket(void)
{
	uint8_t PacketLen;

	MixerIsDone(MIXER_SOURCE_USB);	//ȡ����ǰ����һ��
	PacketLen = UsbDeviceIsoRcv(TempBuf, 192);
   
	if(UsbAudioPlayFlag)
	{
#ifdef FUNC_SPEC_TRUM_EN
		SpecTrumProc((void*)TempBuf, PacketLen/4);
#endif
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
        AudioDRCPro((int16_t *)TempBuf, MIXER_FORMAT_STERO, USB_DAC_FREQ, PacketLen/4);
#endif
#ifdef FUNC_AUDIO_3D_EFFECT_EN
        Audio3dPro((int16_t *)TempBuf, MIXER_FORMAT_STERO, USB_DAC_FREQ, PacketLen/4);
#endif
#ifdef FUNC_AUDIO_VB_EFFECT_EN
        AudioVbPro((int16_t *)TempBuf, MIXER_FORMAT_STERO, USB_DAC_FREQ, PacketLen/4);
#endif
		MixerSetData(MIXER_SOURCE_USB, TempBuf, PacketLen/4);		
	}
}

//44.1KHZ������
//ÿ1������PC����һ��PCM���ݰ�
//ÿ10�����ݰ��У���1�����ݰ�����180�ֽڣ�����9�����ݰ�����176�ֽڣ�ƽ��������������44.1KHZ������
#define DEVICE_AUDIO_UP_DATA_LEN	(88*10 + 2)
uint8_t  DeviceAudioUpData[DEVICE_AUDIO_UP_DATA_LEN];
uint16_t DeviceAudioUpDataR = 0;
uint16_t DeviceAudioUpDataW = 0;

//�����ϴ���PC��PCM����
void DeviceAudioSetUpData(void* Buf, uint16_t Len)
{
	uint16_t R = DeviceAudioUpDataR;
	uint16_t Len1 = DEVICE_AUDIO_UP_DATA_LEN - DeviceAudioUpDataW;
	uint16_t Len2 = (R + DEVICE_AUDIO_UP_DATA_LEN - DeviceAudioUpDataW - 1) % DEVICE_AUDIO_UP_DATA_LEN;
	
	if(Len2 > 0)
	{
		Len2--;
	}
	
	if(Len > Len2)
	{
		Len = Len2;
	}
	
	if(Len1 >= Len)
	{
		memcpy(&DeviceAudioUpData[DeviceAudioUpDataW], Buf, Len);
	}
	else
	{
		memcpy(&DeviceAudioUpData[DeviceAudioUpDataW], Buf, Len1);
		memcpy(&DeviceAudioUpData[0], (uint8_t*)Buf + Len1, Len - Len1);
	}
	DeviceAudioUpDataW = (DeviceAudioUpDataW + Len) % DEVICE_AUDIO_UP_DATA_LEN;
}

//ISO IN�ϵ��жϴ���������PC����PCM����
void OnDeviceAudioSendIsoPacket(void)
{
	uint16_t Len = (DeviceAudioUpDataR == 0) ? 90 : 88;
	uint16_t Len1 = (DeviceAudioUpDataW + DEVICE_AUDIO_UP_DATA_LEN - DeviceAudioUpDataR) % DEVICE_AUDIO_UP_DATA_LEN;
	uint8_t  Buf[180];
	uint16_t i;
	int16_t  Temp;
	uint16_t LeftVolume = AudioMicMuteFlag ? 0 : AudioMicLeftVolume;
	uint16_t RightVolume = AudioMicMuteFlag ? 0 : AudioMicRightVolume;
	
	if(Len > Len1)
	{
		Len = 0;
	}
	
	//������תΪ˫����
	for(i = 0; i < Len/2; i++)
	{	
		Temp = ((int16_t*)&DeviceAudioUpData[DeviceAudioUpDataR])[i];
		((int16_t*)Buf)[2 * i + 0] = (Temp * LeftVolume)/AUDIO_MAX_VOLUME;
		((int16_t*)Buf)[2 * i + 1] = (Temp * RightVolume)/AUDIO_MAX_VOLUME;
	}
		
	UsbDeviceIsoSend(Buf, Len*2);
	DeviceAudioUpDataR = (DeviceAudioUpDataR + Len) % DEVICE_AUDIO_UP_DATA_LEN;
}


// �ϲ�����ʵ��usb audio�ĳ�ʼ������Ҫ��Ϊ��ȥ��ԭdriver�е�ͨ������
static TIMER UsbAudioFreqAdjustTimer;

void UsbAudioFreqAdjust(void)
{
	uint16_t Samples;

	if(!IsTimeOut(&UsbAudioFreqAdjustTimer))
	{
		return;
	}
	TimeOutSet(&UsbAudioFreqAdjustTimer, 1000);

	Samples	= MixerGetRemainSamples(MIXER_SOURCE_USB);
	DacSampRateAdjust(TRUE, (Samples - 1024) * 5);
	APP_DBG("%d\n", Samples);
}
bool UsbAudioControl(void)
{
	uint16_t Msg = 0;
    
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif
    
#ifdef FUNC_USB_AUDIO_EN
	if(gSys.CurModuleID == MODULE_ID_USB_AUDIO)
	{
		APP_DBG("Enter usb-audio\n");
	}
#endif

#ifdef FUNC_USB_READER_EN
	if(gSys.CurModuleID == MODULE_ID_USB_READER)
	{
		APP_DBG("Enter sd reader\n");
	}
#endif

#ifdef FUNC_USB_AUDIO_READER_EN
	if(gSys.CurModuleID == MODULE_ID_USB_AUDIO_READER)
	{
		APP_DBG("Enter usb-audio-reader\n");
	}
#endif

	if(!IsPcLink())
	{
		APP_DBG("device not ready!\n");
		return FALSE;
	}

	TimeOutSet(&UsbAudioFreqAdjustTimer, 1000);    
	NVIC_EnableIRQ(USB_IRQn);
	UsbSetCurrentPort(PC_PORT_NUM);		// ѡ��USB�˿�

	APP_DBG("IsPcLink() OK!\n");

#ifdef FUNC_USB_AUDIO_EN
	if(gSys.CurModuleID == MODULE_ID_USB_AUDIO)
	{
		// ����USB����ģʽ��PCʶ���USB����
		APP_DBG("MODULE_ID_USB_AUDIO\n");
		DeviceAudioSetMode(USB_DEVICE_AUDIO);
		gDeviceVID = 0x0000;
		gDevicePID = 0x0201;
		gDeviceString_Manu = "MVSILICON";					//max length: 32bytes
		gDeviceString_Product = "MV USB AUDIO";				//max length: 32bytes
		gDeviceString_SerialNumber = "20130617A002";		//max length: 32bytes
	}
#endif

#ifdef FUNC_USB_READER_EN
	if(gSys.CurModuleID == MODULE_ID_USB_READER)
	{
		// ����USB������ģʽ��PCʶ���USB������
		APP_DBG("MODULE_ID_USB_READER\n");
		DeviceAudioSetMode(USB_DEVICE_READER);
		DeviceStorInit();
		gDeviceVID = 0x0000;
		gDevicePID = 0x1000;
		gDeviceString_Manu = "MVSILICON";				// max length: 32bytes
		gDeviceString_Product = "USB CARD READER";		// max length: 32bytes
		gDeviceString_SerialNumber = "20130617B002";	// max length: 32bytes
	}
#endif

#ifdef FUNC_USB_AUDIO_READER_EN
	if(gSys.CurModuleID == MODULE_ID_USB_AUDIO_READER)
	{
		// һ��ͨģʽ��PCͬʱʶ���USB������USB������
		APP_DBG("MODULE_ID_USB_AUDIO_READER\n");
		DeviceAudioSetMode(USB_DEVICE_AUDIO_READER);
		DeviceStorInit();
		gDeviceVID = 0x0000;
		gDevicePID = 0x0201;
		gDeviceString_Manu = "MVSILICON";				// max length: 32bytes
		gDeviceString_Product = "MV USB AUDIO";			// max length: 32bytes
		gDeviceString_SerialNumber = "20130617C002";    // max length: 32bytes
	}
#endif


	//USB������ģʽ�£��豸����������ʾ�Ĵ�������������
	gDeviceString_MassStor_Vendor = "MVSI";				// max length: 8bytes
	gDeviceString_MassStor_Product = "USB CARD READER";	// max length: 16bytes
	gDeviceString_MassStor_Ver = "V2.0";				// max length: 4bytes

	//USB����ģʽ�£��ص�����ָ���ʼ��
	gFpDeviceAudioMuteEn = DeviceAudioMuteEn;			// PC��ִ��MUTE����ʱ���ص��˺���
	gFpDeviceAudioMuteDis = DeviceAudioMuteDis;			// PC��ִ��UNMUTE����ʱ���ص��˺���
	gFpDeviceAudioSetVolume = DeviceAudioSetVolume;		// PC�˸ı�����ʱ���ص��˺���


	UsbDeviceEnableInt(DEVICE_CONTROL_EP, DeviceAudioOnCmd);
	UsbDeviceEnableInt(DEVICE_BULK_OUT_EP, DeviceAudioOnCmd);

//	CodecDacChannelSel(DAC_CH_DECD_L | DAC_CH_DECD_R); // select dac channel
	UsbDeviceInit();

//	NVIC_EnableIRQ(DECODER_IRQn);


	AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC : AUDIO_CH_NONE);	
	MixerConfigFormat(MIXER_SOURCE_USB, USB_DAC_FREQ, MIXER_FORMAT_STERO);
	mixer_pcm_format = MIXER_FORMAT_STERO;
	MixerConfigVolume(MIXER_SOURCE_USB, 4000, 4000);
	MixerUnmute(MIXER_SOURCE_USB);
	MixerEnable(MIXER_SOURCE_USB);

    AudioLeftVolume  = (AUDIO_MAX_VOLUME*3)/4;
    AudioRightVolume = (AUDIO_MAX_VOLUME*3)/4;
    APP_DBG("AudioLeftVolume = %d\n", AudioLeftVolume);
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
	if(gSys.AudioDRCEffectFlag)
	{
		if(DRC_ERROR_OK != init_drc(&gSys.AudioDRCEffectHandle, MIXER_FORMAT_STERO, USB_DAC_FREQ, AUDIO_EFFECT_PARAM_DRC_THRESHHOLD,
							0, AUDIO_EFFECT_PARAM_DRC_RATIO, AUDIO_EFFECT_PARAM_DRC_ATTACK_TIME, 
							AUDIO_EFFECT_PARAM_DRC_RELEASE_TIME, AUDIO_EFFECT_PARAM_DRC_CROSSOVER_FREQ))
        {
            APP_DBG("Init DRC Effect fail,close\n");
            gDRCEffectInited = FALSE;
        }
        else
        {
        	gDRCEffectInited = TRUE;
        }
	}
#endif	
#ifdef FUNC_AUDIO_3D_EFFECT_EN
	if(gSys.Audio3dEffectFlag)
	{
		if(THREE_D_ERROR_OK != init_3d(&gSys.Audio3dEffectHandle, MIXER_FORMAT_STERO, USB_DAC_FREQ, AUDIO_EFFECT_PARAM_3D_BASS, AUDIO_EFFECT_PARAM_3D_MID_BOOST))
        {
            //gSys.Audio3dEffectFlag = 0;
            APP_DBG("Init 3D Effect fail,close\n");
            g3dEffectInited = FALSE;
        }
        else
        {
        	g3dEffectInited = TRUE;
        }
	}
#endif
#ifdef FUNC_AUDIO_VB_EFFECT_EN
    if(gSys.AudioVbEffectFlag)
    {
        if(VB_ERROR_OK != init_vb(&gSys.AudioVbEffectHandle, MIXER_FORMAT_STERO, USB_DAC_FREQ, AUDIO_EFFECT_PARAM_VB_CUTOFF_FREQ, AUDIO_EFFECT_PARAM_VB_LIMITER_EN))
        {
            gVbEffectInited = FALSE;
        }
        else
        {
            gVbEffectInited = TRUE;
        }
    }
#endif 
    SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(Msg != MSG_COMMON_CLOSE)
	{
		//UsbDeviceDisableInt(DEVICE_BULK_OUT_EP);
		DeviceStorProcess();
		//UsbDeviceEnableInt(DEVICE_BULK_OUT_EP, DeviceAudioOnCmd);

		DeviceAudioProcess();
        
#if (!defined (OUTPUT_CHANNEL_CLASSD)) && (!defined (OUTPUT_CHANNEL_DAC_CLASSD))      //classdģʽ�£����õ�clk mode = normal mode,��ֹʹ��Ӳ��΢��
   #ifndef FUNC_SOFT_ADJUST_EN
        UsbAudioFreqAdjust();
   #endif
#endif        
		Msg = MsgRecv(10);// ��Ϣ���գ�����Ϣ����20ms������Ϣ��������

		if(Msg == MSG_USB_DEVICE_INTERRUPT_CB)
		{
			UsbIntFlag = FALSE;
		}
                
		switch(Msg)
		{		
			case MSG_POWER:
				gSys.NextModuleID = MODULE_ID_STANDBY;
			case MSG_MODE: // mode change,end of this mode
				APP_DBG("CHANGE_MODE\n");
				// mode change,end of this mode
				Msg = MSG_COMMON_CLOSE;                    
				break;

			case MSG_PLAY_PAUSE:
				APP_DBG("PLAY_PAUSE\n");
				DeviceAudioSendPcCmd(PC_CMD_PLAY_PAUSE);
				break;

			case MSG_PRE:
				APP_DBG("PRE\n");
				DeviceAudioSendPcCmd(PC_CMD_PRE);
				SoundRemind(SOUND_PREV_TRACK);
				break;

			case MSG_NEXT:
				APP_DBG("NEXT\n");
				DeviceAudioSendPcCmd(PC_CMD_NEXT);				
                SoundRemind(SOUND_NEXT_TRACK);
				break;

			case MSG_STOP:
				APP_DBG("STOP\n");
				DeviceAudioSendPcCmd(PC_CMD_STOP);
				break;

			case MSG_MUTE:
				APP_DBG("MUTE\n");
				DeviceAudioSendPcCmd(PC_CMD_MUTE);
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
				CommonMsgProccess(Msg);
				break;
		}
	}

#ifdef FUNC_USB_AUDIO_EN
	if(gSys.CurModuleID == MODULE_ID_USB_AUDIO)
	{
		APP_DBG("Leave usb-audio\n");
	}
#endif

#ifdef FUNC_USB_READER_EN
	if(gSys.CurModuleID == MODULE_ID_USB_READER)
	{
		APP_DBG("Leave sd reader\n");
	}
#endif

#ifdef FUNC_USB_AUDIO_READER_EN
	if(gSys.CurModuleID == MODULE_ID_USB_AUDIO_READER)
	{
		APP_DBG("Leave usb-audio-reader\n");
	}
#endif

	MixerMute(MIXER_SOURCE_USB);
	MixerDisable(MIXER_SOURCE_USB);

	UsbDeviceDisConnect(); // �˳�ǰ�Ͽ�usb device��ֹͣ�������ݣ�������SD/U�̲��Ų�����ͻ
#ifdef FUNC_USB_EN
	UsbSetCurrentPort(UDISK_PORT_NUM); // set usb host port
#endif
	return TRUE;//OK
}

#endif



