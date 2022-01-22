/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __YLOOP_WIFI_EVT_H
#define __YLOOP_WIFI_EVT_H

#include "aos/yloop.h"

#ifdef __cplusplus
extern "C" {
#endif

/* WiFi event */
#ifndef EV_WIFI
#define  EV_WIFI                  0x0002
#endif

#ifndef CODE_WIFI_ON_INIT_DONE
#define  CODE_WIFI_ON_INIT_DONE   1
#endif

#ifndef CODE_WIFI_ON_MGMR_DONE
#define  CODE_WIFI_ON_MGMR_DONE   2
#endif

#ifndef CODE_WIFI_CMD_RECONNECT
#define  CODE_WIFI_CMD_RECONNECT  3
#endif

#ifndef CODE_WIFI_ON_CONNECTED
#define  CODE_WIFI_ON_CONNECTED   4
#endif

#ifndef CODE_WIFI_ON_DISCONNECT
#define  CODE_WIFI_ON_DISCONNECT  5
#endif

#ifndef CODE_WIFI_ON_PRE_GOT_IP
#define  CODE_WIFI_ON_PRE_GOT_IP  6
#endif

#ifndef CODE_WIFI_ON_GOT_IP
#define  CODE_WIFI_ON_GOT_IP      7
#endif

#ifndef CODE_WIFI_ON_CONNECTING
#define  CODE_WIFI_ON_CONNECTING  8
#endif

#ifndef CODE_WIFI_ON_SCAN_DONE
#define  CODE_WIFI_ON_SCAN_DONE   9
#endif

#ifndef CODE_WIFI_ON_SCAN_DONE_ONJOIN
#define  CODE_WIFI_ON_SCAN_DONE_ONJOIN  10
#endif

#ifndef CODE_WIFI_ON_AP_STARTED
#define  CODE_WIFI_ON_AP_STARTED        11
#endif

#ifndef CODE_WIFI_ON_AP_STOPPED
#define  CODE_WIFI_ON_AP_STOPPED        12
#endif

#ifndef CODE_WIFI_ON_PROV_SSID
#define  CODE_WIFI_ON_PROV_SSID         13
#endif

#ifndef CODE_WIFI_ON_PROV_BSSID
#define  CODE_WIFI_ON_PROV_BSSID        14
#endif

#ifndef CODE_WIFI_ON_PROV_PASSWD
#define  CODE_WIFI_ON_PROV_PASSWD       15
#endif

#ifndef CODE_WIFI_ON_PROV_CONNECT
#define  CODE_WIFI_ON_PROV_CONNECT      16
#endif

#ifndef CODE_WIFI_ON_PROV_DISCONNECT
#define  CODE_WIFI_ON_PROV_DISCONNECT   17
#endif

#ifndef CODE_WIFI_ON_PROV_SCAN_START
#define  CODE_WIFI_ON_PROV_SCAN_START   18
#endif

#ifndef CODE_WIFI_ON_PROV_STATE_GET
#define  CODE_WIFI_ON_PROV_STATE_GET    19
#endif

#ifndef CODE_WIFI_ON_MGMR_DENOISE
#define  CODE_WIFI_ON_MGMR_DENOISE      20
#endif

#ifndef CODE_WIFI_ON_AP_STA_ADD
#define  CODE_WIFI_ON_AP_STA_ADD        21
#endif

#ifndef CODE_WIFI_ON_AP_STA_DEL
#define  CODE_WIFI_ON_AP_STA_DEL        22
#endif

#ifndef CODE_WIFI_ON_EMERGENCY_MAC
#define  CODE_WIFI_ON_EMERGENCY_MAC     23
#endif

#ifdef __cplusplus
}
#endif

#endif /* __YLOOP_WIFI_EVT_H */

