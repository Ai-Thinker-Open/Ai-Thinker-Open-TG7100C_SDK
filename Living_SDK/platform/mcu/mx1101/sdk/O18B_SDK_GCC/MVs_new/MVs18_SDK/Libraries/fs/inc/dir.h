//maintainer lilu
#ifndef __DIR_H__
#define __DIR_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fsinfo.h"

//file attributes define
#define	ATTR_READ_ONLY			0x01
#define	ATTR_HIDDEN				0x02
#define	ATTR_SYSTEM 			0x04
#define	ATTR_VOLUME_ID			0x08
#define	ATTR_DIRECTORY			0x10
#define	ATTR_ARCHIVE			0x20
#define	ATTR_LONG_NAME			0x0f	//ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID
#define	ATTR_LONG_NAME_MASK		0x3f	//ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID | ATTR_DIRECTORY | ATTR_ARCHIVE
#define	ATTR_RECYCLED_MASK		0x16	//ATTR_DIRECTORY | ATTR_HIDDEN | ATTR_SYSTEM								



typedef enum _ENTRY_TYPE
{
    ENTRY_FILE,
    ENTRY_FOLDER,		//�ļ���Ŀ¼��������Ͻ�������ļ���
    ENTRY_END, 			//��ǰĿ¼�������
    ENTRY_RECYCLED,
    ENTRY_DOT,
    ENTRY_LONG_NAME,
    ENTRY_DELETED_FILE,
    ENTRY_UNKNOWN

} ENTRY_TYPE;


void DirSetStartEntry(FS_CONTEXT* FsContext, uint32_t StartSecNum, uint8_t Offset, bool InitFlag);

uint32_t DirGetSonClusNum(FS_CONTEXT* FsContext);

uint32_t DirGetSonSecNum(FS_CONTEXT* FsContext);

void DirSetSonClusNum(FS_CONTEXT* FsContext, uint32_t ClusNum);

//�ڵ�ǰĿ¼������һ���ļ�Ŀ¼��
uint8_t DirGetNextEntry(FS_CONTEXT* FsContext);

bool DirFindNextFolder(FS_CONTEXT* FsContext);

//������Ŀ¼
bool DirEnterSonFolder(FS_CONTEXT* FsContext);

//���ظ�Ŀ¼
bool DirEnterParentFolder(FS_CONTEXT* FsContext);

void RewindFolderStart(FS_CONTEXT* FsContext);

bool DirNameConvert(uint8_t* DstFileName, uint8_t* SrcFileName);

void DirGenItem(FS_CONTEXT* FsContext, uint8_t* DirName, uint32_t StartClusNum, uint32_t Size, uint8_t Attribute);

void DirGenLongItem(FS_CONTEXT* FsContext, uint8_t* longName, uint8_t* shortName, uint8_t Len);

// Load file/folder long name.
bool GetLongName(FS_CONTEXT* FsContext, uint32_t ParentFoldSect, uint32_t SectorNum, uint8_t Offset, uint8_t* LongFileName, uint8_t GetMaxLength);

bool FindNextFolder(FS_CONTEXT* FsContext);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
