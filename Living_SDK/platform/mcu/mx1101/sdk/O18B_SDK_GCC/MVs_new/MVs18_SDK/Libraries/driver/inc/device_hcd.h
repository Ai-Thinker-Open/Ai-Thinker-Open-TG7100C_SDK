/**
 *****************************************************************************
 * @file     device_hcd.h
 * @author   Orson
 * @version  V1.0.0
 * @date     24-June-2013
 * @brief    device module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __DEVICE_HCD_H__
#define	__DEVICE_HCD_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

// Max packet size. Fixed, user should not modify.
#define	DEVICE_FS_CONTROL_MPS		64
#define	DEVICE_FS_INT_IN_MPS		64
#define	DEVICE_FS_BULK_IN_MPS		64
#define	DEVICE_FS_BULK_OUT_MPS		64
#define	DEVICE_FS_ISO_IN_MPS		192
#define	DEVICE_FS_ISO_OUT_MPS		192


// Endpoint number. Fixed, user should not modify.
#define	DEVICE_CONTROL_EP			0x00
#define	DEVICE_INT_IN_EP			0x81
#define	DEVICE_BULK_IN_EP			0x82
#define	DEVICE_BULK_OUT_EP			0x03
#define	DEVICE_ISO_IN_EP			0x84
#define	DEVICE_ISO_OUT_EP			0x05


// Bus event define
#define	USB_VBUS_ERROR				(1 << 7)
#define	USB_SESSION_REQ				(1 << 6)
#define	USB_DISCONNECT				(1 << 5)
#define	USB_CONNECT					(1 << 4)
#define	USB_SOF						(1 << 3)
#define	USB_RESET					(1 << 2)
#define	USB_RESUME					(1 << 1)
#define	USB_SUSPEND					(1 << 0)

#define READER_UNREADY              0 // δ����
#define READER_INIT                 1 // ��ʼ��
#define READER_READY                2 // ����
#define READER_READ                 3 // ������
#define READER_WIRTE                4 // д����
#define READER_INQUIRY              5 // ��ѯ��������Ϣ
#define READER_READ_FORMAT_CAPACITY 6 // ��ѯ��ʽ������
#define READER_READ_CAPACITY        7 // ��ѯ��������
#define READER_NOT_ALLOW_REMOVAL    8 // ������ɾ���豸
#define READER_ALLOW_REMOVAL        9 // ����ɾ���豸
extern uint8_t GetSdReaderState(void);

/**
 * @brief  ���USB1�˿����Ƿ���һ��PC����
 * @param  NONE
 * @return 1-��PC���ӣ�0-��PC����
 */
bool UsbDevice1IsLink(void);

/**
 * @brief  ���USB2�˿����Ƿ���һ��PC����
 * @param  NONE
 * @return 1-��PC���ӣ�0-��PC����
 */
bool UsbDevice2IsLink(void);

/**
 * @brief  ��鵱ǰѡ���USB�˿����Ƿ���һ��PC����
 * @param  NONE
 * @return 1-��PC���ӣ�0-��PC����
 */
bool UsbDeviceIsLink(void);

/**
 * @brief  Device initial
 * @param  NONE
 * @return NONE
 */
void UsbDeviceInit(void);

/**
 * @brief  Device software disconnect.
 * @param  NONE
 * @return NONE
 */
void UsbDeviceDisConnect(void);

/**
 * @brief  ����STALLӦ��
 * @param  EndpointNum �˵��
 * @return NONE
 */
void UsbDeviceSendStall(uint8_t EndpointNum);

/**
 * @brief  ��λĳ���˵�
 * @param  EndpointNum �˵��
 * @return NONE
 */
void UsbDeviceResetEndpoint(uint8_t EndpointNum);

/**
 * @brief  �����豸��ַ
 * @brief  ���߸�λ�������豸��ַΪ0����������PC�˷���SetAddress��������ָ�����豸��ַ
 * @param  Address �豸��ַ
 * @return NONE
 */
void UsbDeviceSetAddress(uint8_t Address);

/**
 * @brief  ��ȡ�����ϵ��¼�
 * @param  NONE
 * @return �����¼����룬USB_RESET-���߸�λ�¼���...
 */
uint8_t UsbDeviceGetBusEvent(void);

/**
 * @brief  �ӿ��ƶ˵����SETUP����
 * @param  Buf ���ݻ�����ָ��
 * @param  MaxLen ������ݳ��ȣ�һ��SETUP���ݰ����ȶ���8�ֽ�
 * @return ʵ�ʽ��յ������ݳ���
 */
uint8_t UsbDeviceSetupRcv(uint8_t* Buf, uint8_t MaxLen);

/**
 * @brief  �ӿ��ƶ˵㷢������
 * @param  Buf ���ݻ�����ָ��
 * @param  Len ���ݳ���
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbDeviceControlSend(uint8_t* Buf, uint16_t Len);

/**
 * @brief  �ӿ��ƶ˵����OUT����
 * @param  Buf ���ݻ�����ָ��
 * @param  MaxLen ������ݳ���
 * @return ʵ�ʽ��յ������ݳ���
 */
uint8_t UsbDeviceControlRcv(uint8_t* Buf, uint16_t MaxLen);

/**
 * @brief  ��INT IN�˵㷢������
 * @param  Buf ���ݻ�����ָ��
 * @param  Len ���ݳ��ȣ����ܴ���DEVICE_FS_BULK_IN_MPS�ֽ�
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbDeviceIntSend(uint8_t* Buf, uint16_t Len);

/**
 * @brief  ��BULK IN�˵㷢������
 * @param  Buf ���ݻ�����ָ��
 * @param  Len ���ݳ���
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbDeviceBulkSend(uint8_t* Buf, uint16_t Len);

/**
 * @brief  ��BULK OUT�˵��������
 * @param  Buf ���ݻ�����ָ��
 * @param  MaxLen ������ݳ���
 * @return ʵ�ʽ��յ������ݳ���
 */
uint16_t UsbDeviceBulkRcv(uint8_t* Buf, uint16_t MaxLen);

/**
 * @brief  ��ISO IN�˵㷢������
 * @param  Buf ���ݻ�����ָ��
 * @param  Len ���ݳ��ȣ����ܴ���DEVICE_FS_ISO_IN_MPS�ֽ�
 * @return 1-�ɹ���0-ʧ��
 */
bool UsbDeviceIsoSend(uint8_t* Buf, uint16_t Len);

/**
 * @brief  ��ISO OUT�˵��������
 * @param  Buf ���ݻ�����ָ��
 * @param  MaxLen ������ݳ���
 * @return ʵ�ʽ��յ������ݳ���
 */
uint16_t UsbDeviceIsoRcv(uint8_t* Buf, uint16_t MaxLen);

/**
 * @brief  DEVICEģʽ��ʹ��ĳ���˵��ж�
 * @param  Pipe Pipeָ��
 * @param  Func �жϻص�����ָ��
 * @return NONE
 */
void UsbDeviceEnableInt(uint8_t EpNum, FPCALLBACK Func);

/**
 * @brief  DEVICEģʽ�½�ֹĳ���˵��ж�
 * @param  EpNum �˵��
 * @return NONE
 */
void UsbDeviceDisableInt(uint8_t EpNum);

/**
 * @brief  ʹ��SOF��ʱ��⡣Ĭ��SOF��ʱ���ʹ�ܡ�
 * @param  
 * @return NONE
 */
void EnUsbSofTimeOut(bool En);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
