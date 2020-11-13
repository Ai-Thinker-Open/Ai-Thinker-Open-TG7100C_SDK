/**
 ****************************************************************************************
 *
 * @file wifi_mgmr_event.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <aos/yloop.h>
#include <yloop_extend.h>

#include "bl_main.h"
#include "wifi_mgmr.h"
#include "wifi_mgmr_api.h"
#include "wifi_mgmr_event.h"

static void cb_connect_ind(void *env, struct wifi_event_sm_connect_ind *ind)
{
    wifi_mgmr_msg_t msg_wifi;

    memset(&msg_wifi, 0, sizeof(msg_wifi));
    msg_wifi.ev = ind->status_code ? WIFI_MGMR_EVENT_FW_IND_DISCONNECT : WIFI_MGMR_EVENT_FW_IND_CONNECTED;
    msg_wifi.data1 = (void*)0x11223344;
    msg_wifi.data2 = (void*)0x55667788;
    msg_wifi.len = sizeof(msg_wifi);
    wifi_mgmr_set_connect_stat_info(ind, WIFI_MGMR_CONNECT_IND_STAT_INFO_TYPE_IND_CONNECTION);
    wifi_mgmr_event_notify(&msg_wifi);
}

static void cb_disconnect_ind(void *env, struct wifi_event_sm_disconnect_ind *ind)
{
    wifi_mgmr_msg_t msg_wifi;

    memset(&msg_wifi, 0, sizeof(msg_wifi));
    printf("sending disconnect\r\n");
    msg_wifi.ev = WIFI_MGMR_EVENT_FW_IND_DISCONNECT;
    msg_wifi.data1 = (void*)0x11223344;
    msg_wifi.data2 = (void*)0x55667788;
    msg_wifi.len = sizeof(msg_wifi);
    wifiMgmr.wifi_mgmr_stat_info.type_ind = WIFI_MGMR_CONNECT_IND_STAT_INFO_TYPE_IND_DISCONNECTION;
    wifiMgmr.wifi_mgmr_stat_info.status_code = ind->reason_code;
    wifi_mgmr_event_notify(&msg_wifi);
}

static void cb_beacon_ind(void *env, struct wifi_event_beacon_ind *ind)
{
    wifi_mgmr_api_cipher_t rsn_ucstCipher, rsn_mcstCipher;
    memcpy(&rsn_ucstCipher, &ind->rsn_ucstCipher, sizeof(wifi_cipher_t));
    memcpy(&rsn_mcstCipher, &ind->rsn_mcstCipher, sizeof(wifi_cipher_t));
    wifi_mgmr_api_scan_item_beacon(ind->channel, ind->rssi, ind->auth, ind->bssid, ind->ssid, ind->ssid_len, ind->ppm_abs, ind->ppm_rel, rsn_ucstCipher, rsn_mcstCipher);
}

static void cb_probe_resp_ind(void *env, long long timestamp)
{
    //printf("timestamp = 0x%llx\r\n", timestamp);
}

static void cb_rssi_ind(void *env, int8_t rssi)
{
    wifiMgmr.wlan_sta.sta.rssi = rssi;
}

static void cb_event_ind(void *env, struct wifi_event *event)
{
    switch (event->id) {
        case WIFI_EVENT_ID_IND_CHANNEL_SWITCH:
        {
            struct wifi_event_data_ind_channel_switch *ind;

            ind = (struct wifi_event_data_ind_channel_switch*)event->data;
            wifiMgmr.channel = ind->channel;
            //TODO it seems channel is strange got from fw. Fixit
            printf("[WIFI] [IND] Channel is %d\r\n", wifiMgmr.channel);
        }
        break;
        case WIFI_EVENT_ID_IND_SCAN_DONE:
        {
            struct wifi_event_data_ind_scan_done *ind;

            ind = (struct wifi_event_data_ind_scan_done*)event->data;
            (void) ind;
            puts("[WIFI] [IND] SCAN Done\r\n");
            wifi_mgmr_scan_complete_notify();
            aos_post_event(EV_WIFI, CODE_WIFI_ON_SCAN_DONE, WIFI_SCAN_DONE_EVENT_OK);
        }
        break;
        default:
        {
            printf("----------------UNKNOWN WIFI EVENT %d-------------------\r\n", (int)event->id);
        }
    }
}

int wifi_mgmr_event_init(void)
{
    bl_rx_sm_connect_ind_cb_register(NULL, cb_connect_ind);
    bl_rx_sm_disconnect_ind_cb_register(NULL, cb_disconnect_ind);
    bl_rx_beacon_ind_cb_register(NULL, cb_beacon_ind);
    bl_rx_probe_resp_ind_cb_register(NULL, cb_probe_resp_ind);
    bl_rx_rssi_cb_register(NULL, cb_rssi_ind);
    bl_rx_event_register(NULL, cb_event_ind);
    return 0;
}
