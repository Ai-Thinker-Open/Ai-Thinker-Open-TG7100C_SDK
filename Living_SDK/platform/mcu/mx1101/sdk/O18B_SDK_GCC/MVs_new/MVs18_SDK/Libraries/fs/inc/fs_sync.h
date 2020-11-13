//maintainer lilu
#ifndef __FS_SYNC_H__
#define	__FS_SYNC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fsinfo.h"
//
//��ҪĿ��Ϊʵ���ļ�ϵͳ�Ķ��߳�ͬ��
//

//
// init sync semaphore
//
uint8_t FsSyncInit(void* FsContext);

//
// lock
//
void FsLock(void* FsContext);

//
// unlock
//
void FsUnLock(void* FsContext);

#define ENTER_FAT(fs)	  	FsLock(fs)

#define LEAVE_FAT(fs, res)  \
{							\
	FsUnLock(fs);	        \
	return res;         	\
}


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //_FS_SYNC_H_
