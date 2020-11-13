///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: task_decoder.h
///////////////////////////////////////////////////////////////////////////////
/*
zhouyi,2012.07.11, initial version
maintainer: Aissen
*/
#ifndef __TASK_DECODER_H__
#define __TASK_DECODER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"
#include "audio_decoder.h"
#include "folder.h"

//��ʼ����
//FileHandle: �ļ�ָ��
//FileType:   IO_TYPE_FILE--����U��/SD���е��ļ���IO_TYPE_MEMORY--������ʾ��
//StartTime:  ���ŵĿ�ʼʱ�� 
bool DecoderTaskPlayStart(void* FileHandle, uint8_t FileType, uint32_t StartTime);
void DecoderTaskStop(void);				//ֹͣ����
void DecoderTaskPause(void);			//��ͣ
void DecoderTaskResume(void);			//����ͣ�ָ�����
void DecoderTaskFF(uint32_t StepTime);	//���
void DecoderTaskFB(uint32_t StepTime);	//����
void DecoderTaskSeek(uint32_t Time);	//ָ��ʱ�䲥��
bool DecoderTaskIsSongEnd(void);		//�ж�һ�׸��Ƿ񲥷Ž���
bool DecoderTaskIsSongBegin(void);		//�ж�һ�׸��Ƿ���˵�������ͷ
bool DecoderTaskIsInitializeOk(void);	//�ж�һ�׸��ʼ���Ƿ�ɹ�
uint32_t DecoderTaskGetPlayTime(void);	//��ȡ��ǰ����ʱ��

//����������ʾ����������ָ�ԭ����״̬
bool DecoderTaskSyncPlay(void* FileHandle, uint8_t FileType);
extern void DecoderTaskEntrance(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*__TASK_DECODER_H__*/
