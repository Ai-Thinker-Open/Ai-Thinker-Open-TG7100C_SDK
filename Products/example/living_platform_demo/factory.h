/*
 *copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _FACTORY_H
#define _FACTORY_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  CODE_FACTORY_WIFI_ON_CONNECT_FAILED  1
#define  CODE_FACTORY_WIFI_ON_CONNECTED       2

int scan_factory_ap(void);
int enter_factory_mode(int8_t rssi);
int exit_factory_mode(void);


#ifdef __cplusplus
}
#endif
#endif	// _FACTORY_H
