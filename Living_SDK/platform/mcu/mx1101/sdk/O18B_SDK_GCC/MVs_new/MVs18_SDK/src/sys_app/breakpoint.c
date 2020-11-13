///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: breakpoint.c
//  ChangLog :
//			�޸�bp ģ�������ʽ2014-9-26 ��lujiangang
///////////////////////////////////////////////////////////////////////////////

#include "app_config.h"
#include "breakpoint.h"
#include "fat_file.h"
#include "audio_decoder.h"
#include "player_control.h"
#include "nvm.h"
#include "eq_params.h"

#ifdef FUNC_BREAKPOINT_EN

#ifdef BP_SAVE_TO_FLASH // �������
static bool LoadBPInfoFromFlash(void);
static bool SaveBPInfoToFlash(void);
#endif
static BP_INFO bp_info;

// global sysInfo default init nvm value
const static BP_SYS_INFO sInitSysInfo =
{
	0x5A,                           			// ValidFlag

#ifdef FUNC_BT_EN
	(uint8_t)MODULE_ID_BLUETOOTH,               // CurModuleId
#else
	(uint8_t)MODULE_ID_PLAYER_USB,              // CurModuleId
#endif
	SD_TYPE,                        			// DiskType
       
	DEFAULT_VOLUME,     				        // Sys Volume default value
    EQ_STYLE_OFF,                               // Eq
#ifdef FUNC_SOUND_REMIND
	1,                             				// 1:Enable Alarm Remind;
	1,                            				// 1:Chinese Mode
#endif
#ifdef FUNC_BT_HF_EN
	15,								//bt hf volume
#endif
};

#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)) // player module default init nvm value

const static BP_PLAYER_INFO sInitPlayerInfo =
{
	25, 						// Volume
	PLAY_MODE_REPEAT_ALL,  	// PlayMode
	0, 						// LrcFlag
	{{0}, {0}, {0}}, 			// PlayDisk[0:2]
};

#endif

#ifdef FUNC_RADIO_EN // radio module default init nvm value
const static BP_RADIO_INFO sInitRadioInfo =
{
	{0}, 					// �ѱ����̨�б�/*MAX_RADIO_CHANNEL_NUM*/
	10,  						// FM����
	0,   						// ��ǰFM���η�Χ(00B��87.5~108MHz (US/Europe, China)��01B��76~90MHz (Japan)��10B��65.8~73MHz (Russia)��11B��60~76MHz
	0,  						// �ѱ����̨����
	0,   						// ��ǰ��̨Ƶ��
};
#endif


// ����CRC
uint8_t GetCrc8CheckSum(uint8_t* ptr, uint32_t len)
{
	uint32_t crc = 0;
	uint32_t i;

	while(len--)
	{
		crc ^= *ptr++;
		for(i = 0; i < 8; i++)
		{
			if(crc & 0x01)
			{
				crc = ((crc >> 1) ^ 0x8C);
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	return (uint8_t)crc;
}


#ifdef FUNC_DEBUG_EN
void BP_InfoLog(void)
{
	APP_DBG("**********************************\n");
	APP_DBG("*  Signature:%X\n", bp_info.SysInfo.VaildFlag);
	APP_DBG("*  Cur Mode:%X\n", bp_info.SysInfo.CurModuleId);
	APP_DBG("*  DiskType:%X\n", bp_info.SysInfo.DiskType);
	APP_DBG("*\n");
	APP_DBG("**********************************\n");
}
#endif


// ϵͳ����ʱװ��, ��NVM ��Flash����BP ��Ϣ
//�������ʧ�������Ĭ��BP ֵ
//ע�⣬�ú������ܻ��ȡFlash�������Ҫ��SpiFlashInfoInit()����֮�����
void BP_LoadInfo(void)
{
	bool ret = TRUE;

	APP_DBG("BP_LoadInfo\n");

	ASSERT(sizeof(bp_info) <= BP_MAX_SIZE);

//	if(bp_info.SysInfo.VaildFlag == BP_SIGNATURE)
//	{
//		return;
//	}

	NvmRead(0, (uint8_t*)&bp_info, sizeof(bp_info));
	if(bp_info.Crc != GetCrc8CheckSum((uint8_t*)&bp_info, sizeof(bp_info)))
	{
#ifdef BP_SAVE_TO_FLASH
		ret = LoadBPInfoFromFlash();
#else
		ret = FALSE;
#endif
	}

	//��ȡBP ��Ϣʧ�ܣ���Ĭ��ֵ
	if(!ret)
	{
		memcpy((void*)&bp_info.SysInfo, (void*)&sInitSysInfo, sizeof(BP_SYS_INFO));
#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN))
		memcpy((void*)&bp_info.PlayerInfo, (void*)&sInitPlayerInfo, sizeof(BP_PLAYER_INFO));
#endif

#ifdef FUNC_RADIO_EN
		memcpy((void*)&bp_info.RadioInfo, (void*)&sInitRadioInfo, sizeof(BP_RADIO_INFO));
#endif
		bp_info.SysInfo.VaildFlag = BP_SIGNATURE;
	}

	return;
}


//����bp_info �����ģ��BP ��Ϣ��ָ��
//����Ҫ����BP ��Ϣʱ��ֱ��ͨ����ָ����bp_info ������д
void* BP_GetInfo(BP_INFO_TYPE InfoType)
{
	void *pInfo = NULL;
	switch(InfoType)
	{
		case BP_SYS_INFO_TYPE:
			pInfo = (void *)&(bp_info.SysInfo);
			break;
#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN))			
		case BP_PLAYER_INFO_TYPE:
			pInfo = (void *)&(bp_info.PlayerInfo);
			break;
#endif			
#ifdef FUNC_RADIO_EN			
		case BP_RADIO_INFO_TYPE:
			pInfo = (void *)&(bp_info.RadioInfo);
			break;
#endif			
		default:
			break;
	}
	return pInfo;
}



//����BP ��Ϣ��NVM ��FLASH ��EEPROM
//�ɹ�����TRUE�� ʧ�ܷ���FALSE
bool BP_SaveInfo(BP_SAVE_TYPE SaveType)
{
	bool ret;
	uint8_t crc;

	ASSERT(sizeof(bp_info) <= BP_MAX_SIZE);
	
	//��Save��NVM��FLASH��EEPROM֮ǰ�����ȼ����CRC
	crc = GetCrc8CheckSum((uint8_t*)&bp_info, sizeof(BP_INFO)-1);
	bp_info.Crc = crc;
	switch(SaveType)
	{
		case BP_SAVE2NVM:
			ret = NvmWrite(0, (uint8_t *)&bp_info, sizeof(BP_INFO));
			break;
		case BP_SAVE2FLASH:
			#ifdef BP_SAVE_TO_FLASH // �������
			ret = SaveBPInfoToFlash();
			#endif
			break;
		case BP_SAVE2EEPROM:
			//empty now
			break;
		case BP_SAVE2NVM_FLASH:
			ret = NvmWrite(0, (uint8_t *)&bp_info, sizeof(BP_INFO));
			if(ret)
			{
			#ifdef BP_SAVE_TO_FLASH // �������
				ret = SaveBPInfoToFlash();
			#endif
			}
			break;
		case BP_SAVE2NVM_EEPROM:
			//empty now
			break;
		default:
			break;
	}
	return ret;
}




#ifdef BP_SAVE_TO_FLASH // �������

#include "spi_flash.h"

#ifndef DWNLDR_USRDAT_ADDR
#define DWNLDR_USRDAT_ADDR      (0x90)
#endif

//�������ʽ��NVM Flag(4Bytes "MV18") + NVM BAK ID(4Bytes) + NVM Size(2Bytes) + NVM SUB BAK ID(1Bytes) + CRC(1Bytes) + NVM Data(Sub1/Sub2/...)��
//�������ͷ��
typedef struct __BP_BAK_WRAPPER_
{
	char 	Magic[4];
	uint32_t 	BPBakId;
	uint16_t 	BPBakSize;
	uint8_t 	BPBakSubId;
	uint8_t 	Crc;
}BP_BAK_WRAPPER;

#define NVM_BAK_OFFSET		(64*1024)	// BP ��Ϣ��Flash�е���ʼ��ַ
#define NVM_BAK_CNT		(4)			// BP ��Ϣ�洢�������
#define NVM_BLOCK_SIZE		(4096)		//BP ��Ϣ���������С

// BP ��¼��ϢSize : bp_info ��¼��С+ ��¼��ͷ
#define NVM_BAK_SIZE     		192	//(sizeof(BP_INFO) + sizeof(BP_BAK_WRAPPER))	
#define NVM_SUB_BAK_CNT  	((NVM_BLOCK_SIZE)/NVM_BAK_SIZE)


static uint32_t NVM_BAK_ADDR0   = NVM_BAK_OFFSET;	//BP INFO ��Flash�е���ʼ��ַ

// OutputBakId: ���NVM BAK ID
// OutputSubBakId: ���NVM SUB BAK ID
// return: NVM�����ַ,LJG ��������NVM��������block���׵�ַ��һ��4��block
static uint32_t FindLatestBPBak(uint32_t* OutputBakId, uint8_t* OutputSubBakId, bool IsLoadFlash)
{
	uint32_t i, j;
	int32_t TempIndex = -1;
	uint32_t TempBakId;
	BP_BAK_WRAPPER BpBakWrapper;
	uint8_t  TempSubBakId;

	*OutputBakId = 0;
	if(!IsLoadFlash)
	{
		*OutputSubBakId = (NVM_SUB_BAK_CNT - 1);
	}
	else
	{
		*OutputSubBakId = 0;
	}

	for(i = 0; i < NVM_BAK_CNT; i++)
	{
		// read flash to get nvm bak id
		if(SpiFlashRead(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE, (uint8_t*)&BpBakWrapper, sizeof(BpBakWrapper)) < 0 ||
		        memcmp(BpBakWrapper.Magic, "MV18", 4) != 0)
		{
			continue;
		}

		TempBakId    = BpBakWrapper.BPBakId;
		TempSubBakId = BpBakWrapper.BPBakSubId;

		for(j = 0; j < (NVM_BLOCK_SIZE / NVM_BAK_SIZE); j++)
		{
			if(SpiFlashRead(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE + j * NVM_BAK_SIZE, (uint8_t*)&BpBakWrapper, sizeof(BpBakWrapper)) < 0 ||
			        memcmp(BpBakWrapper.Magic, "MV18", 4) != 0)
			{
				break;
			}

			TempBakId    = BpBakWrapper.BPBakId;;
			TempSubBakId = BpBakWrapper.BPBakSubId;
		}

		if(*OutputBakId > TempBakId)
		{
			continue;
		}

		TempIndex       = i;
		*OutputBakId    = TempBakId;
		*OutputSubBakId = TempSubBakId;
	}

	if(TempIndex == -1)
	{
		TempIndex = (NVM_BAK_CNT - 1);
	}
	else if(!IsLoadFlash)
	{
		*OutputSubBakId = (NVM_SUB_BAK_CNT - 1);

		i = TempIndex;
		if(i == 0)
		{
			i = NVM_BAK_CNT - 1;
		}
		else
		{
			i--;
		}
		for(j = 0; j < (NVM_BLOCK_SIZE / NVM_BAK_SIZE); j++)
		{
			if(SpiFlashRead(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE + j * NVM_BAK_SIZE, (uint8_t*)&BpBakWrapper, sizeof(BpBakWrapper)) < 0)
			{
				return (NVM_BAK_ADDR0 + TempIndex * NVM_BLOCK_SIZE);
			}

			if(memcmp(BpBakWrapper.Magic, "MV18", 4) != 0)
			{
				break;
			}

			*OutputSubBakId = BpBakWrapper.BPBakSubId;
		}
	}
	return (NVM_BAK_ADDR0 + TempIndex * NVM_BLOCK_SIZE);
}

static bool LoadBakBPInfo(uint32_t BakNvmAddr, uint8_t SubBakId)
{
	uint8_t CrcVal;
	BP_BAK_WRAPPER BpBakWrapper;
	
	if(SpiFlashRead((uint32_t)((uint8_t*)BakNvmAddr + SubBakId * NVM_BAK_SIZE), (uint8_t*)&BpBakWrapper, sizeof(BpBakWrapper)) < 0)
	{
		return FALSE;
	}
	if(SpiFlashRead((uint32_t)((uint8_t*)BakNvmAddr + SubBakId * NVM_BAK_SIZE + sizeof(BpBakWrapper)), (uint8_t*)&bp_info, sizeof(bp_info)) < 0)
	{
		return FALSE;
	}
	
	CrcVal = GetCrc8CheckSum((uint8_t*)&bp_info, sizeof(BP_INFO));
	if(CrcVal != BpBakWrapper.Crc)
	{
		return FALSE;
	}
	
	if(!NvmWrite(0, (uint8_t*)&bp_info, sizeof(bp_info)))
	{
		return FALSE;
	}

	return TRUE;
}

static bool SaveBakBPInfo(uint32_t BakNvmAddr, uint32_t NvmBakId, uint8_t SubBakId)	//SaveBakNVM
{
	uint8_t CrcVal;
	uint16_t NvmSize = sizeof(BP_INFO);
	uint8_t Temp[sizeof(BP_BAK_WRAPPER)+sizeof(BP_INFO)];
	BP_BAK_WRAPPER *pBakWrapper;

	CrcVal = GetCrc8CheckSum((uint8_t*)&bp_info, sizeof(BP_INFO)-1);
	bp_info.Crc = CrcVal;	//��������bp_info�ڲ�CRC

	//��������bp_info��crc
	CrcVal = GetCrc8CheckSum((uint8_t*)&bp_info, sizeof(BP_INFO));
	pBakWrapper = (BP_BAK_WRAPPER*)&Temp[0];
	memcpy((void*)&pBakWrapper->Magic, (void*)"MV18", 4);
	pBakWrapper->BPBakId = NvmBakId;
	pBakWrapper->BPBakSize = NvmSize;
	pBakWrapper->BPBakSubId = SubBakId;
	pBakWrapper->Crc = CrcVal;
	memcpy((void*)&Temp[sizeof(BP_BAK_WRAPPER)],(void*)&bp_info, sizeof(BP_INFO));

	if(SubBakId == 0)
	{
		uint8_t buf[4];
		SpiFlashRead(BakNvmAddr,buf,4);
		if(buf[0] !=0xFF ||buf[1] !=0xFF ||buf[2] !=0xFF ||buf[3] !=0xFF)
		{
			SpiFlashErase(BakNvmAddr, NVM_BLOCK_SIZE);
		}
		if(SpiFlashWrite(BakNvmAddr, Temp, sizeof(Temp)) < 0)
		{
			return FALSE;
		}
		return TRUE;
	}

	if(SpiFlashWrite((uint32_t)((uint8_t*)BakNvmAddr + SubBakId * NVM_BAK_SIZE), Temp, sizeof(Temp)) < 0)
	{
		return FALSE;
	}
	return TRUE;
}

static bool LoadBPInfoFromFlash(void)	
{
	uint32_t i;
	uint32_t LoadNvmBakId = 0;
	uint32_t NvmBakAddr;
	uint32_t BtDataAddr;
	uint8_t  NvmSubBakId;

	NVM_BAK_ADDR0 = NVM_BAK_OFFSET;

	// �Ȼ�ȡ�����ĵ�ַ������֮�����NVM�������������ͻ
	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&BtDataAddr, 4) < 0)
	{
		return FALSE;//error
	}

	NVM_BAK_ADDR0 += BtDataAddr;

	// ���������NVM����
	NvmBakAddr = FindLatestBPBak(&LoadNvmBakId, &NvmSubBakId, TRUE);

	if(LoadNvmBakId == 0) // ����û�б������ֱ�ӷ���
	{
		uint8_t tmp[4];

		if(SpiFlashRead(NVM_BAK_ADDR0 +  (NVM_BAK_CNT -1) *NVM_BLOCK_SIZE, tmp, 4) < 0)
		{
			return FALSE;//error
		}
		if(tmp[0] != 0xFF ||tmp[1] != 0xFF || tmp[0] != 0xFF || tmp[0] != 0xFF )
		{
			SpiFlashErase(NVM_BAK_ADDR0, NVM_BAK_CNT * NVM_BLOCK_SIZE);
		}
		
		return FALSE;
	}

	// ���������NVM����
	if(LoadBakBPInfo(NvmBakAddr, NvmSubBakId))
	{
		return TRUE;
	}
	else
	{
		// �����������ʧ�ܣ����԰��Ⱥ�˳�����ε�����������ֱ�������ɹ�����������еľ���ʧ��
		for(i = (NvmBakAddr - NVM_BAK_ADDR0) / NVM_BLOCK_SIZE; i < NVM_BAK_CNT; i++)
		{
			if(LoadBakBPInfo(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE, (NVM_SUB_BAK_CNT - 1)))
			{
				return TRUE;
			}
		}

		for(i = 0; i < (NvmBakAddr - NVM_BAK_ADDR0) / NVM_BLOCK_SIZE; i++)
		{
			if(LoadBakBPInfo(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE, (NVM_SUB_BAK_CNT - 1)))
			{
				return TRUE;
			}
		}
	}

	APP_DBG("Load BP From Flash Fail\n");
	return FALSE;
}

static bool SaveBPInfoToFlash(void)
{
	int32_t i;
	uint32_t SaveNvmBakId = 0;
	uint32_t NvmBakAddr;
	uint32_t BtDataAddr;
	uint8_t  SaveNvmSubBakId = NVM_SUB_BAK_CNT - 1;

	NVM_BAK_ADDR0 = NVM_BAK_OFFSET;

	// �Ȼ�ȡ�����ĵ�ַ������֮�����NVM�������������ͻ
	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&BtDataAddr, 4) < 0)
	{
		return FALSE;//error
	}

	NVM_BAK_ADDR0 += BtDataAddr;

	// ���������NVM����
	NvmBakAddr = FindLatestBPBak(&SaveNvmBakId, &SaveNvmSubBakId, FALSE);

	if(SaveNvmBakId != 0)
	{
		if((NvmBakAddr - NVM_BAK_ADDR0) == 0)
		{
			NvmBakAddr = NVM_BAK_ADDR0 + (NVM_BAK_CNT - 1) * NVM_BLOCK_SIZE;
		}
		else
		{
			NvmBakAddr -= NVM_BLOCK_SIZE;
		}
	}

	SaveNvmBakId++;

	SaveNvmSubBakId++;

	if(SaveNvmSubBakId == NVM_SUB_BAK_CNT)
	{
		SaveNvmSubBakId = 0;
	}

	// ����NVM����
	if(SaveBakBPInfo(NvmBakAddr, SaveNvmBakId, SaveNvmSubBakId))
	{
		return TRUE;
	}
	else
	{
		// ������뾵��ʧ�ܣ����԰��Ⱥ�˳�����ε��뵽������������ֱ������ɹ�����������еľ�������ʧ��
		for(i = ((NvmBakAddr - NVM_BAK_ADDR0) / NVM_BLOCK_SIZE - 1); i >= 0; i--)
		{
			if(SaveBakBPInfo(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE, SaveNvmBakId, 0))
			{
				return TRUE;
			}
		}

		for(i = (NVM_BAK_CNT - 1); i >= (NvmBakAddr - NVM_BAK_ADDR0) / NVM_BLOCK_SIZE; i--)
		{
			if(SaveBakBPInfo(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE, SaveNvmBakId, 0))
			{
				return TRUE;
			}
		}
	}

	APP_DBG("Save BP To Flash Fail\n");
	return FALSE;
}

bool EraseFlashOfUserArea(void)
{
#define DWNLDR_USRDAT_ADDR      (0x90)

	uint32_t BtDataAddr;

	NVM_BAK_ADDR0 = 64 * 1024;

	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&BtDataAddr, 4) < 0)
	{
		return FALSE;//error
	}
	NVM_BAK_ADDR0 += BtDataAddr;
	SpiFlashErase(NVM_BAK_ADDR0, 16 * 1024);
	return TRUE;
}
#endif

#endif/*FUNC_BREAKPOINT_EN*/










