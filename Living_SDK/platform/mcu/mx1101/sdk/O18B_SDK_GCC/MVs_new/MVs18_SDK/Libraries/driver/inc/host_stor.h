/**
 *****************************************************************************
 * @file     host_stor.h
 * @author   Orson
 * @version  V1.0.0
 * @date     24-June-2013
 * @brief    host mass-storage module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __HOST_STOR_H__
#define	__HOST_STOR_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define		MAX_USB_SN_LEN		50


typedef struct _USB_INFO
{
	uint16_t	VID;
	uint16_t	PID;
	uint8_t	LenSN;	//length of SN
	uint8_t  	SN[MAX_USB_SN_LEN];	//Serial number, end with "\0\0"

} USB_INFO;
extern USB_INFO		gLibUsbInfo;

//�����ض�VID���豸����֧�ֵ���U����
//Ĭ��gMassStorInvalidVidΪ0x0000���������豸������U��
//��gMassStorInvalidVid����Ϊ0x05AC������Խ�ֹ��APPLE�豸����U��������
extern uint16_t	gMassStorInvalidVid;


/**
 * @brief  usb host device init
 * @param  NONE
 * @return 1-�ɹ���0-ʧ��
 */
bool HostStorInit(void);

/**
 * @brief  read blocks
 * @param  BlockNum block number
 * @param  Buf ��������ָ��
 * @param  BlockCnt Ҫ����block����
 * @return 1-�ɹ���0-ʧ��
 */
bool HostStorReadBlock(uint32_t BlockNum, void* Buf, uint8_t BlockCnt);

/**
 * @brief  write blocks
 * @param  BlockNum block number
 * @param  Buf д������ָ��
 * @param  BlockCnt Ҫд��block����
 * @return 1-�ɹ���0-ʧ��
 */
bool HostStorWriteBlock(uint32_t BlockNum, void* Buf, uint8_t BlockCnt);

/**
 * @brief  get storage device block size
 * @param  NONE
 * @return block size
 */
uint16_t HostStorGetBlockSize(void);

/**
 * @brief  get storage device last lba number
 * @param  NONE
 * @return last block number
 */
uint32_t HostStorGetLastLBA(void);


void SetDriverTerminateFuc(TerminateFunc func);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
