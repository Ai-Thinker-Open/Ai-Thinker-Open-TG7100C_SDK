/******************************************************************************
 * @file    mixer.h
 * @author  Orson
 * @version V1.0.0
 * @date    29-April-2014
 * @brief   audio mixer
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __MIXER_H__
#define __MIXER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define MIXER_FORMAT_MONO			1	//��������Դ��ÿ��������2���ֽ�
#define MIXER_FORMAT_STERO			2	//˫������Դ��ÿ��������4���ֽ�


//Mixerģ���ʼ��
//PcmFifoAddr: ������PMEM��
void MixerInit(void* PcmFifoAddr, uint16_t PcmFifoSize);
	
//����Mixerģ���ڲ��Ƿ���������ת����ֻ������ͨ��0��Ч
//SrcEnable = FALSE�����������PCM������������ת��
//SrcEnable = TRUE�� Mixerģ���ڲ��������PCM���ݲ�����ͳһת��Ϊ44.1KHZ
void MixerSrcEnable(bool SrcEnable);
	
//ʹ��ĳ������ͨ��
void MixerEnable(uint8_t SourceID);

//��ֹĳ������ͨ��
void MixerDisable(uint8_t SourceID);

//��̬�ı�ĳ��ͨ��������
//SourceID:    0--3
//LeftVolume:  0--8191, 4095: 0dB, 8191: +6dB
//RightVolume: 0--8191, 4095: 0dB, 8191: +6dB
void MixerConfigVolume(uint8_t SourceID, uint16_t LeftVolume, uint16_t RightVolume);

//MUTEĳ������ͨ��
void MixerMute(uint8_t SourceID);

//UNMUTEĳ������ͨ��
void MixerUnmute(uint8_t SourceID);

//��̬�ı�ĳ��ͨ���Ĳ��������ʽ
//SourceID:    0--3
//SampleRate:  �����ʣ�����Դ0֧��9�ֲ����ʣ���������Դֻ֧��44.1KHZ������
//PcmFormat:   1--��������2--˫����
void MixerConfigFormat(uint8_t SourceID, uint16_t SampleRate, uint8_t PcmFormat);

//���ĳͨ���Ƿ���������µ�����
bool MixerIsDone(uint8_t SourceID);

//SourceID:    0--3
//PcmBuf:      PCM�������׵�ַ
//SampleCnt:   ��������
void MixerSetData(uint8_t SourceID, void* PcmBuf, uint16_t SampleCnt);

//��ȡĳ����Դ��Mixer��ʣ��Ĳ�������������Ϊ�ڸ�����Դ�������µĵ���
//SourceID:    0--3
uint16_t MixerGetRemainSamples(uint8_t SourceID);

//����fadein/fadeout�ٶ�
//SourceID:    0--3
//FadeinTime:  0--2000��������0���䵽4095��ʱ�䣬��λ: ms
//FadeoutTime: 0--2000��������4095���䵽0��ʱ�䣬��λ: ms
void MixerSetFadeSpeed(uint8_t SourceID, uint16_t FadeinTime, uint16_t FadeoutTime);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
