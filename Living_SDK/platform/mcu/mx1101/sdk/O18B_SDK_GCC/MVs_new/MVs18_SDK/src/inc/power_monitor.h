//  maintainer: lujiangang
#ifndef __POWER_MONITOR_H__
#define __POWER_MONITOR_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/*
**********************************************************
*					��������
**********************************************************
*/
//
//���ܼ��ӳ�ʼ��
//ʵ��ϵͳ���������еĵ͵�ѹ��⴦���Լ����ó���豸������IO��
//
void PowerMonitorInit(void);

//
//ϵͳ��Դ״̬��غʹ���
//ϵͳ���������LDOIN���ڿ�����ѹ������ϵͳ���������м��LDOIN
//
void PowerMonitor(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
