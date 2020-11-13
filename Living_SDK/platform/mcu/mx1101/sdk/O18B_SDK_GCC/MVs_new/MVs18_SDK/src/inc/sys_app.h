/**
 *******************************************************************************
 * @file    sys_app.h
 * @author  Richard
 * @version V1.0.0
 * @date    17-12-2013
 * @brief   system application function declaration
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

#ifndef __SYS_APP_H__
#define __SYS_APP_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "spi_flash.h"

//��������
typedef enum _LANG
{
    LANG_ZH, // ���ı�ʶ
    LANG_EN, // Ӣ�ı�ʶ
	
    LANG_SUM,
} LANG;


//�˵����ͣ�ѡ�������ӡ������������Ĺ���
typedef enum _AUDIO_SET_MODE
{
	AUDIO_SET_MODE_MAIN_VOL,	//����������
	AUDIO_SET_MODE_MIC_VOL,		//MIC��������
	AUDIO_SET_MODE_ECHO_DEPTH,	//MIC�������
	AUDIO_SET_MODE_ECHO_DELAY,	//MIC��ʱ����
	AUDIO_SET_MODE_TREB,		//��������
	AUDIO_SET_MODE_BASS,		//��������
	
	AUDIO_SET_MODE_SUM			//ģʽ����
} AUDIO_SET_MODE;

typedef enum _MODE_SWITCH_STATE{
	MODE_SWITCH_STATE_START = 0,
	MODE_SWITCH_STATE_DONE
} MODE_SWITCH_STATE;

//#pragma pack(1)
typedef struct _SYS_INFO_
{
	uint8_t CurModuleID;	//current running module id
	uint8_t ModuleIDBackUp;
	uint8_t NextModuleID;	//next module id

	// ��ǰ������������(����ģʽ��¼�����ļ���صĶ����õ�)
	uint8_t DiskType;		// USB_TYPE - U�̣�SD_TYPE - SD��

    bool MicEnable;
	uint8_t MicVolume;
	
	uint8_t Volume;
	uint8_t MuteFlag;
	uint8_t Eq;
#ifdef FUNC_TREB_BASS_EN
	uint8_t TrebVal;
	uint8_t BassVal;
	uint8_t EqStatus;		// set effect to eq or treb/bass. 0: treb/bass, 1:eq
#endif
	
	uint8_t FmSysClkDivFactor;	//system clock divide frequency factor in FM mode
	uint8_t IsSysClk96MHz;
	uint8_t AudioSetMode;
    uint8_t RecordState;
    uint8_t RecordSource;
	bool SoundRemindOn;
    uint8_t LanguageMode;

#ifdef FUNC_BT_HF_EN
	uint8_t HfVolume;
#endif
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
    uint8_t AudioDRCEffectFlag;
    DRCContext AudioDRCEffectHandle;
#endif
#ifdef FUNC_AUDIO_3D_EFFECT_EN
    uint8_t Audio3dEffectFlag;
    ThreeDContext Audio3dEffectHandle;
#endif

#ifdef FUNC_AUDIO_VB_EFFECT_EN
    uint8_t AudioVbEffectFlag;
    VBContext AudioVbEffectHandle;

#endif

} SYS_INFO;

extern SYS_INFO gSys;

void SysVarInit(void);		//ȫ�ֱ����ĳ�ʼ��������ʱ����
void SysVarDeinit(void);	//ȫ�ֱ����ı��棬�ػ�ʱ����

void SystemOn(void);		//ϵͳ�����жϣ���������㿪���������򱾺����м�������˯��ģʽ
void AudioSysInfoSetBreakPoint(void);
//void AudioLoad InfoFromFlash(void);
void AlarmRemindOnOff(void);

//Flash ����
//����: ��
//����ֵ: �����ɹ�����TRUE�����򷵻�FALSE
bool FlashUnlock(void);

//Flash����
//����:lock_range ��Flash������Χ: 
//		 FLASH_LOCK_RANGE_HALF : 			����1/2 Flash �ռ�(��0��ʼ����ͬ)
//		FLASH_LOCK_RANGE_THREE_QUARTERS: 	����3/4 Flash �ռ�
//		FLASH_LOCK_RANGE_SEVENTH_EIGHT:	����7/8 Flash �ռ�
//		FLASH_LOCK_RANGE_ALL:				����ȫ��Flash �ռ�
//ע���û���Ҫ����USER BANK�ĵ�ַ�����������ķ�Χ��������ܽ�USER bank�ռ�������޷�д����
//����ֵ: �����ɹ�����TRUE�����򷵻�FALSE
bool FlashLock(SPI_FLASH_LOCK_RANGE lock_range);


//����Ϣ�����н�����Ϣ
//����Ϣ����������Ϣֵ
//����Ϣ�ȴ�WaitTime����
uint16_t MsgRecv(uint16_t WaitTime);

//����Ϣ�����з���ָ����Ϣ
void MsgSend(uint16_t Msg);

// �����Ϣ�����Ƿ���ָ����Ϣ
bool MsgCheck(uint16_t Msg);

//����Ϣ������ɾ��ָ����Ϣ
void MsgClear(uint16_t Msg);

//����Ϣ������ɾ��ָ����Ϣ����������Ϣ
void MsgClearClass(uint16_t MsgClass);

//////////// mode switch state /////////
void SetModeSwitchState(MODE_SWITCH_STATE state);
MODE_SWITCH_STATE GetModeSwitchState(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

