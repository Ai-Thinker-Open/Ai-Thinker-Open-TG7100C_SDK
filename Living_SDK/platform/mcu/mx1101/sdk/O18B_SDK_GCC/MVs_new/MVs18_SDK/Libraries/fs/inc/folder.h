//maintainer lilu
#ifndef __FOLDER_H__
#define __FOLDER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fsinfo.h"
// Folder info structure define.
typedef struct	_FOLDER
{
	uint16_t			FolderNum;							//�ļ��к�
	uint16_t			ValidFolderNum;						//��Ч�ļ��к�

	uint32_t	        ParentFoldStartSecNum;	            //��Ŀ¼��ʼ������
	uint32_t			DirSecNum;							//FDI����������
	uint8_t			    DirOffset;							//FDI�����������е�ƫ����

	uint32_t			EndDirSecNum;						//�ļ��������һ���ļ�Ŀ¼������������
	uint8_t			    EndDirOffset;						//�ļ��������һ���ļ�Ŀ¼�����������е�ƫ����

	uint32_t			StartSecNum;
	uint8_t			    ShortName[9];						//folder short name

	uint16_t			StartFileNum;						//for calculate absolute file number in whole partition.

	uint16_t			IncFileCnt;							//include file sum of the folder, not recursion
	uint16_t			IncFolderCnt;

	uint16_t			RecFileCnt;							//recursion file sum of the current folder
	uint16_t			RecFolderCnt;

	FS_CONTEXT*         FsContext;

} FOLDER;


//���ļ�����Ŵ��ļ���
//�򿪳ɹ�������TRUE��Ŀ���ļ�����Ϣ��䵽Folder��ָ�Ľṹ�塣
//��ʧ�ܣ�����FALSE��
//ParentFolder == NULL: �������豸�ϵĵ�FolderNum���ļ��С�
//ParentFolder != NULL: ��ParentFolder�ļ����еĵ�FolderNum���ļ��С�
bool FolderOpenByNum(FOLDER* Folder, FOLDER* ParentFolder, uint16_t FolderNum);

//���ļ�����Ч��ţ��˳����ļ��к����ţ����ļ��С�
//��������������FolderOpenByNum()��
bool FolderOpenByValidNum(FOLDER* Folder, FOLDER* ParentFolder, uint16_t ValidFolderNum);

//���ļ������ƴ��ļ��С�
//�򿪳ɹ�������TRUE��Ŀ���ļ�����Ϣ��䵽Folder��ָ�Ľṹ�塣
//��ʧ�ܣ�����FALSE��
//ParentFolder == NULL: �򿪸�Ŀ¼�е�FolderName�ļ��С�
//ParentFolder != NULL: ��ParentFolder�ļ����е�FolderName�ļ��С�
//FolderName[]���Ȳ�����8�ֽڣ���Ŀ¼����Ϊ"\\"
bool FolderOpenByName(FOLDER* Folder, FOLDER* ParentFolder, uint8_t* FolderName);

//
//ͨ�����ļ��д��ļ�
//��Ҫջ�ռ�ϴ� FAT_NAME_MAX *2
//
bool FolderOpenByLfName(FOLDER* Folder, FOLDER* ParentFolder, uint8_t* FolderName, uint8_t Len);

//��ȡָ���ļ��еĳ��ļ�����
//���ļ����Ϊ66���ֽڣ�����LongFileName[]��������Ҫ��66���ֽڣ����򽫻ᵼ�²���Ԥ�ϵĴ���
//��ȡ�����ļ���������TRUE��
//�޳��ļ���������FALSE��
bool FolderGetLongName(FOLDER* Folder, uint8_t* LongFileName, uint8_t GetMaxLength);	//LongFileName[]: 66 Bytes

//�½��ļ��С�
//��ParentFolder�ļ������½�һ������ΪFolderName[]���ļ��С�
//ParentFolder���ļ���ָ�벻��ΪNULL��
//����TRUE�������ɹ����´������ļ�����Ϣ��䵽Folder��ָ�ṹ���С�
//����FALSE������ʧ�ܡ�
bool FolderCreate(FOLDER* Folder, FOLDER* ParentFolder, uint8_t* FolderName);

//�����ļ���������ָ���ļ������½�һ���ļ��С�
//Ŀ���ļ�����Ϣ��䵽Folder��ָ�Ľṹ�塣
//FolderLongName[64]Ϊ���ļ�����,Unicode 16���룬����Ϊ128Byte ����λ��Ϊ0xFF
//FolderShortName[11]Ϊ���ļ�����,Utf-8���룬����Ϊ11Byte ����λ��Ϊ�ո�"0x20"�������λ��Ϊ0x20
//ParentFolder ����Ϊ��
//Len Ϊ���ļ�����ʵ�ʳ��ȣ���λΪuint16_t
bool FolderCreateByLongName(FOLDER* Folder, FOLDER* ParentFolder, uint16_t* FolderLongName, uint8_t* FolderShortName, uint8_t Len);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
