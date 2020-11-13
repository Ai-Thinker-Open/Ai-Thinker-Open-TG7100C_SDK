//maintainer lilu
#ifndef __FAT_FILE_H__
#define __FAT_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fs_config.h"
#include "folder.h"
#include "fsinfo.h"


//open mode

#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08

//file status
//#define FA__WRITTEN			0x20
//#define FA__DIRTY			0x40
//#define FA__ERROR			0x80

//define file seek position.
#define	SEEK_FILE_SET				0					//seek file from the first sector of file
#define	SEEK_FILE_CUR				1					//seek file from the current sector of file
#define	SEEK_FILE_END				2					//seek file from the last sector of file 

typedef enum _FILE_TYPE
{
    FILE_TYPE_MP3,
    FILE_TYPE_WMA,
    FILE_TYPE_WAV,
    FILE_TYPE_SBC,
    FILE_TYPE_FLAC,
    FILE_TYPE_AAC,
    FILE_TYPE_AIF,
    FILE_TYPE_AMR,
    FILE_TYPE_UNKNOWN = 0xFF

} FILE_TYPE;



//�ļ��������ṹ
typedef struct _FAT_FILE
{
	uint16_t	FileNumInDisk;			//�������豸�ϵ��ļ���
	uint16_t	FileNumInFolder;		//�ڵ�ǰ�ļ����е��ļ���
	uint16_t	FolderNum;				//�����ļ��еı��
//	uint16_t	ValidFolderNum;			//�����ļ��е���Ч�ļ��б��

	uint32_t	ParentFoldStartSecNum; //��Ŀ¼��ʼ������
	uint32_t 	DirSecNum;             //FDI ����������
	uint32_t 	DirOffset;			   //FDI ���������е�ƫ����

	uint8_t	ShortName[11];			//���ļ���
#if FAT_USE_LFN == 1
	//uint8_t	LongName[FAT_NAME_MAX];			//���ļ���
#endif
	uint8_t	    FileType;				//�ļ�������
	uint32_t	Size;					//�ļ������ֽ���

	uint32_t 	FptrIndex;
	uint32_t 	FptrCluster;
	uint32_t	StartClusNum;			//�ļ���һ���غ�
	uint32_t	SecterOffset;			// Sector offset in the cluster

	uint32_t    Fptr;					//��ǰλ�����ļ���ƫ���ֽ���
	//uint32_t    DataSec;				//��ǰ�������е�����������
	//uint8_t* 	IOBuffer;				//�ļ���д������
	uint8_t     Flag;				    //����ļ�״̬

	FS_CONTEXT* FsContext;
} FAT_FILE;


//���á���ȡ�ļ�ʱ��Ľṹ��
typedef struct _FILE_TIME
{
	uint16_t Year;	//1980��2107
	uint8_t  Month;	//1��12
	uint8_t  Date;	//1��31
	uint8_t  Hour;	//0��23 
	uint8_t  Min;	//0��59
	uint8_t  Sec;	//0��59

} FILE_TIME;


//
// @brief  Open a file
// @param  file_name    file name   forexample: \\mp3\\short.mp3,\\mp3\\short.mp3
// @param  mode         open mode   FA_READ��FA_WRITE��FA_CREATE_NEW��FA_CREATE_ALWAYS
// @return File Handle
// �ַ������ļ���֧�����������ļ��л��ļ�
// ֧�ֳ��ļ���
bool FileOpen(FAT_FILE* File, const uint8_t* FileName, const uint8_t Mode);

//�ж��ļ������ͣ����ļ�ϵͳ�ײ����
uint8_t FileGetType(FS_CONTEXT* FsContext);

//
// @brief  Check End-of-File indicator.
// @param  File      Pointer to a FILE object that specifies stream.
// @return The feof function returns a nonzero value if a read operation has attempted to read past the end of the file; it returns 0 otherwise.
//
int32_t FileEOF(FAT_FILE* File);

//���ص��ļ���ͷ
void FileRewind(FAT_FILE* File);

//���ļ���Ŵ��ļ�
//�򿪳ɹ�������TRUE��Ŀ���ļ���Ϣ��䵽File��ָ�Ľṹ�塣
//��ʧ�ܣ�����FALSE��
//Folder == NULL: �������豸�ϵĵ�FileNum���ļ���
//Folder != NULL: ��Folder�ļ����еĵ�FileNum���ļ��С�
bool FileOpenByNum(FAT_FILE* File, FOLDER* Folder, uint16_t FileNum);

//���ļ����ƴ�ָ���ļ����е��ļ���
//�򿪳ɹ�������TRUE��Ŀ���ļ���Ϣ��䵽File��ָ�Ľṹ�塣
//��ʧ�ܣ�����FALSE��
//Folderָ�벻��Ϊ��ָ�롣
bool FileOpenByName(FAT_FILE* File, FOLDER* Folder, uint8_t* FileName);

//���ļ������ƴ�ָ���ļ����е��ļ���
//�򿪳ɹ�������TRUE��Ŀ���ļ���Ϣ��䵽File��ָ�Ľṹ�塣
//��ʧ�ܣ�����FALSE��
//Folder����Ϊ�գ�FileNameΪUnicode 16����
bool FileOpenByLfName(FAT_FILE* File, FOLDER* Folder, uint8_t* FileName, uint8_t Len);

//
// @brief  Moves the file pointer to a specified location.
// @param  File      Pointer to a FILE object that specifies stream.
// @param  Offset  Number of bytes from origin.
// @param  Base  Initial position.
// @return If successful, the function returns a zero value. Otherwise, it returns nonzero value.
//
int32_t FileSeek(FAT_FILE* File, int32_t Offset, uint8_t Base);

//
// @brief  Read data from stream
// @param  buffer  Pointer to a block of buffer with a minimum size of (size*count) bytes.
// @param  size    Size in bytes of each element to be read.
// @param  count   Number of elements, each one with a size of size bytes.
// @param  File      Pointer to a FILE object that specifies an input stream.
// @return The total number of elements successfully read.
//
uint32_t FileRead(void* buffer, uint32_t size, uint32_t count, FAT_FILE* File);

//
// @brief  Write data to stream
// @param  buffer  Pointer to a block of buffer with a minimum size of (size*count) bytes.
// @param  size    Size in bytes of each element to be write.
// @param  count   Number of elements, each one with a size of size bytes.
// @param  File      Pointer to a FILE object that specifies an output stream.
// @return The total number of elements successfully write.
//
uint32_t FileWrite(const void* buffer, uint32_t size, uint32_t count, FAT_FILE* File);

//
// @brief  Get size of the opened file.
// @param  File      Pointer to a FILE object that specifies an output stream.
// @return size of the opened file.
//
int32_t FileSof(FAT_FILE* File);

//
// @brief  Gets the current position of a file pointer.
// @param  File      Pointer to a FILE object that specifies stream.
// @return code
//
int32_t FileTell(FAT_FILE* File);

//
// @brief  File save Fat and Dir
// @param  File      Pointer to a FILE object that specifies an output stream.
// @return fclose returns 0 if the stream is successfully closed.
//
int32_t FileSave(FAT_FILE* File);

//
// @brief  Closes a stream.
// @param  File      Pointer to a FILE object that specifies an output stream.
// @return fclose returns 0 if the stream is successfully closed.
//
int32_t FileClose(FAT_FILE* File);

//��ȡָ���ļ��ĳ��ļ�����
//���ļ���LongFileName����С��GetMaxLength
//��ȡ�����ļ���������TRUE��
//�޳��ļ���������FALSE��
bool FileGetLongName(FAT_FILE* File, uint8_t* LongFileName, uint8_t GetMaxLength);

//�ļ���ա�
//�ɹ�:����TRUE��ʧ��:����FALSE��
bool FileFlush(FAT_FILE* File);

//�ļ�ɾ�����ɹ�:����TRUE��ʧ��:����FALSE��
bool FileDelete(FAT_FILE* File);

//���ļ�������ָ���ļ������½�һ���ļ���
//Ŀ���ļ���Ϣ��䵽File��ָ�Ľṹ�塣
//FileName[]Ϊ���ļ��������磺"123.TXT", "ABC123.MP3"��
//���ļ������Ȳ��ܳ���8+3�ֽڣ����ļ����в���ͬʱ���ִ�Сд��ĸ��
bool FileCreate(FAT_FILE* File, FOLDER* Folder, uint8_t* FileName);

//�����ļ�������ָ���ļ������½�һ���ļ���
//Ŀ���ļ���Ϣ��䵽File��ָ�Ľṹ�塣
//FileName[]Ϊ���ļ���
//Folder ����Ϊ�գ�FileNameΪUnicode 16���룬LongFileName��Ϊ128 �ֽڿ���λ��Ϊ0xFF
bool FileCreateByLfName(FAT_FILE* File, FOLDER* Folder, uint8_t* LongFileName, uint8_t Len);

bool FSFormat(void);

//�޸��ļ���ʱ����Ϣ�� 
//CreateTime: ����ʱ�䣬����Ϊ1��
//ModifyTime: ����޸�ʱ�䣬����Ϊ2��
//AccessTime: ������ʱ�䣬ֻ�����ڣ�����ʱ�䲿��
//���ĳ��ʱ����Ϣ�����ָ��ΪNULL�����ʾ���ı��ʱ����Ϣ
//�ɹ�:����TRUE�� ʧ��:����FALSE��
bool FileSetTime(FAT_FILE* File, FILE_TIME* CreateTime, FILE_TIME* ModifyTime, FILE_TIME* AccessTime);

//��ȡ�ļ���ʱ����Ϣ�� 
//CreateTime: ����ʱ�䣬����Ϊ1��
//ModifyTime: ����޸�ʱ�䣬����Ϊ2��
//AccessTime: ������ʱ�䣬ֻ�����ڣ�����ʱ�䲿��
//���ĳ��ʱ����Ϣ�����ָ��ΪNULL�����ʾ����ȡ��ʱ����Ϣ
//�ɹ�:����TRUE�� ʧ��:����FALSE��
bool FileGetTime(FAT_FILE* File, FILE_TIME* CreateTime, FILE_TIME* ModifyTime, FILE_TIME* AccessTime);

void SetFsTerminateFuc(TerminateFunc func);

extern TerminateFunc terminate_cur_fs_proc;

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //_FAT_FILE_H_
