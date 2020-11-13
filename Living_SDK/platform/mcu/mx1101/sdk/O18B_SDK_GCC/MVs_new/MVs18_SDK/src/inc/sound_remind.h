///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: sound_remind.h
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#ifndef __SOUND_REMIND_H__
#define __SOUND_REMIND_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

typedef enum _SOUND_ID
{
	SOUND_NUM0,						//0
	SOUND_NUM1,						//1    
	SOUND_NUM2,						//2   
	SOUND_NUM3,						//3    
	SOUND_NUM4,						//4  
	SOUND_NUM5,						//5   
	SOUND_NUM6,						//6 
	SOUND_NUM7,						//7  
	SOUND_NUM8,						//8    
	SOUND_NUM9,						//9   
	SOUND_PLUS5,					//��5
	SOUND_SPEECH_OP,				//������
	SOUND_SPEECH_CL,				//�����ر�	
	SOUND_SPEECH_CL_FORCE,			//����ǿ�ƹر�
	SOUND_NEXT_FOLD,				//��һĿ¼
	SOUND_REP_1,					//����ѭ��
	SOUND_REP_ALL,					//ȫ��ѭ��
	SOUND_START_REC,				//��ʼ¼��
	SOUND_SAVE_REC,					//¼������
	SOUND_REC_PLAYBACK,				//¼������
	SOUND_DELETE_REC,				//ɾ��¼��
	SOUND_LINEIN,					//��Ƶ����
	SOUND_UDISC_PLAYBACK,			//u�̲���
	SOUND_CARD_PLAYBACK,			//������
	SOUND_RADIO_MODE,				//����ģʽ
	SOUND_PREV_TRACK,				//��һ��
	SOUND_NEXT_TRACK,				//��һ��
	SOUND_PAUSE,					//��ͣ
	SOUND_STOP,						//ֹͣ
	SOUND_ECHO_REGULATE,			//�������
	SOUND_ECHO_PLUS,				//�����
	SOUND_ECHO_REDUCE,				//�����
	SOUND_DELAY_REGULATE,			//��ʱ����
	SOUND_DELAY_PLUS,				//��ʱ��
	SOUND_DELAY_REDUCE,				//��ʱ��
	SOUND_TREB_REGULATE,			//��������
	SOUND_TREB_PLUS,				//������
	SOUND_TREB_REDUCE,				//������
	SOUND_BASS_REGULATE,			//��������
	SOUND_BASS_PLUS,				//������
	SOUND_BASS_REDUCE,				//������
	SOUND_MIC_OPEN,					//��Ͳ��
	SOUND_MIC_CLOSE,				//��Ͳ�ر�
	SOUND_EQ_FLAT,					//��Ȼ
	SOUND_EQ_ROCK,					//ҡ��
	SOUND_EQ_POP,					//����
	SOUND_EQ_CLASSIC,				//�ŵ�
	SOUND_EQ_SOFT,					//���
	SOUND_EQ_JAZZ,					//��ʿ
	SOUND_SUBBASS,					//�ص���
	SOUND_MENU,						//�˵�
	SOUND_BT_MODE,					//����ģʽ
	SOUND_BT_CONNECT,				//��������
	SOUND_BT_DISCONNECT,			//�����Ͽ�
	SOUND_POINT,					//��
	SOUND_LANG_MODE,				//����ģʽ/Ӣ��ģʽ
	SOUND_USB_AUDIO_MODE,			//����ģʽ
	SOUND_USB_AUDIO_READER_MODE,	//һ��ͨģʽ
	SOUND_PLAY,	            		//����
	SOUND_FF,	            		//���
	SOUND_FB,	            		//����
	SOUND_MHZ,	            		//�׺���
	SOUND_REP_FOLD,	        		//Ŀ¼ѭ��
	SOUND_INTRO,	            	//�������
	SOUND_RANDOM,	        		//�������
	SOUND_AUTO_SCAN,	        	//�Զ���̨
	SOUND_PRE_STATION,	    		//��һ��̨
	SOUND_NXT_STATION,	    		//��һ��̨
	SOUND_PWR_OFF,	        		//�ѹػ�
	SOUND_GUITAR_VOL,	    		//��������
	SOUND_SEARCHING,	        	//������
	SOUND_MUTE_ON,	        		//������
	SOUND_MUTE_OFF,	        		//�����ر�
	SOUND_MIC_VOL,	        		//��Ͳ����
	SOUND_MAIN_VOL,	        		//������
	SOUND_MIC_MODE_OPEN,	    	//��Ͳģʽ��
	SOUND_MIC_MODE_CLOSE,			//��Ͳģʽ��
	SOUND_MIC_FIRST_OPEN,			//MIC���ȿ�
	SOUND_MIC_FIRST_CLOSE,			//MIC���ȹ�
	SOUND_OPEN,	            		//��
	SOUND_CLOSE,	            	//�ر�
	SOUND_BAT_LOW_PWR,	    		//��ص����ͣ�����
	SOUND_CHARGING,	        		//���ڳ��
	SOUND_BAT_FULL,	        		//�����ѳ���
	SOUND_WAIT_PAIR,	        	//�ȴ�����豸
	SOUND_SHI,	            		//ʮ
	SOUND_BAI,	            		//��
	SOUND_MIC_INSERT,	    		//��Ͳ����
	SOUND_MIC_PULLOUT,	    		//��Ͳ�γ�
	SOUND_50MS,	            		//50
	SOUND_100MS,	            	//100
	SOUND_150MS,	            	//150
	SOUND_200MS,	            	//200
	
	SOUND_BT_STATE_READY_RING,
	SOUND_BT_STATE_UNREADY_RING,

} SOUND_ID;


//��ʾ��ģ���ʼ��
void SoundRemindInit(void);

//�����ʾ�����ݻ���������decoder task����ʱ����
//return -1 -- stream over; 0 -- no fill; other value -- filled bytes
int32_t SoundRemindFillStream(void);

#ifdef FUNC_SOUND_REMIND
//����������ʾ��
//ע�⣺�ú���������main task�е��ã����������������
void SoundRemind(uint16_t SoundId);
#else
#define SoundRemind(SoundId)	//�������ʾ�����ܣ��򽫸ú�������Ϊ�գ�����ֹ�������
#endif

// �Ƿ��ڲ�����ʾ��
bool IsSoundRemindPlaying(void);

// ֹͣ������ʾ��(������ʾ��ֹͣ��־)
void StopSoundRemind(void);

// ������ʾ��ֹͣ��־
void ResetSoundRemindStopFlag(void);

uint32_t SoundRemindFillStreamCallback(void *buffer, uint32_t length);

extern bool InterruptSoundRemind;

extern bool gVbEffectInited;

#ifdef __cplusplus
}
#endif//__cplusplus

#endif





