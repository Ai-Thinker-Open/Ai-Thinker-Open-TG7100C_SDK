/**
 *****************************************************************************
 * @file     rtc.h
 * @author   Yancy
 * @version  V1.0.0
 * @date     19-June-2013
 * @brief    rtc module driver header file
 * @maintainer: Sam
 * change log:
 *			 Modify by Sam -20140624
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * ����ÿ��������ռ�õ�memory�ֽ���,�ϲ����ɾݴ��Լ���Ҫ֧�ֵ��������������memory�ռ�
 */
#define   MEM_SIZE_PER_ALARM     6
/**
 * ��������������������ͬʱ֧��8������
 */	
#define   MAX_ALARM_NUM			2

/**
 * ����״̬����
 */
#define ALARM_STATUS_OPENED      0x01  /**< ��״̬   */
#define ALARM_STATUS_CLOSED      0x02  /**< �ر�״̬   */
#define ALARM_STATUS_UNUSED      0x03  /**< δʹ��״̬ */
/**
 * ����ģʽ����
 */
typedef enum _RTC_ALARM_MODE
{
	ALARM_MODE_ONCE_ONLY = 1,	/**< �������� */
	ALARM_MODE_PER_DAY,			/**< ÿ������ */
	ALARM_MODE_PER_WEEK,		/**< ÿ������ */
	ALARM_MODE_WORKDAY,			/**< ���������� */
	ALARM_MODE_USER_DEFINED,	/**< �Զ������� */
	ALARM_MODE_MAX,	
} RTC_ALARM_MODE;


#pragma pack(1)	/*Ensure this structure is byte aligned, and not use padding bytes */
typedef struct _ALARM_INFO
{
	uint32_t AlarmTimeAsSeconds;
	uint8_t  AlarmStatus : 2;
	uint8_t  AlarmMode : 4;
	uint8_t  AlarmData; //�������ڣ�bit0�������죬bit6��������
} ALARM_TIME_INFO;
#pragma pack()


/**
 * ����ʱ��ṹ�嶨��
 */
typedef struct _RTC_DATE_TIME
{
	uint16_t	Year;  /**< ������ */
	uint8_t	Mon;   /**< ������ */
	uint8_t	Date;  /**< ������ */
	uint8_t	WDay;  /**< ���ںţ�0�������գ�1~6������1����6 */
	uint8_t	Hour;  /**< Сʱ�� */
	uint8_t	Min;   /**< ������ */
	uint8_t	Sec;   /**< ���� */
} RTC_DATE_TIME;



/**
 * ũ��ʱ��ṹ�嶨��
 */
typedef struct _RTC_LUNAR_DATE
{
	uint16_t Year;         /**< ũ���� */
	uint8_t Month;        /**< ũ���� */
	uint8_t Date;         /**< ũ���� */
	uint8_t MonthDays;	   /**< ũ��ÿ�µ�������30 or 29*/
	bool IsLeapMonth;  /**< �Ƿ�Ϊ����*/
} RTC_LUNAR_DATE;


/**
 * @brief  RTCģ���ʼ����
 * @param  AlarmWorkMem:�ϲ����Ϊ���ӹ��ܷ����memory�ռ���׵�ַ��
 * @param  AlarmWorkMemSize���ϲ����Ϊ���ӹ��ܷ����memory�ռ�Ĵ�С��AlarmWorkMemSize�ɿͻ�ϣ��ͬʱ֧�ֵ����
 *         ���Ӹ���������ÿ��������Ҫ��memory�ռ���rtc.h���ɺ궨��MEM_SIZE_PER_ALARMȷ��������ͻ���Ҫ���֧��
 *		   8�����ӣ���ô�ϲ����Ŀռ��СӦΪMEM_SIZE_PER_ALARM*8���ϲ����������Ϊ��
 *		   #define MAX_ALARM_NUM 8	//���ͬʱ֧��8������
 *		   uint8_t AlarmWorkMem[MEM_SIZE_PER_ALARM * MAX_ALARM_NUM];
 *		   RtcInit(AlarmWorkMem, MEM_SIZE_PER_ALARM * MAX_ALARM_NUM);
 * @return NONE
 */
void RtcInit(uint8_t* AlarmWorkMem, uint8_t AlarmWorkMemSize);

/**
 * @brief  ��ȡ��ǰʱ��
 * @param  ʱ��ṹ��ָ�룬���ڷ��ص�ǰʱ��
 * @return NONE
 */
void RtcGetCurrTime(RTC_DATE_TIME* CurrTime);


/**
 * @brief  ���õ�ǰʱ��
 * @param  ʱ��ṹ��ָ�룬ΪҪ���õ�ʱ��ֵ
 * @return NONE
 */
void RtcSetCurrTime(RTC_DATE_TIME* CurrTime);

/**
 * @brief  Get days count in the month of the year
 * @param  Year:  the year number
 * @param  Month: the month number
 * @return days count in the month of the year
 */
 uint8_t RtcGetMonthDays(uint16_t Year, uint8_t Month);

/**
 * @brief  ��ȡĳ�����ӵ�����ģʽ������ʱ��
 * @param  AlarmTime������ʱ��ṹ��ָ�룬���ڱ����ȡ������ʱ��
 * @param  AlarmMode: ����ģʽָ�룬���ڱ����ȡ������ģʽ
 * @param  ModeData: ������ģʽ��������Ч��ʱ�䣨�ܼ������Զ��ͬʱ��Ч��
 * @param  AlarmID�����Ӻ�
 * @return ���ִ�гɹ�����TRUE�����򷵻�FALSE��
 */
bool RtcGetAlarmTime(uint8_t AlarmID, uint8_t* AlarmMode, uint8_t* AlarmData, RTC_DATE_TIME* AlarmTime);


/**
 * @brief  ����ĳ�����ӵ�����ģʽ������ʱ��
 * @param  AlarmTime������ʱ��ṹ��ָ�룬���ڱ�������ʱ��
 * @param  AlarmMode: ����ģʽ
 * @param  ModeData: ������ģʽ��������Ч��ʱ�䣨�ܼ������Զ��ͬʱ��Ч��
 * @param  AlarmID�����Ӻ�
 * @return ���ִ�гɹ�����TRUE�����򷵻�FALSE��
 */
bool RtcSetAlarmTime(uint8_t AlarmID, uint8_t AlarmMode, uint8_t AlarmData, RTC_DATE_TIME* AlarmTime);


/**
 * @brief  ��������״̬
 * @param  AlarmID�����Ӻ�
 * @param  AlarmStatus��Ҫ���õ�����״̬
 * @arg        ALARM_STATUS_OPENED
 * @arg        ALARM_STATUS_CLOSED
 * @arg        ALARM_STATUS_UNUSED
 * @return ���ִ�гɹ�����TRUE�����򷵻�FALSE��
 */
bool RtcAlarmSetStatus(uint8_t AlarmID, uint8_t AlarmStatus);


/**
 * @brief  ��ȡ���ӵ�״̬(��/�ر�/δʹ��)��
 * @param  AlarmID�����Ӻ�
 * @return ��������״̬��
 *         ALARM_STATUS_OPENED
 *         ALARM_STATUS_CLOSED
 *         ALARM_STATUS_UNUSED
 */
uint8_t RtcGetAlarmStatus(uint8_t AlarmID);


/**
 * @brief  ����Ƿ������ӵ��ˡ�
 * @param  NONE
 * @return ����0��ʾû�����ӵ������ش���0��ֵ��ʾ��Ӧ�����ӵ���
 */
uint8_t RtcCheckAlarmFlag(void);

/**
 * @brief  ��������жϱ�־
 * @param  NONE
 * @return NONE
 */
void RtcAlarmIntClear(void);


/**
 * @brief  ���ӵ�ʱ��Ҫ���Ĵ����ϲ���յ����ӵ�����Ϣ�󣬱�����ô˺�����
 * @param  NONE
 * @return NONE
 */
void RtcAlarmArrivedProcess(void);



/**
 * @brief  ��������תũ������
 * @param  DateTime:����ʱ��ṹ��ָ�룬��Ϊ�������
 * @param  LunarDate:ũ��ʱ��ṹ��ָ�룬��Ϊ�������
 * @return NONE
 */
void SolarToLunar(RTC_DATE_TIME* DateTime, RTC_LUNAR_DATE* LunarDate);

/**
 * @brief  ��ȡũ����ݵ����
 * @param  Year:ũ�����
 * @return ����0-9, �ֱ��Ӧ���: {"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"}
 */
uint8_t GetHeavenlyStem(uint16_t Year);

/**
 * @brief  ��ȡũ����ݵĵ�֧
 * @param  Year:ũ�����
 * @return ����0-11, �ֱ��Ӧ��֧: {"��", "��", "��", "î", "��", "��", "��", "δ", "��", "��", "��", "��"}
 *         �ֱ��Ӧ��Ф: {"��", "ţ", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��"}
 */
uint8_t GetEarthlyBranch(uint16_t Year);

/**
 * @brief  ����ʱ���ܵ�����ֵ
 * @param  NONE
 * @return ʱ���ܵ�����ֵ
 */
uint32_t RtcGetRefCnt(void);

/**
 * @brief  �������ӼĴ�����ֵ
 * @param  AlarmCnt�����ӼĴ�����ֵ
 * @return NONE
 */
void RtcSetAlarmCnt(uint32_t AlarmCnt);

uint16_t RtcDayOffsetInYear(uint16_t Year, uint8_t Month, uint8_t Date);

bool RtcIsLeapYear(uint16_t Year);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
