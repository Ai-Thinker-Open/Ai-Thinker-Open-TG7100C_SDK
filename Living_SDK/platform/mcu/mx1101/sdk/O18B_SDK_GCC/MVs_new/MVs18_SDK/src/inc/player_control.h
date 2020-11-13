///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: player_control.h
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#ifndef __PLAYER_CONTROL_H__
#define __PLAYER_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "lrc.h"
#include "audio_decoder.h"

typedef enum _PLAYER_STATE
{
    PLAYER_STATE_IDLE = 0,    // ����
    PLAYER_STATE_SWITCH_NEXT, // �л���һ���У���ʱ״̬������ɾ��
    PLAYER_STATE_SWITCH_PRE,  // �л���һ���У���ʱ״̬������ɾ��
    PLAYER_STATE_PLAYING,     // ����
    PLAYER_STATE_PAUSE,       // ��ͣ
    PLAYER_STATE_FF,          // �����
    PLAYER_STATE_FB,          // ������
} PLAYER_STATE;


typedef enum _AUDIO_PLAYER_PLAY_MODE_
{
    PLAY_MODE_REPEAT_ALL,     // ˳��ѭ������
    PLAY_MODE_REPEAT_FOLDER,
    PLAY_MODE_REPEAT_ONE,     // ����ѭ������
//  PLAY_MODE_REPEAT_FB_PLAY, // ����ģʽ������ģʽӦ�ÿ���������ģʽ���棬���ܷ��ڲ���ģʽ�У�������Ҫ����ʵ�ָù��ܣ�
    PLAY_MODE_RANDOM,         // �������
	PLAY_MODE_RANDOM_FOLDER,
    PLAY_MODE_PREVIEW_PLAY,   // Ԥ�����ţ����ļ�����У�ѡ�и�������Ԥ�����ż����ӣ�
    PLAY_MODE_SUM,
} AUDIO_PLAYER_MODE;

// �û��ɸ�����Ҫ������ʵ��ʹ�õ�EQ����
#define EQ_STYLE_OFF (EQ_STYLE_SUM)

///////////////////////////////////////////////////////
// ����ģʽ�����ݽṹ������ϵͳ�����е����ĸ���ģʽ����(����ʱ����ѭ������)
#define REPEAT_CLOSED  0 // �����ر�
#define REPEAT_A_SETED 1 // �����ø������
#define REPEAT_OPENED  2 // ������

typedef struct _REPEAT_AB_
{
	uint32_t StartTime; // ��ʼʱ��(MS)������ʱ��ѡ�񸴶�ģʽ���������
	uint32_t Times;      // ����ʱ�䳤��(MS)�������ͨ�����ý���ʱ��ķ�ʽ���������յĸ���ʱ���Դ�Ϊ׼����Ԥ��ֵ
	uint8_t LoopCount;  // ѭ����������Ԥ������(EPROM)��Ĭ��3��
	uint8_t RepeatFlag; // ��ǰ�Ƿ񸴶�ģʽ(0 - �����ر�, 1 - ���������, 2 - �������յ㣬����ʼ����)
} REPEAT_AB;

///////////////////////////////////////////////////////
typedef struct _AUDIO_PLAYER_CONTROL_
{
	uint32_t	CurPlayTime;		// ����ʱ��, ms
	uint32_t	LastPlayTime;		// ��ʱֵ�������ϴ�UIˢ��ʱ�Ĳ���ʱ��ֵ������ˢ���Ż�

	SongInfo*	CurSongInfo;		// ��ǰ������Ϣ
	FAT_FILE	PlayerFile;
	bool		FileOpenSuccessFlag;
	bool		IsPlayerPreSong;
	bool		IsPlayerBeStop;
	uint8_t*	CurSongTitlePtr;	// ��ǰ�������⡢�����ҵ���Ϣ

	uint16_t	CurFileIndex;		// ��ǰ�ļ��ţ����CurFolderIndex == 1 �����ȫ��ID������������ID
	uint16_t	CurFolderIndex;		// ��ǰ�ļ���ȫ��ID�ţ������ļ��и�Ŀ¼����Ŀ¼���������ļ��в���ģʽ
	uint16_t	TotalFileNumInDisk;	// ȫ���е��ļ�����
	uint8_t		State;				// ������״̬ (uint8_t)
	uint8_t		CurPlayMode;		// ��ǰ����ģʽ

	int8_t		FFAndFBCnt;			// �ۼ�������FF(+)��FB(-)�����Ĵ���
	int32_t		StartPoint;			// �и�ʱ��ָ����һ�׸�����λ��(ms)

#ifdef FUNC_LRC_EN
	uint8_t		LrcFlag;			// �����ʾ��־(EPROM������)
	uint8_t		IsLrcRunning;
	LRC_ROW		LrcRow;
#endif

	SW_TIMER	PlayTimer;			// ����״̬��ʱ��
	SW_TIMER	DelayDoTimer;		// �ӳٴ���Timer�������������л���������������˵Ȱ������ӳ��ۼӴ���

	REPEAT_AB	RepeatAB;			// ����ģʽ������Ϣ

	FOLDER		FolderHandle;		// ��ǰ�ļ��о����Ĭ���Ǵ��̸�Ŀ¼��

#ifdef PLAYER_SPECTRUM 				// Ƶ����ʾ
	uint8_t		SpecTrumFlag;		// Ƶ�״򿪱�ʶ
	SPECTRUM_INFO SpecTrumInfo;
#endif
	uint16_t	error_times;		// ĳЩ�������������Ĵ������û��ָ�����
	uint8_t		IsBPMached;			// �Ƿ�FS��ƥ���˶ϵ���Ϣ����Ҫ���ڷ�ֹ������Ч�Ķϵ���Ϣ(����ģʽ��ʼ��ʧ�ܺ���Ҫ�ı�־�ж��Ƿ񱣴�ϵ���Ϣ)

	uint16_t	InputNumber;		// ��������ѡȡ
	SW_TIMER	NumKeyTimer;

	uint8_t		PlayFolderFlag;
	uint16_t	ValidFolderCnt;

#ifdef FUNC_REC_PLAYBACK_EN
	uint8_t		RecBackPlayFlag;
#endif

#ifndef FUNC_PLAY_RECORD_FOLDER_EN
	uint16_t	RecFileStartId;
	uint16_t	RecFileEndId;
	uint16_t	RecFolderId;
#endif

#ifdef FUNC_BROWSER_EN
	bool	BrowseFlag;
#endif
} AUDIO_PLAYER_CONTROL;


extern AUDIO_PLAYER_CONTROL*  gPlayContrl;
/////////////////////////////////////////////////////////////////////////

// ȥ��ʼ��
bool AudioPlayerDeinitialize(void);

// ��ʼ������� FolderIndex == 1����FileIndex��ʾȫ��ID�����FolderIndex > 1����FileIndex��ʾ���ID
bool AudioPlayerInitialize(int32_t FileIndex, uint32_t FolderIndex, bool IsNeedInitFs);
bool AudioPlayerEvent(uint16_t Msg);
bool PlayerControl(void);
void SetPlayState(uint8_t state);
uint8_t GetPlayState(void);
void AudioPlayerPlayPause(void);
void AudioPlayerStop(void);
bool AudioPlayerNextSong(void);
bool AudioPlayerPreSong(void);


void PlayerTimerCB(void);

bool IsPlayerEnvironmentOk(void);

extern AUDIO_PLAYER_CONTROL*  gPlayContr;

#ifdef __cplusplus
}
#endif//__cplusplus

#endif





