///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: lrc.h
//  maintainer: Sam
///////////////////////////////////////////////////////////////////////////////

#ifndef __LRC_H__
#define __LRC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#pragma pack(1)

typedef enum _TEXT_ENCODE_TYPE
{
    ENCODE_UNKNOWN,
    ENCODE_ANSI,
    ENCODE_UTF8,
    ENCODE_UTF16, // unicode
    ENCODE_UTF16_BIG,
    ENCODE_GBK,
} TEXT_ENCODE_TYPE;

typedef struct _LRC_ROW
{
	int32_t StartTime; // ��ʼʱ���, ms
	uint32_t Duration;  // ��ǰ����г���ʱ��, ms
	uint8_t* LrcText;   // ���ָ�룬���ⲿ�����ڴ�
	int16_t MaxLrcLen; // ���Buffer���ڴ��С
} LRC_ROW;

#pragma pack()

/**
* @brief                   ���ļ�ȫ·����ʼ��Lrc Parser, ��ע�⡿��֧��ͬʱ�򿪶��Lrc Parser
* @param input CurFolder   ��ǰ�ļ��о��
* @param input FileName    ����ļ�ȫ·��
* @param input FileNameLen ����ļ�������
* @param input ReadBuffer  Lrc Parser�Ĺ����ڴ�ռ䣬���û��ⲿָ���ڴ�ռ䣬
*                          �������ΪNULL��Ĭ����Parser�ڲ�����
* @param input ReadBufSize Lrc Parser�����ռ�Ĵ�С����������ռ����û�ָ������ReadBuffer != NULL����
*                          ���ֵ���� >= 128����������ռ����ڲ����䣬��ֵ���˿����� >= 128��ֵ�⣬
*                          ����������Ϊ0����ʾLrc Parser�Լ�Ĭ�Ϸ���512Bytes�Ĺ����ռ�
* @return Success - TRUE, Fail - FALSE
*/
bool OpenLrcByFileName(FOLDER* CurFolder, uint8_t* FileName, uint32_t FileNameLen,
                           uint8_t* ReadBuffer, uint32_t ReadBufSize);

/**
* @brief                   ���ļ�ID�ų�ʼ��Lrc Parser, ��ע�⡿��֧��ͬʱ�򿪶��Lrc Parser
* @param input CurFolder   ��ǰ�ļ��о��
* @param input FileId      ����ļ�ID����CurFolder != NULLʱ��ʾȫ��ID�����ʾ���ID
* @param input ReadBuffer  Lrc Parser�Ĺ����ڴ�ռ䣬���û��ⲿָ���ڴ�ռ䣬
*                          �������ΪNULL��Ĭ����Parser�ڲ�����
* @param input ReadBufSize Lrc Parser�����ռ�Ĵ�С����������ռ����û�ָ������ReadBuffer != NULL����
*                          ���ֵ���� >= 128����������ռ����ڲ����䣬��ֵ���˿����� >= 128��ֵ�⣬
*                          ����������Ϊ0����ʾLrc Parser�Լ�Ĭ�Ϸ���512Bytes�Ĺ����ռ�
* @return Success - TRUE, Fail - FALSE
*/
bool OpenLrcByFileId(FOLDER* CurFolder, uint32_t FileId,
                         uint8_t* ReadBuffer, uint32_t ReadBufSize);

/**
* @brief �ر�Lrc Parser
* @return Success - TRUE, Fail - FALSE
*/
bool CloseLrcFile(void);

/**
* @brief ����ʱ�����ĳ�и�ʵ�ʵ�ʳ��ȣ�����Ҳ�����ʱ����Ӧ�ĸ���򷵻� -1
* @param input SeekTime ���ʱ���
* @param ���û�ϣ���ö�̬�ڴ�ķ�ʽ��ø��ʱ�����ڵ���GetLrcInfo����ǰ��
*        �ȵ��ñ�����Ԥ��֪���ض�ʱ���ĸ�ʳ��ȣ�Ȼ��̬�����װ�ظ�������ڴ棬
*        ��ΪGetLrcInfo�������������
* @return �����ʱ����Ӧ�ĸ�ʳ��ȣ�����Ҳ�����ʣ��򷵻�-1
*/
int32_t GetLrcTextLength(uint32_t SeekTime);

/**
* @brief ����ʱ����ѯ�����Ϣ����Ҫ�и����ʼʱ�䣬����ʱ�䣬�������
* @param input SeekTime ���ʱ���
* @param input TextOffset ���ƫ�� ���ڶԺܳ��ĸ�ʷֶλ�ȡ��ÿ�δ�ָ��ƫ�Ƶ�ַ��ʼ��һ�θ��
* @param �ú���������ʱ�������ָ����Buffer Size(LrcRow->MaxLrcLen)�����Buffer(LrcRow->LrcText)
*        ���Buffer SizeС��ʵ�ʵĸ�ʳ��ȣ����Զ��ضϸ��
*        ��ע�⡿�����ʹ������ضϵĸ��ĩβ�ַ���Ҫ�жϺ��ֵ������ԣ�
*                ���ڸýӿ�ֵ������ԭ�ַ��������ұ������ͽ϶࣬���Ժ��������ж��ɽӿڵ��������д���

* @return ��ʵ�ʵ�ʳ��ȣ�����Ҳ�����ʣ�����-1��
*        ��ע�⡿������ʵ�ʳ��ȴ��ڸ��Buffer(LrcRow->LrcText)����󳤶�(LrcRow->MaxLrcLen)��
*                �򷵻�ֵ > LrcRow->MaxLrcLen������ ����ֵ <= LrcRow->MaxLrcLen
*/
int32_t GetLrcInfo(LRC_ROW* LrcRow, uint32_t SeekTime/*ms*/, uint32_t TextOffset);

/**
* @brief ��õ�ǰ����ļ��ı����ʽ
* @return ��ʵı�������
*/
TEXT_ENCODE_TYPE GetLrcEncodeType(void);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif





