///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: browser.c
//  maintainer: lilu
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "fat_file.h"
#include "folder.h"

#ifdef FUNC_BROWSER_EN

#define DISP_SHORT_NAME_LEN	11		//�ǵ�ǰ����ʾ����󳤶�
#define DISP_LONG_NAME_LEN	66		//��ǰ����ʾ����󳤶�
#define DISP_ROW_CNT		5		//�����ʾ������

//�ļ�����ڵ���Ϣ�����Ա����ļ����ļ�����Ϣ��
typedef struct _BW_NODE
{
	uint8_t Name[DISP_SHORT_NAME_LEN];

	bool 	 IsFile;		//TRUE: file, FALSE: folder
	uint16_t NumInDisk;		//�ļ����ļ�����ȫ���ϵ����
	uint16_t NumInFolder;	//�ļ����ļ���������Ŀ¼�е����
	uint16_t IncFileCnt;	//������ļ��У���������ļ����������ݹ飩
	uint16_t IncFolderCnt;	//������ļ��У���������ļ��и��������ݹ飩

} BW_NODE;

//���·���ڵ���Ϣ
typedef struct _BW_PATH_NODE
{
	FOLDER 	 Folder;
	uint16_t StartRowNum;
	uint8_t	 CurNodeIndex;

} BW_PATH_NODE;


static BW_NODE	BrowserNodes[DISP_ROW_CNT];			//���浱ǰ���ҳ������Ҫ��ʾ���ļ����ļ��е���Ϣ
static uint8_t	CurNodeIndex;						//��ǰ���ڻ����е�λ��
static uint8_t	NodesCnt;							//�������л���ĸ���
static uint16_t	StartRowNum;						//�������е�һ���ڵ�����
//static uint8_t 	CurLongName[DISP_LONG_NAME_LEN];	//���浱ǰ�е��ļ����ļ��еĳ����ƣ����ڹ�����ʾ
static FOLDER	CurFolder;							//��ǰ����������ļ���

//��¼�Ӹ��ļ��е���ǰ�ļ��е�·����Ϣ���Ա��ܹ�������һ���ļ���
static BW_PATH_NODE 	BrowserPath[MAX_STACK_DEPTH];
static uint8_t			CurPathLevel;


static void BW_Display(void)
{
	uint8_t i;
		
	DBG("\n****");
	
	//��ʾ��ǰ�ļ���·��
	for(i = 0; i < CurPathLevel; i++)
	{
		DBG("%-.8s\\", &BrowserPath[i].Folder.ShortName[0]);
	}

	DBG("%-.8s(%d, %d)*******\n", 
		&CurFolder.ShortName[0],		
		CurFolder.IncFolderCnt, 
		CurFolder.IncFileCnt);

	for(i = 0; i < NodesCnt; i++)
	{
		if(i == CurNodeIndex)
		{
			DBG(">>");	
		}
		else
		{
			DBG("  ");	
		}

		if(!BrowserNodes[i].IsFile)		
		{	
			//��ʾ�ļ���Ϣ			
			DBG("[] %-.8s (%d, %d)\n", 
				&BrowserNodes[i].Name[0], 
				BrowserNodes[i].IncFolderCnt, 
				BrowserNodes[i].IncFileCnt);			
		}
		else							
		{	
			//��ʾ�ļ���Ϣ
			DBG("** %-.8s.%-.3s\n", &BrowserNodes[i].Name[0], &BrowserNodes[i].Name[8]);	
		}
	}
	DBG("*************************\n\n");	
}


static bool BW_LoadNodes(uint8_t NodeIndex, uint16_t Num, uint8_t n)
{
	FOLDER Folder;
	FAT_FILE File;
	
	DBG("LoadNode(%d, %d, %d)\n", (uint16_t)NodeIndex, (uint16_t)Num, (uint16_t)n);
	DBG("(%d, %d)\n", (uint16_t)CurFolder.IncFolderCnt, (uint16_t)CurFolder.IncFolderCnt);		
	while(n--)
	{
		if(NodeIndex >= DISP_ROW_CNT)
		{
			return FALSE;
		}

		if(Num <= CurFolder.IncFolderCnt)
		{
		 	//�����ļ�����Ϣ
			if(!FolderOpenByNum(&Folder, &CurFolder, Num))
			{
				DBG("Open folder error 001!\n");
				return FALSE;
			}
			memcpy(&BrowserNodes[NodeIndex].Name[0], &Folder.ShortName[0], 8);
			BrowserNodes[NodeIndex].IsFile = FALSE;
			BrowserNodes[NodeIndex].NumInFolder = Num;
			BrowserNodes[NodeIndex].IncFileCnt = Folder.IncFileCnt;
			BrowserNodes[NodeIndex].IncFolderCnt = Folder.IncFolderCnt;
		}
		else if(Num <= CurFolder.IncFolderCnt + CurFolder.IncFileCnt)	
		{
			//�����ļ���Ϣ
			if(!FileOpenByNum(&File, &CurFolder, Num - CurFolder.IncFolderCnt))
			{
			 	DBG("Open file error!\n");
				return FALSE;
			}
			memcpy(&BrowserNodes[NodeIndex].Name[0], &File.ShortName[0], 11);
			BrowserNodes[NodeIndex].IsFile = TRUE;
			BrowserNodes[NodeIndex].NumInFolder = Num - CurFolder.IncFolderCnt;
			BrowserNodes[NodeIndex].NumInDisk = File.FileNumInDisk;
			BrowserNodes[NodeIndex].IncFileCnt = 0;		//����ֻ���ļ�����Ч
			BrowserNodes[NodeIndex].IncFolderCnt = 0;	//����ֻ���ļ�����Ч
			FileClose(&File);
		}
		else
		{
		 	return FALSE;
		}

		Num++;
		NodeIndex++;
	}

	return TRUE;
}


//��ʾ��ǰ�ļ��������ļ��к��ļ������ݹ飩
//���StartNum == 0: �����л����ϼ�����Ϣ��NumΪĿ���к�
//���StartNum != 0: ��StartNum�ʼ���أ�NumΪĿ���к�
static bool BW_LoadInfo(uint16_t StartNum, uint16_t Num)
{	
	uint8_t i;

	if(Num == 0)
	{
		Num = 1;
	}
	else if(Num > CurFolder.IncFolderCnt + CurFolder.IncFileCnt)
	{
		Num = CurFolder.IncFolderCnt + CurFolder.IncFileCnt;
	}

	if(!StartNum)
	{
		//�����������л�����: StartRowNum ~ StartRowNum+LoadCnt
		if(Num < StartRowNum - 1)
		{
			//ȫ�����¼��أ���Num��ʼ����DISP_ROW_CNT��, CurNodeIndex = 0
			CurNodeIndex = 0;
			StartRowNum = Num;
			BW_LoadNodes(0, Num, DISP_ROW_CNT);
		}
		else if(Num == StartRowNum - 1)
		{
			//ȫ�����һ�ֻ����Numһ�У�����������һ��, CurNodeIndex = 0
			for(i = DISP_ROW_CNT - 1; i > 0; i--)
			{
				memcpy(&BrowserNodes[i], &BrowserNodes[i - 1], sizeof(BW_NODE));
			}
			CurNodeIndex = 0;
			StartRowNum = Num;
			BW_LoadNodes(0, Num, 1);
		}
		else if(Num > StartRowNum + DISP_ROW_CNT)
		{
			//ȫ�����¼��أ���(Num - DISP_ROW_CNT + 1)��ʼ����DISP_ROW_CNT��, CurNodeIndex = (DISP_ROW_CNT - 1)
			CurNodeIndex = DISP_ROW_CNT - 1;
			StartRowNum = Num - DISP_ROW_CNT + 1;
			BW_LoadNodes(0, StartRowNum, DISP_ROW_CNT);
		}
		else if(Num == StartRowNum + DISP_ROW_CNT)
		{
			//ȫ��ǰ��һ�ֻ����Numһ�У������������һ��, CurNodeIndex = (DISP_ROW_CNT - 1)
			memcpy(&BrowserNodes[0], &BrowserNodes[1], sizeof(BW_NODE)*(DISP_ROW_CNT - 1));
			CurNodeIndex = DISP_ROW_CNT - 1;
			StartRowNum = Num - DISP_ROW_CNT + 1;
			BW_LoadNodes(DISP_ROW_CNT - 1, Num, 1);
		}
		else
		{
			//����Ҫ���¼���, CurNodeIndex = (Num - StartRowNum)
			CurNodeIndex = Num - StartRowNum;
		}
	}
	else
	{
		//��ʼ��ȫ���ؼ���
		//��StartNum�п�ʼ���������CurNodeIndex = (Num - StartNum)
		CurNodeIndex = Num - StartNum;
		StartRowNum = StartNum;
		
		NodesCnt = CurFolder.IncFolderCnt + CurFolder.IncFileCnt;
		if(NodesCnt > DISP_ROW_CNT)
		{
			NodesCnt = DISP_ROW_CNT;
		}

		BW_LoadNodes(0, StartNum, NodesCnt);
	}

	BW_Display();
	return TRUE;
}


//static void DisplayPath(void)
//{
//	uint8_t i;
//
//	DBG(("*******Path: %d*********\n", (WORD)CurPathLevel));
//	for(i = 0; i < CurPathLevel; i++)
//	{
//		DBG(("%-.8s  %d %d\n", 
//			&BrowserPath[i].Folder.ShortName[0],
//			(WORD)BrowserPath[i].StartRowNum,
//			(WORD)BrowserPath[i].CurNodeIndex));
//	}
//}


//���뵱ǰ�ļ��е�ĳ�����ļ���
//·���в�������ǰ�ļ�����Ϣ
static bool BW_EnterSubFolder(uint16_t SubFolderNum)
{
	//����Ƿ������ȣ����ļ������Ϊ0
	if(CurPathLevel >= MAX_STACK_DEPTH - 1)
	{
		return FALSE;
	}
	
	//����ǰ�ļ��б��浽·��
	memcpy(&BrowserPath[CurPathLevel].Folder, &CurFolder, sizeof(FOLDER));
	BrowserPath[CurPathLevel].StartRowNum = StartRowNum;
	BrowserPath[CurPathLevel].CurNodeIndex = CurNodeIndex;
	CurPathLevel++;
//	DisplayPath();

	//�����ļ���
	if(!FolderOpenByNum(&CurFolder, &CurFolder, SubFolderNum))
	{
	 	DBG("Open folder error 002!\n");
		return FALSE;
	}

	DBG("Open folder OK!\n");

	//��ʾ��ǰ�ļ��������ļ��к��ļ������ݹ飩
	BW_LoadInfo(1, 1);

	return TRUE;
}


//���ص�ǰ�ļ��е��ϼ��ļ���
static bool BW_EnterParentFolder(void)
{
	//����ǰ�ļ��д�·����ɾ��
	if(CurPathLevel == 0)
	{
		DBG("���Ǹ�Ŀ¼��\n"); 
		return FALSE;
	}

//	DisplayPath();
	CurPathLevel--;	
	memcpy(&CurFolder, &BrowserPath[CurPathLevel].Folder, sizeof(FOLDER));
	BW_LoadInfo(BrowserPath[CurPathLevel].StartRowNum, BrowserPath[CurPathLevel].StartRowNum + BrowserPath[CurPathLevel].CurNodeIndex);  
	return TRUE;
}


//Browser initial.
bool FileBrowseInit(void)
{
	DBG("FileBrowseInit()\n");
	
	CurPathLevel = 0;

	//�򿪸�Ŀ¼
	if(!FolderOpenByNum(&CurFolder, NULL, 1))
	{
	 	DBG("Open root error!\n");
		return FALSE;
	}
	DBG("Open root OK!\n");

	DBG("%-.8s (%d folder, %d files)  %ld  %d\n", 
		CurFolder.ShortName, 
		CurFolder.IncFolderCnt, 
		CurFolder.IncFileCnt, 
		CurFolder.StartSecNum, 
		CurFolder.StartFileNum);

	//��ʾ��ǰ�ļ��������ļ��к��ļ������ݹ飩
	BW_LoadInfo(1, 1);	
	return TRUE;
}


bool FileBrowseOp(uint16_t Msg)
{
	DBG("FileBrowseOp()\n");
	switch(Msg)
	{
		case MSG_PRE:		//�����һ���ļ���
			BW_LoadInfo(0, StartRowNum + CurNodeIndex - 1);
			break;

		case MSG_NEXT:		//�����һ���ļ���	
			BW_LoadInfo(0, StartRowNum + CurNodeIndex + 1);
			break;

		case MSG_PLAY_PAUSE:	//�������ļ��У��򲥷Ÿ���
			if(BrowserNodes[CurNodeIndex].IsFile)
			{
				return TRUE;
			}
			else
			{
				DBG("�������ļ���\n");
				BW_EnterSubFolder(BrowserNodes[CurNodeIndex].NumInFolder);
			}
			break;

		case MSG_STOP:	//������һ���ļ���
			BW_EnterParentFolder();
			break;

		default:
			break;
	}
	return FALSE;
}

bool FileBrowseGetResult(FOLDER** Folder, uint16_t* FileNumInFolder, uint16_t* FileNumInDisk)
{
	DBG("FileBrowseGetResult()\n");
	if(Folder != NULL)
	{
		*Folder = &CurFolder;
	}
	
	if(FileNumInFolder != NULL)
	{
		*FileNumInFolder = BrowserNodes[CurNodeIndex].NumInFolder;
	}
	
	if(FileNumInDisk != NULL)
	{
		*FileNumInDisk = BrowserNodes[CurNodeIndex].NumInDisk;
	}
	return TRUE;
}

#endif

