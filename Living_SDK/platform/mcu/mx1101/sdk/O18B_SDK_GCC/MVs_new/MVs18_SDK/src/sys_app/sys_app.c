/**
 *******************************************************************************
 * @file    sys_app.c
 * @author  Richard
 * @version V1.0.0
 * @date    17-12-2013
 * @brief   system application
 * @maintainer: Sam
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#include "type.h"
#include "app_config.h"
#include "msgq.h"
#include "app_msg.h"
#include "uart.h"
#include "cache.h"
#include "gpio.h"
#include "wakeup.h"
#include "clk.h"
#include "timer.h"
#include "sys_app.h"
#include "ir.h"
#include "nvm.h"
#include "sound_remind.h"
#include "breakpoint.h"
#include "fsinfo.h"


SYS_INFO gSys;


//ȫ�ֱ����ĳ�ʼ��������ʱ����
void SysVarInit(void)
{
	memset(&gSys, 0, sizeof(gSys));

#ifdef FUNC_RADIO_DIV_CLK_EN
	//system clock frequency divide into 4
	gSys.FmSysClkDivFactor = 2;
	gSys.IsSysClk96MHz = 1;
#endif
		
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
	gSys.AudioDRCEffectFlag = 0;
#endif	
	
#ifdef FUNC_AUDIO_3D_EFFECT_EN
	gSys.Audio3dEffectFlag = 0;
#endif
    
#ifdef FUNC_AUDIO_VB_EFFECT_EN
    gSys.AudioVbEffectFlag = 0;
#endif	
	
#ifdef FUNC_TREB_BASS_EN
	gSys.BassVal = DEFAULT_BASS_VAL;
	gSys.TrebVal = DEFAULT_TREB_VAL;
	gSys.EqStatus = 1; // set default effect to EQ
#endif
	
	gSys.AudioSetMode = AUDIO_SET_MODE_MAIN_VOL;
	gSys.Volume = DEFAULT_VOLUME;
#ifdef FUNC_MIC_EN
	gSys.MicVolume = DEFAULT_MICIN_VOLUME;
#endif

#ifdef FUNC_BT_HF_EN
	gSys.HfVolume = MAX_BTHF_VOLUME;
#endif

#ifdef FUNC_SOUND_REMIND
	gSys.SoundRemindOn = TRUE;
	gSys.LanguageMode = LANG_ZH;
#endif


	// װ�ضϵ���Ϣ��������Ϣ
#ifdef FUNC_BREAKPOINT_EN
{
	BP_SYS_INFO *pBpSysInfo;
	pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
	gSys.CurModuleID = BP_GET_ELEMENT(pBpSysInfo->CurModuleId);
	gSys.DiskType = BP_GET_ELEMENT(pBpSysInfo->DiskType);
	gSys.Volume = BP_GET_ELEMENT(pBpSysInfo->Volume);
    gSys.Eq = BP_GET_ELEMENT(pBpSysInfo->Eq);
	if(gSys.Volume > MAX_VOLUME)
	{
		gSys.Volume = DEFAULT_VOLUME;
	}
#ifdef FUNC_BT_HF_EN
	gSys.HfVolume = BP_GET_ELEMENT(pBpSysInfo->HfVolume);    
#endif

#ifdef FUNC_SOUND_REMIND
	gSys.SoundRemindOn = BP_GET_ELEMENT(pBpSysInfo->SoundRemindOn);    
	if(gSys.SoundRemindOn > 1)
	{
		gSys.SoundRemindOn = TRUE;
	}
        
	gSys.LanguageMode = BP_GET_ELEMENT(pBpSysInfo->LanguageMode);    
	if(gSys.LanguageMode >= LANG_SUM)
	{
		gSys.LanguageMode = LANG_ZH;
	}
#endif	
}
#endif	
}

//ȫ�ֱ����ı��棬�ػ�ʱ����
void SysVarDeinit(void)
{
#ifdef FUNC_BREAKPOINT_EN
	BP_SaveInfo(BP_SAVE2NVM_FLASH);
#endif	
}

//ϵͳ�����жϣ���������㿪���������򱾺����м�������˯��ģʽ
void SystemOn(void)
{
	APP_DBG("*******SystemOn************\n");

	//��ô�SLEEP״̬���ѵĴ���Դ��־
	APP_DBG(">>WakeUpOp(), gWakeupTrigFlg:%x\n", gWakeUpFlag);
#ifdef FUNC_IR_KEYPAD_EN
	//ϵͳ������IR���ѹ��ܣ�IR����ͷ������GPIOC2��IR�źŻ���ϵͳ�����жϼ�ֵ
	if(gWakeUpFlag == WAKEUP_FLAG_GPIOC2)
	{
		TIMER	WaitTimer;
		uint32_t IrKeyData;

		TimeOutSet(&WaitTimer, 3500);
		//����ڹ涨ʱ����δ����Ԥ����IR��ֵ�������˯��
		while(!IsTimeOut(&WaitTimer))
		{
			IrKeyData = IrGetKeyCode();
			APP_DBG("IrKeyData = : %X\n", IrKeyData);

			//�����⵽Ԥ����IR��ֵ����ʼ����ϵͳ
			if(IrKeyData == 0xB847FF00)  // ��ͬң������Ҫ���ĸ�ֵ
			{
				APP_DBG("WAKE UP FROM SLEEP BY IR!\n");
				return;
			}
			else
			{
				APP_DBG("ERROR IR KEY!\n");
				break;
			}
		}

		APP_DBG("TIMEOUT!\n");
		if(IR_KEY_PORT == IR_USE_GPIOC2)
		{
			APP_DBG("*GPIO_C_IN:%x\n", GpioGetReg(GPIO_C_IN));

			APP_DBG("StandBy Mode\n");
			SysVarDeinit();
			SysSetWakeUpSrcInDeepSleep(WAKEUP_SRC_SLEEP_C2, WAKEUP_POLAR_C2_LOW, WAKEUP_TMODE_1MS);
			while((GpioGetReg(GPIO_C_IN) & GPIOC2) != GPIOC2)
			{
				//APP_DBG("��ȷ������Sleep״̬ǰWakeUp������������(GPIO_C_IN:%bx(GPIOC3 shoule be 0))\n", GetGpioReg(GPIO_B_IN));
				WaitMs(10);
			}
		}
//		SysGotoDeepSleepCfg();
		ClkSwitchDpllToRc();
		ClkDpllClose(); //close dpll

		SysGotoDeepSleep(); //power down, �����غ�Ӳ������Ч��bypassģʽ��Ч��
		OSRescheduleTimeout(0xFFFFFFFF);
		APP_DBG("<<WakeUpOp()\n");
		return;
	}
#endif
}


void AudioSysInfoSetBreakPoint(void)
{
#ifdef FUNC_BREAKPOINT_EN
	BP_SYS_INFO *pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);

	pBpSysInfo->VaildFlag = BP_SIGNATURE;
	pBpSysInfo->CurModuleId = gSys.CurModuleID;
	pBpSysInfo->Volume = gSys.Volume;
    pBpSysInfo->Eq = gSys.Eq;
#ifdef FUNC_SOUND_REMIND
	pBpSysInfo->SoundRemindOn = gSys.SoundRemindOn;
	pBpSysInfo->LanguageMode = gSys.LanguageMode;
#endif
	BP_SaveInfo(BP_SAVE2NVM);

#ifdef BP_SAVE_TO_FLASH // ������� test
	BP_SaveInfo(BP_SAVE2FLASH); //Save BPInfoToFlash();
#endif
	APP_DBG("AudioSysInfoSetBreakPoint\n");
	APP_DBG("*  Signature:%X\n", pBpSysInfo->VaildFlag);
	APP_DBG("*  Cur Mode:%X\n", pBpSysInfo->CurModuleId);
	APP_DBG("*  DiskType:%X\n", pBpSysInfo->DiskType);
	APP_DBG("*\n");
	APP_DBG("**********************************\n");
#endif
}

#if 0
void AudioLoadInfoFromFlash(void)
{
#ifdef FUNC_BREAKPOINT_EN
	BP_SYS_INFO NvmSysInfo;

//#ifdef BP_SAVE_TO_FLASH // ������� test
	//Load BPInfoFromFlash();//LoadNvmFromFlash();
	//Save BPInfoToFlash();//SaveNvmToFlash();
//#endif

	//BP_LoadSysInfo(&NvmSysInfo);
	APP_DBG("AudioLoadInfoFromFlash\n");
	APP_DBG("*  Signature:%X\n", NvmSysInfo.VaildFlag);
	APP_DBG("*  Cur Mode:%X\n", NvmSysInfo.CurModuleId);
	APP_DBG("*  DiskType:%X\n", NvmSysInfo.DiskType);
	APP_DBG("*\n");
	APP_DBG("**********************************\n");
#endif

}
#endif

//Flash ����
//����: ��
//����ֵ: �����ɹ�����TRUE�����򷵻�FALSE
bool FlashUnlock(void)
{
	char cmd[3] = "\x35\xBA\x69";
	
	// unlock flash
	if(SpiFlashIOCtl(IOCTL_FLASH_UNPROTECT, cmd, sizeof(cmd)) != FLASH_NONE_ERR)
	{
		return FALSE;
	}

	return TRUE;
	
}


//Flash����
//����:lock_range ��Flash������Χ: 
//		 FLASH_LOCK_RANGE_HALF : 			����1/2 Flash �ռ�(��0��ʼ����ͬ)
//		FLASH_LOCK_RANGE_THREE_QUARTERS: 	����3/4 Flash �ռ�
//		FLASH_LOCK_RANGE_SEVENTH_EIGHT:	����7/8 Flash �ռ�
//		FLASH_LOCK_RANGE_ALL:				����ȫ��Flash �ռ�
//ע��: ��Ҫ���ݶ����USER BANK �ĵ�ַ�����������ķ�Χ��������ܽ�USER bank�ռ�������޷�д����
//����ֵ: �����ɹ�����TRUE�����򷵻�FALSE
bool FlashLock(SPI_FLASH_LOCK_RANGE lock_range)
{
	if(SpiFlashIOCtl(IOCTL_FLASH_PROTECT, lock_range) != FLASH_NONE_ERR)
	{
		return FALSE;
	}

	return TRUE;
}

//����Ϣ�����н�����Ϣ
//����Ϣ����������Ϣֵ
//����Ϣ�ȴ�WaitTime����
uint16_t MsgRecv(uint16_t WaitTime)
{
	extern bool SysDeviceMsgCtrl(uint16_t Msg);
	extern int32_t MainTaskHandle;	//MAIN TASK��ID��
	extern int32_t* hcurrent;		//��ǰTASK��ID��
	
	uint16_t Msg = (uint16_t)OSQueueMsgRecv(NULL, NULL, WaitTime);

	if(*hcurrent == MainTaskHandle)
	{
		if(SysDeviceMsgCtrl(Msg))
		{
			return MSG_NONE;
		}
	}
	
	return Msg;
}


//����Ϣ�����з���ָ����Ϣ
void MsgSend(uint16_t Msg)
{
#ifdef FUNC_SOUND_REMIND
	if(Msg != MSG_USB_DEVICE_INTERRUPT_CB)
	{
		if(IsSoundRemindPlaying())
		{
			StopSoundRemind();
		}
	}
#endif
	if(Msg == MSG_SD_PLUGOUT
		|| Msg == MSG_USB_PLUGOUT
		|| Msg == MSG_PC_PLUGOUT
		|| Msg == MSG_LINEIN_PLUGOUT
		|| Msg == MSG_RTC_STOP_ALARM_REMIND
		|| Msg == MSG_MIC_PLUGOUT)
		OSQueueMsgSend(Msg, NULL, 0, MSGPRIO_LEVEL_HI, 0);
	else
		OSQueueMsgSend(Msg, NULL, 0, MSGPRIO_LEVEL_MD, 0);
}

bool MsgCheck(uint16_t Msg)
{
	int32_t		ret = 0;
	ret = OSQueueMsgIOCtl(MSGQ_IOCTL_PEEK_TASKMSG, Msg);
	return (ret == 0 ? FALSE:TRUE);
}

//����Ϣ������ɾ��ָ����Ϣ
void MsgClear(uint16_t Msg)
{
	OSQueueMsgIOCtl(MSGQ_IOCTL_DEL_SPECMSG, Msg);
}


//����Ϣ������ɾ��ָ����Ϣ����������Ϣ
void MsgClearClass(uint16_t MsgClass)
{
	uint32_t i;

	for(i = 0; i <= 0xFF; i++)
	{
		MsgClear((MsgClass & 0xFFFFFF00) + i);
	}
}

///////////// mode switch state ///////////
static MODE_SWITCH_STATE	gModeSwitchState = MODE_SWITCH_STATE_DONE;
void SetModeSwitchState(MODE_SWITCH_STATE state)
{
	gModeSwitchState = state;
}

MODE_SWITCH_STATE GetModeSwitchState(void)
{
	return gModeSwitchState;
}


