///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: breakpoint.h
//  ChangLog :
//			�޸�bp ģ�������ʽ2014-9-26 ��lujiangang
///////////////////////////////////////////////////////////////////////////////
#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

// Disk����
#define UNKNOWN_DISK_TYPE -1
#define NONE_DISK 0 // û�в������
#define USB_TYPE  1 // U��Ϊ������
#define SD_TYPE   2 // SD��Ϊ������

#ifdef FUNC_BREAKPOINT_EN

#define BP_MAX_SIZE		115		//BP ��Ϣ����ֽ���(���ǵ�NVM�ռ�����)
#define BP_SIGNATURE	0x5A
#define BP_SET_ELEMENT(a, b) a = b
#define BP_GET_ELEMENT(x) x
#pragma pack(1)


//**************************************************************************************************************
//	PLAYER ��ضϵ�������ݽṹ
//	ע��:: �޸�BP_PLAYER_INFO �������sInitPlayerInfo ����
//**************************************************************************************************************
#if defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)	//PLAYER ���ݴ洢�ṹ

#define PLAYER_DISK_BP_CNT 3

typedef struct _BP_PLAY_DISK_INFO_
{
	uint32_t FileAddr; 		// �ļ�������
	uint16_t PlayTime; 		// ����ʱ��
	uint8_t  CRC8;     		// �ļ���У����
	uint8_t  FolderEnFlag; 	//�ļ���ʹ�ܱ�־
} BP_PLAY_DISK_INFO;

typedef struct _BP_PLAYER_INFO_ // ����ģʽ ���ݴ洢�ṹ
{
	// ��������
	uint8_t	PlayerVolume; // Volume:0--32
	// ��ǰ����ģʽ
	uint8_t	PlayMode : 7; // Play mode
	// ��ʿ���
	uint8_t 	LrcFlag  : 1; // Lrc
	// ��������Ϣ
	BP_PLAY_DISK_INFO PlayDiskInfo[PLAYER_DISK_BP_CNT];
} BP_PLAYER_INFO;
#endif




//**************************************************************************************************************
//	Radio ��ضϵ�������ݽṹ
//	ע��:: �޸�BP_RADIO_INFO �������sInitRadioInfo ����
//**************************************************************************************************************
#ifdef FUNC_RADIO_EN
typedef struct _BP_RADIO_INFO_
{
	uint8_t		StationList[50]; 		// �ѱ����̨�б�/*MAX_RADIO_CHANNEL_NUM*/
	uint8_t		RadioVolume   : 6;  	// FM����
	uint8_t		CurBandIdx : 2;  		// ��ǰFM���η�Χ(00B��87.5~108MHz (US/Europe, China)��01B��76~90MHz (Japan)��10B��65.8~73MHz (Russia)��11B��60~76MHz
	uint8_t		StationCount;    		// �ѱ����̨����
	uint16_t	CurFreq;         				// ��ǰ��̨Ƶ��
} BP_RADIO_INFO;
#endif




//**************************************************************************************************************
//	SYSTEM ��ضϵ�������ݽṹ
//	ע��:: �޸�BP_SYS_INFO �������sInitSysInfo ����
//**************************************************************************************************************
typedef struct _BP_SYS_INFO_
{
	uint8_t 	VaildFlag;   // Only when this field is equal to BP_SIGNATURE, this structure is valid.
	// ��ǰӦ��ģʽ
	uint8_t 	CurModuleId; // system function mode.
	// ��ǰ������������(����ģʽ��¼�����ļ���صĶ����õ�)
	uint8_t 	DiskType;    // USB_TYPE - U�̣�SD_TYPE - SD��
	uint8_t  	Volume;
	uint8_t	    Eq;          // EQ
    
#ifdef FUNC_SOUND_REMIND
	uint8_t 	SoundRemindOn;
	uint8_t 	LanguageMode;
#endif
#ifdef FUNC_BT_HF_EN
	uint8_t 	HfVolume;
#endif
} BP_SYS_INFO;






//**************************************************************************************************************
//	Breakpoint �ϵ�������ݽṹ
//**************************************************************************************************************
typedef struct	_BP_INFO_
{
	BP_SYS_INFO    	SysInfo;
	
#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)) 	// ����ģʽ���ݴ洢����
	BP_PLAYER_INFO PlayerInfo;
#endif

#ifdef FUNC_RADIO_EN // FMģʽ���ݴ洢����
	BP_RADIO_INFO 	RadioInfo;
#endif

	uint8_t 			Crc;
	
} BP_INFO;



typedef enum _BP_SAVE_TYPE_
{
	BP_SAVE2NVM,
	BP_SAVE2FLASH,
	BP_SAVE2EEPROM,
	BP_SAVE2NVM_FLASH,
	BP_SAVE2NVM_EEPROM
}BP_SAVE_TYPE;

typedef enum _BP_INFO_TYPE_
{
	BP_SYS_INFO_TYPE,
	BP_PLAYER_INFO_TYPE,
	BP_RADIO_INFO_TYPE,
}BP_INFO_TYPE;

#pragma pack()

bool EraseFlashOfUserArea(void);
void BP_InfoLog(void);
uint8_t GetCrc8CheckSum(uint8_t* ptr, uint32_t len);	// ����CRC
void BP_LoadInfo(void);
void* BP_GetInfo(BP_INFO_TYPE InfoType);
bool BP_SaveInfo(BP_SAVE_TYPE SaveType);

#endif
#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*__BREAKPOINT_H_*/

