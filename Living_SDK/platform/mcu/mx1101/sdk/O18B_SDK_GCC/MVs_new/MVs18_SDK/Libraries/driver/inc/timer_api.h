/*
* timer_api.h
* timer API for application module
* auther: lean.xiong @2011-09-13
* notes: ���Timer��һ���;��ȵĶ�ʱ������ʱ�������С��MIN_TIMER_PERIOD
* maintainer lujiangang
*/
#ifndef __TIMER_API_H__
#define __TIMER_API_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define APP_TIMER_API // ��OS Timer �ķ�װ


#ifdef APP_TIMER_API

unsigned int xTaskGetTickCount(void);

#define MIN_TIMER_PERIOD 50/*ms*/ // ��С��ʱ���

#define MAX_SYS_TICKS 0xFFFFFFFF // ϵͳTicks���ֵ�����ڼ�ʱ��ת

typedef void (*TIMER_CALLBACK)(void* Param);

#pragma pack(1)

typedef struct _SW_TIMER_
{
	uint32_t TimerPeriod;  // ms,��ע�⡿��С��ʱ�����Ӧ��ģʽ��ѭ����������������ʱ���йأ����鲻Ҫ������ѭ�����������ʱ�䣻
	uint32_t LastTickets;  // ����timeout�ж�
	uint32_t InitTicks;    // Timer��һ�ο���ʱ��ticks
	uint32_t PausedTimes;  // ms������ͣ��ʱ��
	TIMER_CALLBACK Callback;
	uint8_t  IsRunning;
	uint8_t  TicksResetCount; // ��¼SysTicks��ת����
} SW_TIMER;

#pragma pack()

// ��õ�ǰTimerʵ������ʱ�䣬��λms
uint32_t GetPassTime(SW_TIMER* TimerHandle);

// Ӧ��ģʽ��ѭ���У����øú�������ѯTimer����ʱ����CallBack��
void CheckTimer(SW_TIMER* TimerHandle);

// ��ʼ��Timer
bool InitTimer(SW_TIMER* TimerHandle,
	               uint32_t TimerPeriod, /*��ʱ���ms*/
	               TIMER_CALLBACK CallbackFunc/*��ʱ�ص�����*/);

// ����Timer
bool StartTimer(SW_TIMER* TimerHandle);

// ֹͣTimer
bool StopTimer(SW_TIMER* TimerHandle);

// ����Timer
bool DeinitTimer(SW_TIMER* TimerHandle);

// ����Timer�ļ�ʱ״̬����ע�⡿�����ǰ��ֵ����ʵ������ʱ�䣬Timer����ʱ����
bool ResetTimer(SW_TIMER* TimerHandle);

// ����Timer��ʱ���
bool ChangeTimerPeriod(SW_TIMER* TimerHandle, uint32_t TimerPeriod/*��ʱ���*/);

#endif/*SYS_TIMER_API*/

#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*TIMER_API_H*/

