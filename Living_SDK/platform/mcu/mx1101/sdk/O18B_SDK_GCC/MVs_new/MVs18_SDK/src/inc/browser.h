///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: browser.h
//  maintainer: lilu
///////////////////////////////////////////////////////////////////////////////

#ifndef __BROWSER_H__
#define __BROWSER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//�ļ������ʼ��
bool FileBrowseInit(void);

//�ļ�����¼�����
bool FileBrowseOp(uint16_t Msg);

//�ļ������ȡѡ��Ľ��	
bool FileBrowseGetResult(FOLDER** Folder, uint16_t* FileNumInFolder, uint16_t* FileNumInDisk);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif




