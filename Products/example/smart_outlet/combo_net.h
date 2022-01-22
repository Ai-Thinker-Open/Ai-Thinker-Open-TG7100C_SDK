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

// Combo device AP config need or not
#define COMBO_AWSS_NOT_NEED            0        // ap configuration is not required
#define COMBO_AWSS_NEED                1        // ap configuration is required

typedef int (* combo_event_cb)(uint16_t evt_code);

int combo_net_init(void);
int combo_net_deinit(void);
uint8_t combo_ble_conn_state(void);
void combo_set_cloud_state(uint8_t cloud_connected);
void combo_set_ap_state(uint8_t ap_connected);
void combo_set_awss_state(uint8_t awss_running);
void combo_ap_conn_notify(void);
void combo_token_report_notify(void);


typedef enum {
    AIS_AWSS_STATUS_AP_CONNECTED        = 1,        /* 用于上报配网成功状态 */
    AIS_AWSS_STATUS_AP_CONNECT_FAILED   = 2,        /* 用于上报配网失败状态 */
    AIS_AWSS_STATUS_TOKEN_REPORTED      = 3,        /* 用于上报token上报成功状态 */
    AIS_AWSS_STATUS_PROGRESS_REPORT     = 4,        /* 用于上报配网过程状态 */
    AIS_AWSS_STATUS_MAXIMUM             = 5,        /* 不可用 */
} ais_awss_status_t;

typedef enum {
    AIS_AWSS_FATALERR_SSID_NOT_EXIST    = 50400,
    AIS_AWSS_FATALERR_SSID_SIGNAL_LOW   = 50401,
    AIS_AWSS_FATALERR_AUTH_TIMEOUT      = 50402,
    AIS_AWSS_FATALERR_CONNECT_AP_FAILED = 50403,
} ais_awss_fatal_error_substatus_t;

typedef enum {
    AIS_AWSS_PROGRESS_CONNECT_AP_START      = 0x0001,
    AIS_AWSS_PROGRESS_GET_IP_START          = 0x0002,
    AIS_AWSS_PROGRESS_CONNECT_MQTT_START    = 0x0003,

    AIS_AWSS_PROGRESS_CONNECT_AP_SUCCESS    = 0x0004,
    AIS_AWSS_PROGRESS_GET_IP_SUCCESS        = 0x0005,
    AIS_AWSS_PROGRESS_CONNECT_MQTT_SUCCESS  = 0x0006,

    AIS_AWSS_PROGRESS_CONNECT_AP_FAILED     = 0x0100,
    AIS_AWSS_PROGRESS_GET_IP_FAILED         = 0x0200,
    AIS_AWSS_PROGRESS_CONNECT_MQTT_FAILED   = 0x0300,
} ais_awss_progress_substatus_t;

int32_t aiot_ais_report_awss_status(ais_awss_status_t status, uint16_t subcode);

int32_t aiot_ais_report_log(const char *fmt, ...);


#endif
#endif
