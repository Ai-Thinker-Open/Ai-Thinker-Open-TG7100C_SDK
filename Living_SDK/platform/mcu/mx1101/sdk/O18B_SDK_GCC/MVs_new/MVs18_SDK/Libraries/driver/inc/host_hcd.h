/**
 *****************************************************************************
 * @file     host_hcd.h
 * @author   Orson
 * @version  V1.0.0
 * @date     24-June-2013
 * @brief    usb host module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __HOST_HCD_H__
#define	__HOST_HCD_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * Max packet size. Fixed, user should not modify.
 */
#define	HOST_FS_CONTROL_MPS		64
#define	HOST_FS_INT_IN_MPS		64
#define	HOST_FS_BULK_IN_MPS		64
#define	HOST_FS_BULK_OUT_MPS	64
#define	HOST_FS_ISO_OUT_MPS		192
#define	HOST_FS_ISO_IN_MPS		768


/**
 * Endpoint number. Fixed, user should not modify.
 * ��ΪHOSTʱ�������Ķ˵�Ÿ�����˵�ſ��Բ���ͬ
 */
#define	HOST_CONTROL_EP			0
#define	HOST_BULK_IN_EP			1
#define	HOST_BULK_OUT_EP		2
#define	HOST_INT_IN_EP			3
#define	HOST_ISO_OUT_EP			4
#define	HOST_ISO_IN_EP			5


/**
 * pipe type
 */
#define PIPE_TYPE_CONTROL		0
#define PIPE_TYPE_ISO_IN		1
#define PIPE_TYPE_ISO_OUT		2
#define PIPE_TYPE_INT_IN		3
#define PIPE_TYPE_INT_OUT		4
#define PIPE_TYPE_BULK_IN		5
#define PIPE_TYPE_BULK_OUT		6


/**
 * define OTG endpoint descriptor, used for configuration endpoint information
 */
typedef struct _PIPE_INFO
{
	uint8_t	EndpointNum	;	//endpoint number
	uint8_t	PipeType;		//endpoint type: ctrl, bulk, interrupt, isochronous
	uint16_t	MaxPacketSize;	//max packet size

} PIPE_INFO;


/**
 * @brief  usb host init
 * @param  NONE
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbHostOpen(void);

/**
 * @brief  ѡ��ǰUSB�˿�
 * @param  NONE
 * @return NONE
 */
void UsbSetCurrentPort(uint8_t PortNum);

/**
 * @brief  ʹ��USB1�˿ڼ��U������
 * @param  HostEnable: �Ƿ���U������
 * @param  DeviceEnable: �Ƿ���PC����
 * @return NONE
 */
void Usb1SetDetectMode(bool HostEnable, bool DeviceEnable);

/**
 * @brief  ʹ��USB2�˿ڼ��U������
 * @param  HostEnable: �Ƿ���U������
 * @param  DeviceEnable: �Ƿ���PC����
 * @return NONE
 */
void Usb2SetDetectMode(bool HostEnable, bool DeviceEnable);

/**
 * @brief  ���USB1�˿����Ƿ���һ��U���豸����
 * @param  NONE
 * @return 1-��U���豸���ӣ�0-��U���豸����
 */
bool UsbHost1IsLink(void);

/**
 * @brief  ���USB2�˿����Ƿ���һ��U���豸����
 * @param  NONE
 * @return 1-��U���豸���ӣ�0-��U���豸����
 */
bool UsbHost2IsLink(void);

/**
 * @brief  USB���Ӽ�⺯��
 * @brief  �ڶ�ʱ���жϴ�������ÿ�������һ��
 * @param  NONE
 * @return NONE
 */
void OTGLinkCheck(void);

/**
 * @brief  ��鵱ǰѡ���USB�˿����Ƿ���һ��U���豸����
 * @param  NONE
 * @return 1-��U���豸���ӣ�0-��U���豸����
 */
bool UsbHostIsLink(void);

/**
 * @brief  usb���߸�λ
 * @param  NONE
 * @return NONE
 */
void UsbHostPortReset(void);

/**
 * @brief  �ӿ��ƶ˵����һ�ο����������
 * @param  SetupPacket SETUP��ָ��
 * @param  Buf OUT���ݻ�����ָ��
 * @param  Len OUT���ݳ���
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbHostControlOutTransfer(uint8_t* SetupPacket, uint8_t* Buf, uint16_t Len);

/**
 * @brief  �ӿ��ƶ˵����һ�ο������봫��
 * @param  SetupPacket SETUP��ָ��
 * @param  Buf IN���ݻ�����ָ��
 * @param  Len IN���ݳ���
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbHostControlInTransfer(uint8_t* SetupPacket, uint8_t* Buf, uint16_t Len);

/**
 * @brief  ����һ�����ݰ�
 * @param  Pipe pipeָ��
 * @param  Buf �������ݻ�����ָ��
 * @param  Len �������ݳ���
 * @param  TimeOut �������ݳ�ʱʱ�䣬��λΪ����
 * @return ʵ�ʽ��յ������ݳ���
 */
uint16_t UsbHostRcvPacket(PIPE_INFO* Pipe, uint8_t* Buf, uint16_t Len, uint16_t TimeOut);

/**
 * @brief  ����һ�����ݰ�
 * @param  Pipe pipeָ��
 * @param  Buf �������ݻ�����ָ��
 * @param  Len �������ݳ���
 * @param  TimeOut �������ݳ�ʱʱ�䣬��λΪ����
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbHostSendPacket(PIPE_INFO* Pipe, uint8_t* Buf, uint16_t Len, uint16_t TimeOut);

/**
 * @brief  ����ĳ���˵㷢��һ�����ݰ�
 * @brief  �첽��ʽ�������󣬲��ȴ���������
 * @brief  Ӳ�����Զ����Ϸ���OUT���ƣ�ֱ��������һ�����ݰ�
 * @param  Pipe pipeָ��
 * @param  Buf �������ݻ�����ָ��
 * @param  Len �������ݳ���
 * @return NONE
 */
void UsbHostStartSendPacket(PIPE_INFO* Pipe, uint8_t* Buf, uint16_t Len);

/**
 * @brief  �жϷ����Ƿ����
 * @param  Pipe pipeָ��
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbHostIsSendOK(PIPE_INFO* Pipe);

/**
 * @brief  ����ĳ���˵����һ�����ݰ�
 * @brief  �첽��ʽ�������󣬲��ȴ���������
 * @brief  Ӳ�����Զ����Ϸ���IN���ƣ�ֱ�����յ�һ�����ݰ�
 * @param  Pipe pipeָ��
 * @return NONE
 */
void UsbHostStartRcvPacket(PIPE_INFO* Pipe);

/**
 * @brief  HOSTģʽ��ʹ��ĳ���˵��ж�
 * @param  Pipe Pipeָ��
 * @param  Func �жϻص�����ָ��
 * @return NONE
 */
void UsbHostEnableInt(PIPE_INFO* Pipe, FPCALLBACK Func);

/**
 * @brief  HOSTģʽ�½�ֹĳ���˵��ж�
 * @param  Pipe Pipeָ��
 * @return NONE
 */
void UsbHostDisableInt(PIPE_INFO* Pipe);

/**
 * @brief  ����������豸��ַ
 * @param  Address ������豸��ַ
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbHostSetAddress(uint8_t Address);

/**
 * @brief  ��ȡ������
 * @param  Type ����������
 * @param  Index index
 * @param  Buf ���������ջ�����ָ��
 * @param  Size �������󳤶�
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbHostGetDescriptor(uint8_t Type, uint8_t Index, uint8_t* Buf, uint16_t Size);

/**
 * @brief  ��������
 * @param  ConfigurationNum ���ú�
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbHostSetConfiguration(uint8_t ConfigurationNum);

/**
 * @brief  ���ýӿ�
 * @param  InterfaceNum �ӿں�
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbHostSetInterface(uint8_t InterfaceNum);

/**
 * @brief  ����iPod������
 * @param  Current ������
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbHostSetIpodChargeCurrent(int16_t Current);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
