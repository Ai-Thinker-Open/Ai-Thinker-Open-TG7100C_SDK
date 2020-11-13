/////////////////////////////////////////////////////////////////////////
//                  Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc.,ShangHai,China
//                      All rights  reserved
//  Filename:filesort.h
//maintainer lilu
/////////////////////////////////////////////////////////////////////////

#ifndef __FS_FILESORT_H__
#define __FS_FILESORT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fs_config.h"

#if FS_FILE_SORT_EN == 1
//
//Ԥ�����������ҵ�һ���µĸ����ļ��������ļ���������������
//��PreSearch()��������
void FileSortSearchNewFile(FS_CONTEXT* FsContext);

//Ԥ��������������ļ�������
//Ȼ���������б�ѹ��������FileSortTable[]������
//��PreSearch()��������
void FileSortSearchEnd(FS_CONTEXT* FsContext);

//�ú�����FileOpenByNum()����֮ǰ����
//���������ĵ�FileSortNum���ļ�����Ӧ��ԭʼȫ���ļ���
//����ԭʼ�ļ��Ų���[StartFileNum + 1, EndFileNum]��Χ�ڵ��ļ�
//1.����ȫ���������ĵ�m���ļ���ԭʼȫ���ļ��ţ�
//		GetFileNumInDisk(FsContext, m, 0, 0xFFFF)���õ�ȫ��������m���ļ���Ӧ��ԭʼȫ���ļ���
//2.�����ļ����������ĵ�m���ļ���ԭʼȫ���ļ��ţ�
//		GetFileNumInDisk(FsContext, m, Folder->StartFileNum, Folder->StartFileNum + Folder->IncFileCnt)
uint16_t GetFileNumInDisk(FS_CONTEXT* FsContext, uint16_t FileSortNum, uint16_t StartFileNum, uint16_t EndFileNum);

#endif //FS_FILE_SORT_EN

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //__FS_FILESORT_H__
