//maintainer lilu
#ifndef __FS_MOUNT_H__
#define	__FS_MOUNT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//
//��ҪĿ��Ϊʵ�ֶ�������ز���
//
#define CONF_DRIVES (2)

//�����ļ�ϵͳ
extern uint8_t FsMount(uint8_t LogicId, FS_CONTEXT * fs);

//�л���ǰĬ�Ϲ������豸
extern uint8_t FsChDrive(uint8_t LogicId);

//��ȡ��ǰĬ�Ϲ������豸����
extern uint8_t FsGetDriveType(void);

//��ȡ�ɹ��ص��߼��豸��
extern int8_t FsGetAvailableLogicId(void);

//��ȡ��ǰ�������ļ�ϵͳ
extern FS_CONTEXT* FsGetCurContext(void);

//ͨ���豸���ͻ�ȡ��ǰ�������ļ�ϵͳ
extern FS_CONTEXT* FsGetContextByDeviceType(uint8_t DeviceType);

//ͨ���豸���ͻ�ȡ��ǰ�������ļ�ϵͳ�߼���
extern int8_t FsGetLogicIdByDeviceType(uint8_t DeviceType);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //_FS_MOUNT_H_
