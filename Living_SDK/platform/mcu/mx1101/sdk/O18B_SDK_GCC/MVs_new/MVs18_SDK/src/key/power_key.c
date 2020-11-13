////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2014, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:power_key_driver.c
//
//		Description	:
//					power keyboard driver(soft push botton)
//
//             maintainer: lujiangang
//
//		Changelog	:
//					2014-09-25	Created Source File
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "adc.h"
#include "app_config.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "app_msg.h"
#include "wakeup.h"

#if (defined(FUNC_POWER_KEY_EN) && defined(USE_POWERKEY_SOFT_PUSH_BUTTON))

	#warning	"MV: PUSH BUTTON POWER KEY ENABLE!"

#define 	POWER_KEY_JITTER_TIME		100			//����ʱ�䣬��ʱ������ؿ��ػ�Ӳ��ʱ���й�
#define 	POWER_KEY_CP_TIME			100			


//extern bool PowerkeyGetOnkeyReg(void);


typedef enum _POWER_KEY_STATE
{
	POWER_KEY_STATE_IDLE,
	POWER_KEY_STATE_JITTER,
	POWER_KEY_STATE_PRESS_DOWN,
	POWER_KEY_STATE_CP,

} POWER_KEY_STATE;


TIMER			PowerKeyWaitTimer;
POWER_KEY_STATE	PowerKeyState;
										
static const uint16_t PowerKeyEvent[2] = {MSG_NONE/*������ʼ*/, MSG_PWR_KEY_SP/*�̰��ɿ�*/};	


// Initialize POWER_KEY scan operation.
void PowerKeyScanInit(void)
{
	DBG("PowerKeyScanInit*******\n");
	PowerKeyState = POWER_KEY_STATE_IDLE;
}


// POWER_KEY����ͨ�İ�����ͬ�����Ӱ�ť���أ����أ�ʱ����Ҫ���û���ϵͳ���ػ�����Ȼ��Ҳ�����ö̰����ܡ�
// �̰�����ʱ�����Ͷ̰���Ϣ�������̰����䣬�˴������κδ�����ϵͳӲ�����ػ���
uint16_t PowerKeyScan(void)							
{
	switch(PowerKeyState)
	{
		case POWER_KEY_STATE_IDLE:
			if(!PowerkeyGetOnkeyReg())
			{
				return MSG_NONE;
			}
			else
			{	
				TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_JITTER_TIME);
				PowerKeyState = POWER_KEY_STATE_JITTER;
			}
			break;
		case POWER_KEY_STATE_JITTER:
			if(!PowerkeyGetOnkeyReg())
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
			}
			else if(IsTimeOut(&PowerKeyWaitTimer))
			{
				PowerKeyState = POWER_KEY_STATE_PRESS_DOWN;
				TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_CP_TIME);
				return PowerKeyEvent[0];			//return key sp value
			}
			break;
			
		case POWER_KEY_STATE_PRESS_DOWN:
			if(!PowerkeyGetOnkeyReg())
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
				return PowerKeyEvent[1];
			}
			else if(IsTimeOut(&PowerKeyWaitTimer))
			{
				PowerKeyState = POWER_KEY_STATE_CP;
				return PowerKeyEvent[0];
			}
			break;
			
		case POWER_KEY_STATE_CP:
			//�˴�����֤һ�ΰ���������Ӧ��ζ̰�
			if(!PowerkeyGetOnkeyReg())
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
				return PowerKeyEvent[1];//return MSG_NONE;				
			}
			else
			{
				//do no thing
			}
			break;
			
		default:
			PowerKeyState = POWER_KEY_STATE_IDLE;
			break;
	}
	return MSG_NONE;
}

#endif
