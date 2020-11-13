/**
 *****************************************************************************
 * @file     device_audio.h
 * @author   Orson
 * @version  V1.0.0
 * @date     24-June-2013
 * @brief    device audio module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __DEVICE_AUDIO_H__
#define	__DEVICE_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#define	USB_DAC_FREQ				44100			//USB����SPEAKERĬ�ϲ�����
#define	USB_MIC_FREQ				44100			//USB����MICĬ�ϲ�����

//Ӧ�ò����USB����ģʽ��ʼ��ʱ�޸���������ȫ�ֱ���
//�ͻ��ڿ�������ʱ��ע�⣺
//�������44KHZ���������ӵ��ԣ�Ȼ���ٴ���48KHZ(������)����������ͬһ̨���ԣ����Ի��޷����ų�������
//���������������������ֽ���취��
//1. �����ڵ����豸���������ֹ�ж�ظ�USB�豸��Ȼ�����²�����豸��
//2. ���Խ������е�gDevicePID�޸�Ϊ����ֵ��
extern uint32_t gDeviceSpeakerSampleRate;			//USB����SPEAKER������
extern uint32_t gDeviceMicSampleRate;				//USB����MIC������


extern uint16_t  gDeviceVID;
extern uint16_t  gDevicePID;
extern uint8_t* gDeviceString_Manu;					//max length: 32bytes
extern uint8_t* gDeviceString_Product;				//max length: 32bytes
extern uint8_t* gDeviceString_SerialNumber;			//max length: 32bytes

//USB��������
extern uint16_t AudioLeftVolume;
extern uint16_t AudioRightVolume;

extern FPCALLBACK gFpDeviceAudioMuteEn;
extern FPCALLBACK gFpDeviceAudioMuteDis;
extern FPCALLBACK gFpDeviceAudioSetVolume;


//����Mic�������ƽӿ�
extern uint16_t AudioMicLeftVolume;
extern uint16_t AudioMicRightVolume;
extern bool     AudioMicMuteFlag;

extern FPCALLBACK gFpDeviceAudioMicMuteEn;
extern FPCALLBACK gFpDeviceAudioMicMuteDis;
extern FPCALLBACK gFpDeviceAudioMicSetVolume;

//pc volume: 0 --- 999
#define AUDIO_MAX_VOLUME	999


//pc command
#define PC_CMD_NEXT			0x04
#define PC_CMD_PRE			0x08
#define PC_CMD_MUTE			0x10
#define PC_CMD_PLAY_PAUSE	0x40
#define PC_CMD_STOP			0x80


/**
 * @brief  USB����ģʽ��ʼ��
 * @param  NONE
 * @return NONE
 */
void DeviceAudioInit(void);

/**
 * @brief  USB����ģʽ�£��ײ������������
 * @param  NONE
 * @return NONE
 */
void DeviceAudioProcess(void);

/**
 * @brief  USB����ģʽ�£����ͷ����������
 * @param  Cmd �����������
 * @return 1-�ɹ���0-ʧ��
 */
bool DeviceAudioSendPcCmd(uint8_t Cmd);

//����USB�豸����ģʽ
#define USB_DEVICE_AUDIO			1	//PCʶ��Ϊ����������
#define USB_DEVICE_READER			2	//PCʶ��Ϊ�����Ķ�����
#define USB_DEVICE_AUDIO_READER		3	//PCͬʱʶ��������Ͷ�����
/**
 * @brief  ����USB�豸����ģʽ
 * @param  DeviceMode ����ģʽ
 * @param            USB_DEVICE_AUDIO: PCʶ��Ϊ����������
 * @param            USB_DEVICE_READER: PCʶ��Ϊ�����Ķ�����
 * @param            USB_DEVICE_AUDIO_READER: PCͬʱʶ��������Ͷ�����
 * @return NONE
 */
void DeviceAudioSetMode(uint8_t DeviceMode);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
