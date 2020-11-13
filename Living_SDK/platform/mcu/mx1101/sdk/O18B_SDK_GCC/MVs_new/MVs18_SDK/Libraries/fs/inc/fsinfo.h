/////////////////////////////////////////////////////////////////////////
//                  Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc.,ShangHai,China
//                      All rights  reserved
//  Filename:fsinfo.h
//maintainer lilu
/////////////////////////////////////////////////////////////////////////

#ifndef __FSINFO_H__
#define __FSINFO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <string.h>
#include "fs_config.h"

#define		SECTOR_SIZE			512		//bytes per sector
/////////////////////////�˳����ļ���//////////////////////////////////////////////////////
#define	MAX_FOLDER_NUM_SAVE 	2048			//save foler 1-2048 empty flag

// Define device ID in system.
typedef	enum _DEV_ID
{
    DEV_ID_NONE,
    DEV_ID_USB,
    DEV_ID_SD,
    DEV_ID_SPIFS,
    DEV_ID_ATA
} DEV_ID;

//��ȡ��ǰĬ�Ϲ������豸����
extern uint8_t FsGetDriveType(void);

// File system structure
typedef struct _FS_INFO
{
	DEV_ID 	DevID;				//device id of this drive
	bool 	FAT32;				//is FAT32 file system
	bool	IsCpatFS;			//is compatalbe file system

	bool	IsFATChange;
	//bool	IsDIRChange;

	int32_t	CurrFATSecNum;		//current FAT sector number
	//int32_t	CurrDirSecNum;		//current directory sector number

//	uint8_t  	ClusterSize;		//sector per cluster  ExFat Max Sector Size:32MB
	uint32_t  	ClusterSize;		//sector per cluster
	uint32_t    FatStart;			//the sector number of FAT start
	uint32_t  	RootStart;			//This is set to the cluster number of the first cluster of the root director.,
	uint32_t  	DataStart;			//the first sector of data
	uint32_t  	MaxCluster;			//max cluster number in this drive
	uint32_t  	MaxSector;			//max sector number in this drive

	uint32_t  	FreeClusNum;

	uint16_t	FileSum;			//�����豸�ϵĸ����ļ�����
	uint16_t	FileSumInFolder;	//�ļ�ϵͳ�ڲ�ʹ��
	uint16_t	FolderSum;			//�����豸�ϵ��ļ�������
	uint16_t	FolderNumInFolder;	//�ļ�ϵͳ�ڲ�ʹ��

	uint16_t	ValidFolderSum;		//�����豸�ϵķǿ��ļ�������

	uint16_t	RootFileSum;		//���ļ����µ��ļ�����

} FS_INFO;


#define	MAX_STACK_DEPTH 	9
typedef struct _LEAD_PATH
{
	uint32_t		SecNum[MAX_STACK_DEPTH];		//���ڵ�����������������
	uint8_t		Offset[MAX_STACK_DEPTH];		//���ڵ������������������ڲ���ƫ����
	uint8_t		NodesCnt;

	uint32_t		FdiSecNum;						//ĩ�˽ڵ����������ţ�������ƫ����ʼ��Ϊ0

	uint16_t 		PreFileNumInDisk;				//�������ڵ�֮ǰ���ļ����������������ڵ�
	uint16_t 		PreFileNumInFolder;				//�������ڵ�֮ǰͬһ�ļ����е��ļ����������������ڵ�

	uint16_t 		ParentFolderNum;				//�������ڵ����ڵ��ļ��к�
	uint16_t 		PreFolderNumInFolder;			//�ڱ������ڵ������ļ����ڣ��������ڵ�֮ǰͬһ�����ļ��и���
	uint16_t		PreFolderNumInDisk;				//���ļ��б���˳��ʱ�����ڵ�֮ǰȫ���������ļ��и���
	uint16_t		StartFileNumNextFolder;			//���ڵ������ļ����Լ�֮ǰ�����ļ����е������ļ�������������һ���ļ���ʱʹ��

	uint32_t		FolderDirStart;

} LEAD_PATH;

#define MAX_LEAD_PATH_CNT	10		//����¼10������·����ÿ��·���������7�������ڵ㣬��ͬ·���ϵĽڵ�������ظ���	



#define			MEMORY_PAGE_CNT		1024

typedef struct _FDI_STATCK
{
	uint32_t		SecNum[MAX_STACK_DEPTH];			//���ڵ�����������������
	uint8_t		Offset[MAX_STACK_DEPTH];			//���ڵ������������������ڲ���ƫ����
	uint16_t		FolderNum[MAX_STACK_DEPTH];			//���ڵ�ͬһ�ļ����У��ڱ��ڵ�֮ǰ���ļ�����

	uint8_t		Depth;								//����·���ϵĽڵ����

} FDI_STATCK;


//define File Description Block
#pragma pack(1)
typedef struct _FDB
{
	uint8_t	FileName[8];
	uint8_t	ExtName[3];
	uint8_t	Attribute;
	uint8_t 	NTRes;
	uint8_t	CrtTimeTenth;
	uint16_t	CreateTime;
	uint16_t	CreateDate;
	uint16_t	AccessDate;
	uint16_t	StartHigh;			//this entry's first cluster number. High
	uint16_t	ModifyTime;
	uint16_t	ModifyDate;
	uint16_t	StartLow;			//this entry's first cluster number. Low
	uint32_t	Size;				//32-bit uint32_t holding this file��s size in bytes

} FDB;
#pragma pack()

#pragma pack(1)
// Define long name dirctory entry sturct.
typedef struct _LONG_DIR_ENTRY
{
	uint8_t		Ord;
	uint8_t		Name1[10];
	uint8_t		Attribute;
	uint8_t		Type;
	uint8_t		CheckSum;
	uint8_t		Name2[12];
	uint8_t		Resv[2];
	uint8_t		Name3[4];

} LONG_DIR_ENTRY;
#pragma pack()

#define PRESEARCH_STATUS_NONE        0x00
#define PRESEARCH_STATUS_SEARCHING   0x01
#define PRESEARCH_STATUS_SEARCHED    0x02
typedef struct _FS_IO_BUFFER
{
	uint8_t Buffer[SECTOR_SIZE];
	uint32_t BufferSecNum;	
	bool	IsBufferChange;//������Buffer ��BufferSecNumͬʱ�ı�
}FS_IO_BUFFER;

// File system context
typedef struct _FS_CONTEXT
{
	// File System memory address.
	uint8_t CurrentBufferFlag;	//ָʾIO_BUFFER���ĸ���������µ�
	FS_IO_BUFFER IO_BUFFER[2];	//����ԭFileIoBuffer��DIR_BUFFER�����ý�ʡ�ڴ�
	
	uint8_t FAT_BUFFER[SECTOR_SIZE];


	FS_INFO gFsInfo;  // File system structure

	uint8_t	gPreSearchStatus;
	//�����ļ�ϵͳ����������Ϣ
	//ÿ�����������ڵ��������Ϣ��Ŀ¼�����������š������ļ��е�·���������ļ��б�š�ǰһ�������ļ����
	//���ļ��ʹ��ļ���ʱ���ô�������Ϣ��ʵ�ֿ��ٶ�λ��Ŀ���ļ����ļ���
	//���ļ���ʱ�����ô�������Ϣ������ͳ���ļ��а������ļ������ļ��и���
	LEAD_PATH			LeadPaths[MAX_LEAD_PATH_CNT];
	uint16_t				CurrentInterval;
	uint16_t				FdiCnt;
	uint8_t				OutIndex;

	//
	uint16_t	gPreFileNum;
	uint16_t	gPreFileNumInFolder;
	uint16_t	gPreFolderNum;
	uint16_t	gPreFolderNumInFolder;
	bool	gFindFolderFlag;


	uint16_t 	FolderStartFileNum;

	bool	gMemoryPageMapValid;

	//dir
	FDB*			gCurrentEntry;
	uint32_t			gDirSecNum;
	uint8_t 			gEntryOffset;
	uint32_t 			gFolderDirStart;
	FDI_STATCK		FdiStack;
	//bool			gExcludeRecycled = FALSE;

	//fat_write
	uint8_t			gMemoryPageMap[MEMORY_PAGE_CNT / 8];
	uint32_t			gClusStepNum;

	//folder
	uint16_t IncFileSum;
	uint16_t IncFolderSum;

	uint8_t gFolderEmpty[MAX_FOLDER_NUM_SAVE / 8];		//one bit per folder

	uint32_t gCurFreeClusNum;


} FS_CONTEXT;

extern bool FSInit(uint8_t DeviceID);
extern bool FSDeInit(uint8_t DeviceID);

//����һ�麯��Ϊ�ļ�ϵͳ�ڲ�ʹ�á�
void FSINFO_Init(FS_CONTEXT* FsContext);
void FSINFO_SaveFdi(FS_CONTEXT* FsContext);
void FSINFO_ChangeFolderNum(FS_CONTEXT* FsContext);
void FSINFO_ChangeFileNum(FS_CONTEXT* FsContext);
void FSINFO_InitOpenFolder(FS_CONTEXT* FsContext, uint16_t StartFolderNum, uint16_t FolderNum);
bool FSINFO_FindLastNode(FS_CONTEXT* FsContext,  uint32_t SecNum, uint8_t Offset, uint8_t Depth, bool RecFlag, bool RootFlag);
void FSINFO_InitOpenFile(FS_CONTEXT* FsContext, uint16_t FileNum);


//��ĳ���ļ��б�ʶ����Ϊ�ǿա�
void ClrFolderEmptyFlag(FS_CONTEXT* FsContext, uint16_t FolderNum);

//��ĳ���ļ��б�ʶ����Ϊ�ա�
void SetFolderEmptyFlag(FS_CONTEXT* FsContext, uint16_t FolderNum);

//�ж�ĳ���ļ����Ƿ�Ϊ�ա�
bool IsFolderEmpty(FS_CONTEXT* FsContext, uint16_t FolderNum);

//����ԭʼ�ļ��кż�����Ч�ļ��кţ��˳����ļ��к����ţ���
uint16_t GetValidFolderNum(FS_CONTEXT* FsContext, uint16_t OriginalFolderNum);

//������Ч�ļ��кţ��˳����ļ��к����ţ�����ԭʼ�ļ��кš�
uint16_t GetOriginalFolderNum(FS_CONTEXT* FsContext, uint16_t ValidFolderNum);

//����ǿ��ļ�����Ŀ
//uint16_t GetValidFolderSum();

// ��ȡ��ǰ�ѳ�ʼ����������ʹ�õ��豸����
DEV_ID GetCurFsDevType(void);

// ��ȡFSɨ����벥��ģʽ�ϵ���Ϣ��ƥ����ļ����ļ���ID��
#ifdef FUNC_MATCH_PLAYER_BP
typedef struct __PLAYER_BP_RET_
{
	int8_t BPIndex; // ��ƥ��Ĳ��Ŷϵ����(0~2), -1 -- not found
	uint16_t FileId;   // ���Ŷϵ������ļ�ID
	uint16_t FolderId; // ���Ŷϵ������ļ���ID
} PLAYER_BP_RET;

typedef struct _PLAYER_BP_INFO_ // ����ģʽ �ϵ���Ϣ
{
	uint32_t FileAddr;
	uint8_t  CRC8;
} PLAYER_BP_INFO;


// ��ȡ���Ŷϵ�ƥ����Ϣ��Ӧ����FS��ʼ���ɹ������
PLAYER_BP_RET* GetMatchPlayerBPRet(void);
void SetMatchPlayerBPRet(int8_t BPIndex, uint16_t FileId, uint16_t FolderId);

#endif

const unsigned char *GetLibVersionFs(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
