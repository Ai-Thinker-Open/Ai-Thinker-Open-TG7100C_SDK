////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:msgq.h
//
//  maintainer: Halley
//
//		Description	:
//			define user & system message type
//          ��ע�⡿���ļ�����OS�ں�����ʹ�õ���Ϣ�������ο����û���Ҫ���޸ģ�����Ϣ�����ƺ���չ��ͳһ�޸�msgq_plus.h�ļ�
//           *** ��ϸ˵������ο��ĵ���ϵͳ��Ϣ����ϸ˵��.doc��
//           *** ����˵����
//           1. ��Ϣ���������ͣ��㲥��Ϣ��ϵͳ��Ϣ���豸��Ϣ��Misc��Ϣ��
//           2. �㲥��Ϣ����Ϣ���� - [0x0000 ~ 0x00FF]����Ҫ�ص������е�Task�������յ��㲥��Ϣ��
//           3. ϵͳ��Ϣ����Ϣ���� - [0x0100 ~ 0x0FFF]����Ҫ����һЩOS��ص���Ϣ���Լ�����ģ�������Ϣ��
//           4. �豸��Ϣ����Ϣ���� - [0x1000 ~ 0x2FFF]����Ҫ���������Χ�豸��Ϣ����
//           5. Misc��Ϣ����Ϣ���� - [0x3000 ~ 0x3FFF]������Ӧ��ģ�����Ϣ�������߸���������Ϣ(���÷����)��
//           6. ��ϢID��ʽ(32Bits)��4Bits(����ID) + 2Bits(���ȼ�) + 8Bits(��������) + 4Bits(��Ϣ״̬) + (6Bits(��Ϣ��) + 8Bits(��Ϣֵ))(��ϢID)
//		Changelog	:
///////////////////////////////////////////////////////////////////////////////

#ifndef	__MSGQ_H__
#define __MSGQ_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#pragma pack(2)
/*
 * message group type
 */
typedef enum
{
    /*
     * ��һ����Ϣ:�㲥��Ϣ
     * [0x0000	~	0x00FF]
     */
    //�㲥��Ϣ.����Ϣ
    //[[0x0000	~	0x0000]]
    MSGNULL = 0x000,

#define	MSGBRDCST_CLASS	0x00FF
    //�㲥��Ϣ.����ģʽ
    //[[0x0001	~	0x009F]]
#define	MSGBRDCST_BASE	0x0001
    MSGBRDCST_PWROFF = 0x0001,
    MSGBRDCST_RESET,
    MSGBRDCST_SLEEP,
    MSGBRDCST_SLEEP_DEEP,
    MSGBRDCST_WAKEUP,
    //�㲥��Ϣ.����ģʽ.�û���չ
    //[[0x00A0	~	0x0FF]]
#define	MSGBRDCST_BASE_USREX	0x00A0
    MSGBRDCST_FREQCHG_REQ = MSGBRDCST_BASE_USREX,//ϵͳ��Ƶ�仯����
    MSGBRDCST_FREQCHG_RES,//ϵͳ��Ƶ�仯��Ӧ
    MSGBRDCST_FILL_FIFO_START,//Ϊ����LineIn¼����ʼʱ�Ķ�����InitTask���͵����fifo����Ϣ  ��ʼ
    MSGBRDCST_FILL_FIFO_END,//Ϊ����LineIn¼����ʼʱ�Ķ�����InitTask���͵����fifo����Ϣ  ����

    /*
     * �ڶ�����Ϣ:ϵͳ��Ϣ
     * [0x0100	~	0x0FFF]
     */
#define	MSGSYS_BASE		(0x0100)
#define	MSGSYS_CLASS		(0x01FF)

    //ϵͳ��Ϣ.�ں�
    //[[0x0100	~	0x019F]]
#define	MSGSYS_KERNEL_CLASS	(0x01FF)
    MSGSYS_KERNEL_INITING = 0x0100,
    MSGSYS_KERNEL_INITED,
    MSGSYS_KERNEL_SUSPEND,
    MSGSYS_KERNEL_RESUME,

    //ϵͳ��Ϣ.����
    //[[0x0200	~	0x029F]]
#define	MSGSYS_TASK_CLASS	(0x02FF)
    MSGSYS_TASK_CREATE = 0x0200,
    MSGSYS_TASK_DELETE,
    MSGSYS_TASK_SUSPEND,
    MSGSYS_TASK_RESUME,
    MSGSYS_TASK_SCHEDULE,
	
    //ϵͳ��Ϣ.��ʱ��
    //[[0x0400	~	0x049F]]
#define	MSGSYS_TIMER_CLASS	(0x04FF)
    MSGSYS_TIMER_START = 0x0400,
    MSGSYS_TIMER_STOP,
    MSGSYS_TIMER_CHANGE_PEROID,
    MSGSYS_TIMER_DELETE,

	//�豸��Ϣ.COM UART
	MSGDEV_BUART_CLASS		= 0x0500,
    MSGDEV_BUART_DATRDY,
    MSGDEV_BUART_DATDON,
    MSGDEV_BUART_DATERR,


    //�豸��Ϣ.DEBUG UART
	MSGDEV_FUART_CLASS		= 0x0600,
    MSGDEV_FUART_DATREQ,
    MSGDEV_FUART_DATRDY,


#define	MSGALL_TOTAL_NUM	(0x3FFF + 1)
} msg_type_t, MSGTYPE;
#pragma pack()

#define	MSGPRIO_LEVEL_HI	2
#define	MSGPRIO_LEVEL_MD	1
#define	MSGPRIO_LEVEL_LO	0

extern int msgq_task_register(int pid);
extern void msgq_task_unregister(int pid);

// ��Ϣ����
extern int msgq_msg_recv(void* buf, int* len, int wait);

// ��Ϣ����
extern int msgq_msg_send(int msgcode, void* buf, int len, unsigned char prio, unsigned char async);
extern int msgq_msg_send_fromisr(int msgcode, void* buf, int len, unsigned char prio);

// ������Ϣ��������
extern int msgq_msg_ioctl(int cmd, unsigned int opt);
//
extern int msgq_sleepon_msgqueue(unsigned int wait);

extern int msgq_tmrchk_cbfun(unsigned short msg, unsigned char* buf, int buf_len);

// ע�ᡢע��  ���� ��Ϣ��
// ע�ᵥ��������Ϣ��(������Ϣǰ��ѡ��Ľ����ض���Ϣ��)
extern void MsgAddSet(uint32_t Msg);
// ע������������Ϣ��
extern void MsgDelSet(uint32_t Msg);
// ע�����н�����Ϣ��
extern void MsgEmptySet(void);
// ע��������Ϣ��Ϊ ������Ϣ��
extern void MsgFillSet(void);

// ע�ᡢע�� ��Ϣ�� Ϊ �����ȼ��� �����Ϣ��

// �������Ϣ�������һ��TASK��ע���������Ľ�����Ϣ��������ĳЩ����ʱ��ֻ�������ĳһ��
// ����������ȼ�����Ϣʱ���ɲ��ø���Ϣ��������,��ʱ�Ȳ����������Ϣ���������Ϣ��
// ���ֻ��Ƶ�ȱ���ǣ������Ϣ���������Ϣ���г�ʱ��ʧ��Σ�գ����Դ�������ã�

// ע�ᵥ�������Ϣ��
extern void AltMsgAddSet(uint32_t Msg);
// ע�����������Ϣ��
extern void AltMsgDelSet(uint32_t Msg);
// ע�����������Ϣ��
extern void AltMsgEmptySet(void);
// ע��������Ϣ��Ϊ�����Ϣ��
extern void AltMsgFillSet(void);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__MSGQ_H__
