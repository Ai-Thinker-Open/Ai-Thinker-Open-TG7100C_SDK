/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifdef EN_COMBO_NET

#ifndef __COMBO_NET_H__
#define __COMBO_NET_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#define COMBO_EVT_CODE_AP_INFO         0x0001
#define COMBO_EVT_CODE_RESTART_ADV     0x0002

// Combo device AP connect status
#define COMBO_AP_DISCONNECTED          0        // ap connected
#define COMBO_AP_CONNECTED             1        // ap disconnected
#define COMBO_AP_CONN_UNINIT           2        // combo device not connect ap yet

// Combo awss silent adv flag
#define COMBO_AWSS_NORMAL              0        // ap configuration is not required
#define COMBO_AWSS_SILENT              1        // ap configuration is required

typedef int (* combo_event_cb)(uint16_t evt_code);

int combo_net_init(void);
int combo_net_deinit(void);
uint8_t combo_ble_conn_state(void);
void combo_set_cloud_state(uint8_t cloud_connected);
void combo_set_ap_state(uint8_t ap_connected);
void combo_set_awss_state(uint8_t awss_running);
void combo_ap_conn_notify(void);
void combo_token_report_notify(void);

int32_t aiot_ais_report_log(const char *fmt, ...);


#endif
#endif
