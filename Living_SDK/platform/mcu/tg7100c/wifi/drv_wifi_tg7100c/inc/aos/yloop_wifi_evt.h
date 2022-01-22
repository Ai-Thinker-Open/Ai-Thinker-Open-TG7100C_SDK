/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __YLOOP_WIFI_EVT_H
#define __YLOOP_WIFI_EVT_H

#include "aos/yloop.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  TG7100C_EV_WIFI                0x0010

#define  CODE_WIFI_ON_INIT_DONE         7
#define  CODE_WIFI_ON_MGMR_DONE         8
#define  CODE_WIFI_ON_CONNECTING        9
#define  CODE_WIFI_ON_SCAN_DONE         10
#define  CODE_WIFI_ON_AP_STARTED        11
#define  CODE_WIFI_ON_AP_STOPPED        12
#define  CODE_WIFI_ON_PROV_SSID         13
#define  CODE_WIFI_ON_PROV_BSSID        14
#define  CODE_WIFI_ON_PROV_PASSWD       15
#define  CODE_WIFI_ON_PROV_CONNECT      16
#define  CODE_WIFI_ON_PROV_DISCONNECT   17
#define  CODE_WIFI_ON_PROV_SCAN_START   18
#define  CODE_WIFI_ON_PROV_STATE_GET    19
#define  CODE_WIFI_ON_MGMR_DENOISE      20
#define  CODE_WIFI_ON_AP_STA_ADD        21
#define  CODE_WIFI_ON_AP_STA_DEL        22
#define  CODE_WIFI_ON_EMERGENCY_MAC     23
#define  CODE_WIFI_ON_SCAN_DONE_ONJOIN  24

/* Network Event */
#define EV_NETWORK EV_WIFI
#define CODE_ON_DISCONNECT CODE_WIFI_ON_DISCONNECT
#define CODE_ON_GOT_IP CODE_WIFI_ON_GOT_IP

#ifdef __cplusplus
}
#endif

#endif /* __YLOOP_WIFI_EVT_H */

