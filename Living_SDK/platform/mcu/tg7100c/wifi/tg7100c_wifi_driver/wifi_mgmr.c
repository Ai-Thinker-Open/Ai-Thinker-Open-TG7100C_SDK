/**
 ****************************************************************************************
 *
 * @file wifi_mgmr.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <lwip/netifapi.h>
#include <aos/yloop.h>
#include <yloop_extend.h>
#include <tg710x_fw_api.h>
#include "bl_main.h"
#include "wifi_mgmr.h"
#include "wifi_mgmr_profile.h"
#include "wifi_mgmr_api.h"
#include "wifi_mgmr_event.h"
#include "wifi_netif.h"
#include "include/wifi_mgmr_ext.h"
#include "os_hal.h"
#include "hal_sys.h"
#include <bl_adc.h>
#include <hal/wifi.h>

#include <blog.h>
#define USER_UNUSED(a) ((void)(a))

#define DEBUG_HEADER "[WF][SM] "
#define mgmr_TASK_PRIORITY     (4)
#define TSEN_RELOAD_MS         (10000)

wifi_mgmr_t wifiMgmr;
const static struct state
    stateGlobal, 
    stateIdle,
    stateConnecting,
    stateConnectedIPNo, stateConnectedIPYes,
    stateDisconnect,
    stateIfaceDown,
    stateSniffer,
    stateError;

extern void NetCallback(hal_wifi_ip_stat_t *pnet);
static void printErrMsg( void *stateData, struct event *event )
{
   os_printf( "ENTERED ERROR STATE!" );
}

static bool stateGlobalGuard( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    USER_UNUSED(msg);
    msg = event->data;
    os_printf(DEBUG_HEADER "%s:event is 0x%08X\r\n", __func__, msg->ev);
    return false;
}

char *wifi_mgmr_auth_to_str(uint8_t auth)
{
    switch (auth) {
        case WIFI_EVENT_BEACON_IND_AUTH_OPEN:
        {
            return "Open";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WEP:
        {
            return "WEP";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK:
        {
            return "WPA/WPA2-PSK";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT:
        {
            return "WPA/WPA2-Enterprise";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN:
        {
            return "Unknown";
        }
        break;
        default: 
        {
            return "Unknown";
        }
    }
}

char *wifi_mgmr_cipher_to_str(wifi_mgmr_cipher_t cipher)
{
    if (cipher.ccmp == 1) {
        return "ccmp";
    } else if (cipher.tkip == 1) {
        return "tkip";
    } else if (cipher.wep40 == 1) {
        return "wep40";
    } else if (cipher.wep104 == 1) {
        return "wep104";
    } else{
        return "open";
    }
}

static bool stateGlobalGuard_scan_beacon( void *ch, struct event *event )
{
#define SCAN_UPDATE_LIMIT_TIME_MS (3000)

    int i, empty = -1, oldest = -1;
    uint32_t lastseen = 0xFFFFFFFF;
    uint32_t counter = 0;
    uint32_t lastseen_found = 0;
    wifi_mgmr_msg_t *msg;
    wifi_mgmr_scan_item_t *scan;

    msg = event->data;
    scan = (wifi_mgmr_scan_item_t*)msg->data;
    if (ch != (void*)msg->ev) {
        return false;
    }
#ifdef DEBUG_SCAN_BEACON
    os_printf(DEBUG_HEADER "channel %02u, bssid %02X:%02X:%02X:%02X:%02X:%02X, rssi %3d, auth %s, pair_key:%s, group_key:%s \t, SSID %s\r\n",
            scan->channel,
            scan->bssid[0],
            scan->bssid[1],
            scan->bssid[2],
            scan->bssid[3],
            scan->bssid[4],
            scan->bssid[5],
            scan->rssi,
            scan->ppm_abs,
            scan->ppm_rel,
            wifi_mgmr_auth_to_str(scan->auth),
            wifi_mgmr_cipher_to_str(scan->rsn_ucstCipher),
            wifi_mgmr_cipher_to_str(scan->rsn_mcstCipher),
            scan->ssid
    );
#endif
    if (scan->channel > wifiMgmr.channel_nums || !scan->channel){
        goto __exit;
    }
    if (0 == scan->ssid[0]) {
        goto __exit;
    }

    /*update scan_items, we just store the newly found item, or update exsiting one*/
    counter = os_tick_get();
    for (i = 0; i < sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0]); i++) {
        if(wifiMgmr.scan_items[i].channel > wifiMgmr.channel_nums){
            memset(&wifiMgmr.scan_items[i], 0, sizeof(wifi_mgmr_scan_item_t));
            wifiMgmr.scan_items[i].is_used = 0;
        }

        if (wifiMgmr.scan_items[i].is_used) {
            /*track the oldest scan_item*/
            if ((0 == lastseen_found) ||
                ((int32_t)wifiMgmr.scan_items[i].timestamp_lastseen - (int32_t)lastseen < 0)) {
                lastseen_found = 1;
                lastseen = wifiMgmr.scan_items[i].timestamp_lastseen;
                oldest = i;
            }

            /*bssid and ssid must be the same at the same time*/
            if (0 == memcmp(wifiMgmr.scan_items[i].bssid, scan->bssid, sizeof(scan->bssid)) &&
                    0 == strcmp(scan->ssid, wifiMgmr.scan_items[i].ssid)) {

                /*exactly the same scan item found*/
                if ((scan->rssi < wifiMgmr.scan_items[i].rssi) &&
                    ((int32_t)os_tick_get() - (int32_t)wifiMgmr.scan_items[i].timestamp_lastseen < SCAN_UPDATE_LIMIT_TIME_MS)) {

                    //blog_info("skip update %s with rssi %d\r\n", scan->ssid, scan->rssi);

                } else {

                    wifiMgmr.scan_items[i].channel = scan->channel;
                    wifiMgmr.scan_items[i].rssi = scan->rssi;
                    wifiMgmr.scan_items[i].ppm_abs = scan->ppm_abs;
                    wifiMgmr.scan_items[i].ppm_rel = scan->ppm_rel;
                    wifiMgmr.scan_items[i].timestamp_lastseen = counter;
                    wifiMgmr.scan_items[i].auth = scan->auth;
                    memcpy(&wifiMgmr.scan_items[i].rsn_mcstCipher, &scan->rsn_mcstCipher, sizeof(wifi_mgmr_cipher_t));
                    memcpy(&wifiMgmr.scan_items[i].rsn_ucstCipher, &scan->rsn_ucstCipher, sizeof(wifi_mgmr_cipher_t));
                }
                break;
            }
        } else {
            empty  = i;
        }
    }
    if (i == sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0])) {
        /*no valid item found in database, so try to store this newly found*/
        i = (-1 != empty) ? empty : oldest;
        if (-1 != i) {
            memset(&wifiMgmr.scan_items[i], 0, sizeof(wifiMgmr.scan_items[0]));
            strncpy(wifiMgmr.scan_items[i].ssid, scan->ssid, sizeof(wifiMgmr.scan_items[0].ssid));
            wifiMgmr.scan_items[i].ssid_tail[0] = '\0';
            wifiMgmr.scan_items[i].ssid_len = strlen(wifiMgmr.scan_items[i].ssid);
            memcpy(wifiMgmr.scan_items[i].bssid, scan->bssid, sizeof(wifiMgmr.scan_items[i].bssid));
            wifiMgmr.scan_items[i].channel = scan->channel;
            wifiMgmr.scan_items[i].rssi = scan->rssi;
            wifiMgmr.scan_items[i].timestamp_lastseen = counter;
            wifiMgmr.scan_items[i].is_used = 1;
        }
    }

__exit:

    /*we always return false, since we only store the info from beacon frame*/
    return false;
}

static bool stateGlobalGuard_disable_autoreconnect( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    if (&stateDisconnect == wifiMgmr.m.currentState) {
        os_printf("Disable Autoreconnect in Disconnec State\r\n");
        os_printf(DEBUG_HEADER "Removing STA interface...\r\n");
        bl_main_if_remove(wifiMgmr.wlan_sta.vif_index);
        return true;
    }
    /*we need set disable now for future use*/
    os_printf("Disable Auto Reconnect\r\n");
    wifi_mgmr_profile_autoreconnect_disable(&wifiMgmr, -1);

    /*we always return false, since we only store the info from beacon frame*/
    return false;
}

static bool stateGlobalGuard_enable_autoreconnect( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    /*we need set enable now for future use*/
    os_printf("Enable Auto Reconnect\r\n");
    wifi_mgmr_profile_autoreconnect_enable(&wifiMgmr, -1);

    /*we always return false, since we only store the info from beacon frame*/
    return false;
}

static bool stateGlobalGuard_fw_disconnect(void *ch, struct event *event)
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (WIFI_MGMR_EVENT_FW_DISCONNECT == msg->ev) {
        os_printf("Disconnect CMD\r\n");
        bl_main_disconnect();
    }

    return false;
}

static bool stateGlobalGuard_fw_powersaving(void *ch, struct event *event)
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (WIFI_MGMR_EVENT_FW_POWERSAVING == msg->ev) {
        os_printf("------>>>>>> Powersaving CMD, mode: %u\r\n", (unsigned int)msg->data1);
//TODO mode check?
        bl_main_powersaving((int)msg->data1);
    }

    return false;
}

static bool stateGlobalGuard_fw_scan(void *ch, struct event *event)
{
    wifi_mgmr_msg_t *msg;

    if (&stateIdle != wifiMgmr.m.currentState &&
            &stateConnectedIPYes != wifiMgmr.m.currentState &&
            &stateSniffer != wifiMgmr.m.currentState) {
            os_printf("------>>>>>> FW busy\r\n");
            aos_post_event(EV_WIFI, CODE_WIFI_ON_SCAN_DONE, WIFI_SCAN_DONE_EVENT_BUSY);
            return false;
    }

    msg = event->data;
    if (WIFI_MGMR_EVENT_FW_SCAN == msg->ev) {
        os_printf("------>>>>>> Scan CMD\r\n");
        bl_main_scan();
    }

    return false;
}

static void stateGlobalAction( void *oldStateData, struct event *event,
      void *newStateData )
{
    os_printf(DEBUG_HEADER "Global Action\r\n");
}

static void stateGlobalEnter( void *stateData, struct event *event )
{
   os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
}

static void stateGlobalExit( void *stateData, struct event *event )
{
   os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
}

/*function for state sniffer*/
static bool stateSnifferGuard( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch == (void*)msg->ev) {
        return true;
    }
    return false;
}

static void stateSnifferAction( void *oldStateData, struct event *event,
      void *newStateData )
{
    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
}

static bool stateSnifferGuard_ChannelSet( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    bl_main_monitor_channel_set((int)msg->data1, (int)msg->data2);
    /*will never trigger state change, since we just want to trigger the guard*/
    return false;
}

static bool stateSnifferGuard_raw_send(void *ch, struct event *event)
{
    wifi_mgmr_msg_t *msg;
    uint8_t *pkt;
    int len;

    if ((&stateIdle) == wifiMgmr.m.currentState || (&stateIfaceDown) == wifiMgmr.m.currentState) {
        /* NO Raw Send in IDLE mode */
        return false;
    }

    msg = event->data;
    if (WIFI_MGMR_EVENT_FW_DATA_RAW_SEND == msg->ev) {
        pkt = msg->data1;
        len = (int)msg->data2;
        //blog_info("------>>>>>> RAW Send CMD, pkt %p, len %d\r\n", pkt, len);
        bl_main_raw_send(pkt, len);
    }

    return false;
}

static void stateSnifferEnter( void *stateData, struct event *event )
{
    //bl60x_fw_xtal_capcode_autofit();
    os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
}

static void stateSnifferExit( void *stateData, struct event *event )
{
    //bl60x_fw_xtal_capcode_restore();
    os_printf(DEBUG_HEADER "Exiting %s state\r\n", (char *)stateData);
}

/*function for state CONNECTING*/
static bool stateConnectingGuard( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch == (void*)msg->ev) {
        return true;
    }
    return false;
}

static void stateConnectingAction_connected( void *oldStateData, struct event *event,
      void *newStateData )
{
    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
}

static void stateConnectingAction_disconnect( void *oldStateData, struct event *event,
      void *newStateData )
{
    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
}

static void stateConnectingEnter( void *stateData, struct event *event )
{
   os_printf(DEBUG_HEADER "Entering %s state dsaada\r\n", (char *)stateData);
   aos_post_event(EV_WIFI, CODE_WIFI_ON_CONNECTING, 0);
}

static void stateConnectingExit( void *stateData, struct event *event )
{
   os_printf(DEBUG_HEADER "Exiting %s state\r\n", (char *)stateData);
}

static bool stateGlobalGuard_AP(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;
    wifi_mgmr_ap_msg_t *ap;

    msg = event->data;
    if (WIFI_MGMR_EVENT_APP_AP_START != msg->ev) {
        return false;
    }

    if (bl_main_if_add(0, &(wifiMgmr.wlan_ap.netif), &(wifiMgmr.wlan_ap.vif_index))) {
        os_printf(DEBUG_HEADER "%s: add AP iface failed\r\n", __func__);
        return false;
    }
    netifapi_netif_set_link_up(&(wifiMgmr.wlan_ap.netif));
void dhcpd_start(struct netif *netif);
    netifapi_netif_common(&(wifiMgmr.wlan_ap.netif), dhcpd_start, NULL);

    ap = (wifi_mgmr_ap_msg_t*)msg->data;
    os_printf(DEBUG_HEADER "start AP with ssid %s;\r\n", ap->ssid);
    //os_printf(DEBUG_HEADER "              pwd  %s;\r\n", ap->psk);
    os_printf(DEBUG_HEADER "              channel  %ld;\r\n", ap->channel);
    bl_main_apm_start(ap->ssid, ap->psk, ap->channel, wifiMgmr.wlan_ap.vif_index);
    wifiMgmr.inf_ap_enabled = 1;
    aos_post_event(EV_WIFI, CODE_WIFI_ON_AP_STARTED, 0);

    return false;
}

static bool stateGlobalGuard_stop(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (WIFI_MGMR_EVENT_APP_AP_STOP != msg->ev) {
        return false;
    }

    os_printf(DEBUG_HEADER "Stoping AP interface...\r\n");
    bl_main_apm_stop(wifiMgmr.wlan_ap.vif_index);
    os_printf(DEBUG_HEADER "Removing and deauth all sta client...\r\n");
    bl_main_apm_remove_all_sta();
    os_printf(DEBUG_HEADER "Removing AP interface...\r\n");
    bl_main_if_remove(wifiMgmr.wlan_ap.vif_index);
    os_printf(DEBUG_HEADER "Stopping DHCP on AP interface...\r\n");
err_t dhcp_server_stop(struct netif *netif);
    netifapi_netif_common(&(wifiMgmr.wlan_ap.netif), NULL, dhcp_server_stop);
    os_printf(DEBUG_HEADER "Removing ETH interface ...\r\n");
    netifapi_netif_remove(&(wifiMgmr.wlan_ap.netif));
    wifiMgmr.inf_ap_enabled = 0;
    aos_post_event(EV_WIFI, CODE_WIFI_ON_AP_STOPPED, 0);

    return false;
}

const static struct state stateGlobal = {
   .parentState = NULL,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_GLB, (void*)WIFI_MGMR_EVENT_GLB_SCAN_IND_BEACON, &stateGlobalGuard_scan_beacon, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_GLB, (void*)WIFI_MGMR_EVENT_GLB_DISABLE_AUTORECONNECT, &stateGlobalGuard_disable_autoreconnect, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_GLB, (void*)WIFI_MGMR_EVENT_GLB_ENABLE_AUTORECONNECT, &stateGlobalGuard_enable_autoreconnect, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_APP, NULL, &stateGlobalGuard, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_AP_START, &stateGlobalGuard_AP, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_AP_STOP, &stateGlobalGuard_stop, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_DISCONNECT, &stateGlobalGuard_fw_disconnect, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_POWERSAVING, &stateGlobalGuard_fw_powersaving, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_SCAN, &stateGlobalGuard_fw_scan, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_FW,  (void*)WIFI_MGMR_EVENT_FW_DATA_RAW_SEND, &stateSnifferGuard_raw_send, &stateGlobalAction, &stateIdle},
   },
   .numTransitions = 10,
   .data = "group",
   .entryAction = &stateGlobalEnter,
   .exitAction = &stateGlobalExit,
};

const static struct state stateSniffer = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_IDLE, &stateSnifferGuard, &stateSnifferAction, &stateIdle},
      /*Will NOT transfer state*/
      {EVENT_TYPE_FW,  (void*)WIFI_MGMR_EVENT_FW_CHANNEL_SET, &stateSnifferGuard_ChannelSet, &stateSnifferAction, &stateIdle},
   },
   .numTransitions = 2,
   .data = "sniffer",
   .entryAction = &stateSnifferEnter,
   .exitAction = &stateSnifferExit,
};

const static struct state stateConnecting = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_IND_CONNECTED, &stateConnectingGuard, &stateConnectingAction_connected, &stateConnectedIPNo},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_IND_DISCONNECT, &stateConnectingGuard, &stateConnectingAction_disconnect, &stateDisconnect},
   },
   .numTransitions = 2,
   .data = "connecting",
   .entryAction = &stateConnectingEnter,
   .exitAction = &stateConnectingExit,
};

/********************section for ilde *************************/
static bool stateIdleGuard_connect(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ev != (void*)msg->ev) {
        return false;
    }
    if (bl_main_if_add(1, &wifiMgmr.wlan_sta.netif, &wifiMgmr.wlan_sta.vif_index)) {
        os_printf(DEBUG_HEADER "%s: add STA iface failed\r\n", __func__);
        return false;
    }
    return true;
}

static bool stateIdleGuard_sniffer(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ev != (void*)msg->ev) {
        return false;
    }

    bl_main_monitor();
    return true;
}

static void stateIdleAction_connect( void *oldStateData, struct event *event,
      void *newStateData )
{
    wifi_mgmr_msg_t *msg;
    wifi_mgmr_profile_msg_t *profile_msg;

    msg = event->data;
    profile_msg = (wifi_mgmr_profile_msg_t*)msg->data;
    profile_msg->ssid_tail[0] = '\0';
    profile_msg->psk_tail[0] = '\0';
    os_printf(DEBUG_HEADER "Action Connect\r\n");
    os_printf("           ssid %s\r\n", profile_msg->ssid);
    os_printf("           ssid len %u\r\n", (unsigned int)profile_msg->ssid_len);
    //os_printf("           psk %s\r\n", profile_msg->psk);
    os_printf("           psk len %u\r\n", (unsigned int)profile_msg->psk_len);
    os_printf("           pmk %s\r\n", profile_msg->pmk);
    os_printf("           pmk len %u\r\n", (unsigned int)profile_msg->pmk_len);
    os_printf("           channel band %d\r\n", (uint8_t)profile_msg->band);
    os_printf("           channel freq %d\r\n", (uint16_t)profile_msg->freq);
    os_printf("           mac %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            profile_msg->mac[5],
            profile_msg->mac[4],
            profile_msg->mac[3],
            profile_msg->mac[2],
            profile_msg->mac[1],
            profile_msg->mac[0]
    );
    os_printf("           dhcp status: %s\r\n", profile_msg->dhcp_use ? "true" : "false");
    wifi_mgmr_profile_add(&wifiMgmr, profile_msg, -1);

    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );

    //TODO Other security support
    bl_main_connect((const uint8_t *)profile_msg->ssid, profile_msg->ssid_len,
            (const uint8_t *)profile_msg->psk, profile_msg->psk_len,
            (const uint8_t *)profile_msg->pmk, profile_msg->pmk_len,
            (const uint8_t *)profile_msg->mac,
            (const uint8_t)profile_msg->band,
            (const uint16_t)profile_msg->freq
    );
}

static void stateIdleAction_sniffer( void *oldStateData, struct event *event,
      void *newStateData )
{
    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
}

static void stateIdleEnter( void *stateData, struct event *event )
{
   os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
}

static void stateIdleExit( void *stateData, struct event *event )
{
   os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
}

const static struct state stateIdle = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_CONNECT, &stateIdleGuard_connect, &stateIdleAction_connect, &stateConnecting},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_SNIFFER, &stateIdleGuard_sniffer, &stateIdleAction_sniffer, &stateSniffer},
   },
   .numTransitions = 2,
   .data = "idle",
   .entryAction = &stateIdleEnter,
   .exitAction = &stateIdleExit,
};
/*==================================================================================================*/


/********************section for ifacedown *************************/
static bool stateIfaceDownGuard_phyup(void *ev, struct event *event )
{
    int error;
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ev != (void*)msg->ev) {
        os_printf(DEBUG_HEADER "state mismatch\r\n");
        return false;
    }

    //TODO no such usage for function call
    error = bl_main_phy_up();
    if (error) {
        return false;
    }
    return true;
}

static void stateIfaceDownAction_phyup( void *oldStateData, struct event *event,
      void *newStateData )
{
    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
}

static void stateIfaceDownEnter( void *stateData, struct event *event )
{
    os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
}

static void stateIfaceDownExit( void *stateData, struct event *event )
{
    os_printf(DEBUG_HEADER "Exiting %s state\r\n", (char *)stateData);
}

const static struct state stateIfaceDown = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_PHY_UP, &stateIfaceDownGuard_phyup, &stateIfaceDownAction_phyup, &stateIdle},
   },
   .numTransitions = 1,
   .data = "ifaceDown",
   .entryAction = &stateIfaceDownEnter,
   .exitAction = &stateIfaceDownExit,
};
/*==================================================================================================*/

/********************section for connected but still no IP address*************************/
typedef struct connectedIPNoData {
    char name[32];//all the state data must start with name field
    os_timer_t timer;//used for timeout detect on obtain IP address
} connectedIPNoData_t;
static connectedIPNoData_t stateConnectedIPNo_data = {
    .name = "wifiConnected_ipObtaining",
};

typedef struct tsen_reload_data {
    char name[32];//all the state data must start with name field
    os_timer_t timer;//used for timeout detect on obtain IP address
} tsen_reload_data_t;
static tsen_reload_data_t state_tsen_reload_data = {
    .name = "wifi tsen reload",
};

static bool stateConnectedIPNoGuard(void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch == (void*)msg->ev) {
        return true;
    }
    return false;
}

static bool stateConnectedIPNoGuard_disconnect(void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    bl_main_disconnect();
    /*will never trigger state change, since we just want to trigger the guard*/
    return false;
}

static void stateConnectedIPNoAction_ipgot(void *oldStateData, struct event *event,
      void *newStateData )
{
    wifi_mgmr_msg_t *msg;
    wifi_mgmr_ipgot_msg_t *ipgot;

    USER_UNUSED(ipgot);
    msg = event->data;
    ipgot = (wifi_mgmr_ipgot_msg_t*)msg->data;
    os_printf(DEBUG_HEADER 
            "IP GOT IP:%u.%u.%u.%u, "
            "MASK: %u.%u.%u.%u, "
            "Gateway: %u.%u.%u.%u, "
            "dns1: %u.%u.%u.%u, "
            "dns2: %u.%u.%u.%u\r\n",
        (unsigned int)((ipgot->ip & 0x000000FF) >> 0),
        (unsigned int)((ipgot->ip & 0x0000FF00) >> 8),
        (unsigned int)((ipgot->ip & 0x00FF0000) >> 16),
        (unsigned int)((ipgot->ip & 0xFF000000) >> 24),
        (unsigned int)((ipgot->mask & 0x000000FF) >> 0),
        (unsigned int)((ipgot->mask & 0x0000FF00) >> 8),
        (unsigned int)((ipgot->mask & 0x00FF0000) >> 16),
        (unsigned int)((ipgot->mask & 0xFF000000) >> 24),
        (unsigned int)((ipgot->gw & 0x000000FF) >> 0),
        (unsigned int)((ipgot->gw & 0x0000FF00) >> 8),
        (unsigned int)((ipgot->gw & 0x00FF0000) >> 16),
        (unsigned int)((ipgot->gw & 0xFF000000) >> 24),
        (unsigned int)((ipgot->dns1 & 0x000000FF) >> 0),
        (unsigned int)((ipgot->dns1 & 0x0000FF00) >> 8),
        (unsigned int)((ipgot->dns1 & 0x00FF0000) >> 16),
        (unsigned int)((ipgot->dns1 & 0xFF000000) >> 24),
        (unsigned int)((ipgot->dns2 & 0x000000FF) >> 0),
        (unsigned int)((ipgot->dns2 & 0x0000FF00) >> 8),
        (unsigned int)((ipgot->dns2 & 0x00FF0000) >> 16),
        (unsigned int)((ipgot->dns2 & 0xFF000000) >> 24)
    );

    hal_wifi_ip_stat_t wifi_pnet;
    snprintf(wifi_pnet.ip, sizeof(wifi_pnet.ip), "%d.%d.%d.%d", (unsigned int)((ipgot->ip & 0x000000FF) >> 0), (unsigned int)((ipgot->ip & 0x0000FF00) >> 8),(unsigned int)((ipgot->ip & 0x00FF0000) >> 16), (unsigned int)((ipgot->ip & 0xFF000000) >> 24));
    NetCallback(&wifi_pnet);

    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
}

static void stateConnectedIPNoAction_disconnect(void *oldStateData, struct event *event,
      void *newStateData )
{
    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
}

static void ip_obtaining_timeout(void *timer, timer_cb_arg_t data)
{
    connectedIPNoData_t *stateData = os_timer_data(data);

    (void)stateData;
    //os_timer_delete_nodelay(&(stateData->timer));//detach no stop

    os_printf(DEBUG_HEADER "IP obtaining timeout\r\n");
    wifi_mgmr_api_fw_disconnect();
}

static void __reload_tsen(timer_cb_arg_t data)
{
    uint32_t *stateData = os_timer_data(data);

    (void)stateData;

    wifi_mgmr_api_fw_tsen_reload();
}

static void __run_reload_tsen(void)
{
    int16_t temp = 0;
    extern void phy_tcal_callback(int16_t temperature);

    if (&stateConnecting == wifiMgmr.m.currentState || &stateDisconnect == wifiMgmr.m.currentState || &stateConnectedIPYes == wifiMgmr.m.currentState ||
            &stateSniffer == wifiMgmr.m.currentState || &stateConnectedIPNo == wifiMgmr.m.currentState) {
        bl_tsen_adc_get(&temp, 0);
        phy_tcal_callback(temp);

        return ;
    }

    return;

}

static void stateConnectedIPNoEnter(void *stateData, struct event *event )
{
    connectedIPNoData_t *stateConnectedIPNo_data;

    stateConnectedIPNo_data = stateData;
    os_printf(DEBUG_HEADER "Entering %s state\r\n", stateConnectedIPNo_data->name);
    os_timer_init(&(stateConnectedIPNo_data->timer),
        "wifi IP obtaining",
        ip_obtaining_timeout,
        stateConnectedIPNo_data,
        15000,//timeout 15 seconds for ip obtaining
        OS_TIMER_TYPE_ONESHOT
    );
    // this timer is autorun
    //os_timer_start(&(stateConnectedIPNo_data->timer));
    os_printf(DEBUG_HEADER "DHCP Starting...%p\r\n", &(wifiMgmr.wlan_sta.netif));
    wifi_netif_dhcp_start(&(wifiMgmr.wlan_sta.netif));
    aos_post_event(EV_WIFI, CODE_WIFI_ON_CONNECTED, 0);
}

static void periodic_tsen_reload(void *stateData, struct event *event )
{
    tsen_reload_data_t *state_tsen_data;

    state_tsen_data = stateData;
    os_printf(DEBUG_HEADER "reload tsen \r\n");
    os_timer_init(&(state_tsen_data->timer),
        "wifi reload tsen",
        __reload_tsen,
        state_tsen_data,
        TSEN_RELOAD_MS,
        OS_TIMER_TYPE_REPEATED
    );
    os_timer_start(&(state_tsen_data->timer));
}

static void stateConnectedIPNoExit(void *stateData, struct event *event )
{
    connectedIPNoData_t *stateConnectedIPNo_data;

    stateConnectedIPNo_data = stateData;
    os_printf(DEBUG_HEADER "Exiting %s state\r\n", stateConnectedIPNo_data->name);
    os_timer_delete_nodelay(&(stateConnectedIPNo_data->timer));//detach no stop
}

const static struct state stateConnectedIPNo = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_IP_GOT, &stateConnectedIPNoGuard, &stateConnectedIPNoAction_ipgot, &stateConnectedIPYes},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_DISCONNECT, &stateConnectedIPNoGuard_disconnect, &stateConnectedIPNoAction_disconnect, &stateDisconnect},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_IND_DISCONNECT, &stateConnectedIPNoGuard, &stateConnectedIPNoAction_disconnect, &stateDisconnect},
   },
   .numTransitions = 3,
   .data = &stateConnectedIPNo_data,
   .entryAction = &stateConnectedIPNoEnter,
   .exitAction = &stateConnectedIPNoExit,
};
/*==================================================================================================*/




/********************section for connected with IP address*************************/
static bool stateConnectedIPYesGuard( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch == (void*)msg->ev) {
        return true;
    }
    return false;
}

static bool stateConnectedIPYesGuard_disconnect( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    bl_main_disconnect();
    /*will never trigger state change, since we just want to trigger the guard*/
    return false;
}

static bool stateConnectedIPYesGuard_rcconfig( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    os_printf(DEBUG_HEADER "rate config, use sta_idx 0, rate_config %04X\r\n", (unsigned int)(msg->data1));
    bl_main_rate_config(0, (uint32_t)msg->data1);
    /*will never trigger state change, since we just want to trigger the guard*/
    return false;
}

static void stateConnectedIPYes_action( void *oldStateData, struct event *event,
      void *newStateData )
{
    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
}

extern void WifiStatusHandler(int status);
static void stateConnectedIPYes_enter( void *stateData, struct event *event )
{
   os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
   WifiStatusHandler(NOTIFY_STATION_UP);
   aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0);
}

static void stateConnectedIPYes_exit( void *stateData, struct event *event )
{
   os_printf(DEBUG_HEADER "Exiting %s state\r\n", (char *)stateData);
}

const static struct state stateConnectedIPYes = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_DISCONNECT, &stateConnectedIPYesGuard_disconnect, &stateConnectedIPYes_action, &stateDisconnect},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_RC_CONFIG, &stateConnectedIPYesGuard_rcconfig, &stateConnectedIPYes_action, &stateDisconnect},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_IND_DISCONNECT, &stateConnectedIPYesGuard, &stateConnectedIPYes_action, &stateDisconnect},
   },
   .numTransitions = 3,
   .data = "wifiConnected_IPOK",
   .entryAction = &stateConnectedIPYes_enter,
   .exitAction = &stateConnectedIPYes_exit,
};
/*==================================================================================================*/

/********************section for disconnect *************************/
typedef struct disconnectData {
    char name[32];//all the state data must start with name field
    os_timer_t timer;
    uint8_t timer_started;
    wifi_mgmr_profile_msg_t profile_msg;
} disconnectData_t;
static disconnectData_t stateDisconnect_data = {
    .name = "disconnect",
};

static bool stateDisconnect_guard( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch == (void*)msg->ev) {
        return true;
    }
    return false;
}

static void stateDisconnect_action_reconnect( void *oldStateData, struct event *event,
      void *newStateData )
{
    disconnectData_t *stateDisconnect_data;
    wifi_mgmr_profile_msg_t *profile_msg;

    stateDisconnect_data = (disconnectData_t*)oldStateData;
    profile_msg = &(stateDisconnect_data->profile_msg);
    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );

    os_printf(DEBUG_HEADER " Action Connect\r\n");
    os_printf("           ssid %s\r\n", profile_msg->ssid);
    os_printf("           ssid len %u\r\n", (unsigned int)profile_msg->ssid_len);
    //os_printf("           psk %s\r\n", profile_msg->psk);
    os_printf("           psk len %u\r\n", (unsigned int)profile_msg->psk_len);
    os_printf("           pmk %s\r\n", profile_msg->pmk);
    os_printf("           pmk len %u\r\n", (unsigned int)profile_msg->pmk_len);
    os_printf("           mac %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            profile_msg->mac[5],
            profile_msg->mac[4],
            profile_msg->mac[3],
            profile_msg->mac[2],
            profile_msg->mac[1],
            profile_msg->mac[0]
    );

    //TODO Other security support
    bl_main_connect((const uint8_t *)profile_msg->ssid, profile_msg->ssid_len,
            (const uint8_t *)profile_msg->psk, profile_msg->psk_len,
            (const uint8_t *)profile_msg->pmk, profile_msg->pmk_len,
            (const uint8_t *)profile_msg->mac,
            0,
            0
    );
    aos_post_event(EV_WIFI, CODE_WIFI_CMD_RECONNECT, 0);
}

static void stateDisconnect_action_idle( void *oldStateData, struct event *event,
      void *newStateData )
{
    os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
    os_printf(DEBUG_HEADER "Removing STA interface...\r\n");
    bl_main_if_remove(wifiMgmr.wlan_sta.vif_index);
}

static void disconnect_retry(void *timer, timer_cb_arg_t data)
{
    disconnectData_t *stateData = os_timer_data(data);
    //os_timer_delete_nodelay(&(stateData->timer));

    /*XXX may in the handler mode*/
    /*TODO use EVENT to copy profile*/
    if (wifi_mgmr_profile_get(&wifiMgmr, &(stateData->profile_msg))) {
        os_printf(DEBUG_HEADER "Retry Again --->>> retry Abort, since profile copy failed\r\n");
    } else {
        os_printf(DEBUG_HEADER "Retry Again --->>> retry connect\r\n");
        wifi_mgmr_api_reconnect();
    }
}

static void stateDisconnect_enter(void *stateData, struct event *event)
{
    disconnectData_t *stateDisconnect_data;

    stateDisconnect_data = stateData;
    os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
    WifiStatusHandler(NOTIFY_STATION_DOWN);
    if (wifi_mgmr_profile_autoreconnect_is_enabled(&wifiMgmr, -1)) {
        os_timer_init(&(stateDisconnect_data->timer),
            "wifi disconnect",
            disconnect_retry,
            stateDisconnect_data,
            2000,//timeout 6 seconds for reconnect
            OS_TIMER_TYPE_ONESHOT
        );
        os_printf(DEBUG_HEADER "Will retry in 2000 ticks\r\n");
        //os_timer_start(&(stateDisconnect_data->timer));
        stateDisconnect_data->timer_started = 1;
    } else {
        os_printf(DEBUG_HEADER "Will NOT retry connect\r\n");
    }
    aos_post_event(EV_WIFI, CODE_WIFI_ON_DISCONNECT, wifiMgmr.wifi_mgmr_stat_info.status_code);
}

static void stateDisconnect_exit(void *stateData, struct event *event)
{
    disconnectData_t *stateDisconnect_data = stateData;

    os_printf(DEBUG_HEADER "Exiting %s state\r\n", (char *)stateDisconnect_data);
    if (stateDisconnect_data->timer_started) {
        os_printf("Delete Timer.\r\n");
        os_timer_delete_nodelay(&(stateDisconnect_data->timer));
        stateDisconnect_data->timer_started = 0;
    } else {
        os_printf("Delete Timer Skipped\r\n");
    }
}

const static struct state stateDisconnect = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_RECONNECT, &stateDisconnect_guard, &stateDisconnect_action_reconnect, &stateConnecting},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_IDLE, &stateDisconnect_guard, &stateDisconnect_action_idle, &stateIdle},
   },
   .numTransitions = 2,
   .data = &stateDisconnect_data,
   .entryAction = &stateDisconnect_enter,
   .exitAction = &stateDisconnect_exit,
};
/*==================================================================================================*/


const static struct state stateError = {
   .entryAction = &printErrMsg
};

int wifi_mgmr_event_notify(wifi_mgmr_msg_t *msg)
{
    while (0 == wifiMgmr.ready) {
        os_printf("Wait Wi-Fi Mgmr Start up...\r\n");
        os_thread_delay(20);
    }
    if (os_mq_send(&(wifiMgmr.mq), msg, msg->len)) {
        os_printf("Failed when send msg 0x%p, len dec:%u\r\n", msg, (unsigned int)msg->len);
        return -1;
    }
    return 0;
}

void wifi_mgmr_start(void)
{
    struct event ev;
    uint8_t buffer[WIFI_MGMR_MQ_MSG_SIZE + 8];
    wifi_mgmr_msg_t *msg;

    msg = (wifi_mgmr_msg_t*)(buffer + 1);
    ev.type = EVENT_TYPE_APP;
    ev.data = msg;
    stateM_init(&(wifiMgmr.m), &stateIfaceDown, &stateError);

    /*register event cb for Wi-Fi Manager*/
    wifi_mgmr_event_init();

    /*Noitfy mgmr is ready*/
    aos_post_event(EV_WIFI, CODE_WIFI_ON_MGMR_DONE, 0);

    /*TODO: use another way based on event sys?*/
    hal_sys_capcode_update(255, 255);

    /*periodic reload tsen */
    //periodic_tsen_reload(&state_tsen_reload_data, NULL);

    /*Run the event handler loop*/
    while (1) {
        if (0 == os_mq_recv(&(wifiMgmr.mq), msg, WIFI_MGMR_MQ_MSG_SIZE)) {
            if (msg->ev == WIFI_MGMR_EVENT_APP_RELOAD_TSEN) {
                __run_reload_tsen();
                continue;
            }
            ev.type = msg->ev < WIFI_MGMR_EVENT_MAXAPP_MINIFW ? EVENT_TYPE_APP : 
                (msg->ev < WIFI_MGMR_EVENT_MAXFW_MINI_GLOBAL ? EVENT_TYPE_FW : EVENT_TYPE_GLB);
            stateM_handleEvent(&(wifiMgmr.m), &ev);
        }
    }
}

static void _wifi_mgmr_entry(void *pvParameters)
{
    wifi_mgmr_start();
}

#define WIFI_MGMR_STACK_SIZE  (640)
static ktask_t wifi_mgmr_task;
static cpu_stack_t wifi_mgmr_stack[WIFI_MGMR_STACK_SIZE];

void wifi_mgmr_start_background(wifi_conf_t *conf)
{
    kstat_t ret;

    wifi_mgmr_drv_init(conf);
    ret = krhino_task_create(&wifi_mgmr_task, "wifi_mgmr", NULL, mgmr_TASK_PRIORITY, 
        0, wifi_mgmr_stack, WIFI_MGMR_STACK_SIZE, _wifi_mgmr_entry, RHINO_TRUE);
    while(ret != RHINO_SUCCESS);

#if 0
    os_thread_create("wifi_mgmr",
            _wifi_mgmr_entry,
            mgmr_TASK_PRIORITY,
            1024 * 3,
            NULL
    );
#endif
}

int wifi_mgmr_init(void)
{
    int ret;

    ret = os_mq_init(
            &(wifiMgmr.mq),
            "wifiMgmr",
            wifiMgmr.mq_pool,
            WIFI_MGMR_MQ_MSG_SIZE,
            sizeof(wifiMgmr.mq_pool)
    );
    while(ret != 0);
    printf("wifi_mgmr_queue init success\n");

    wifiMgmr.ready = 1;//TODO check ret
    return ret;
}

int wifi_mgmr_status_code_get_internal(int *s_code)
{
    (*s_code) = wifiMgmr.wifi_mgmr_stat_info.status_code;
    return 0;
}

int wifi_mgmr_state_get_internal(int *state)
{
    const struct state *m_state;
    int s_code = 0;

    m_state = wifiMgmr.m.currentState;
    wifi_mgmr_status_code_get_internal(&s_code);

    if (m_state == &stateIdle) {
        if (wifiMgmr.inf_ap_enabled) {
            *state = WIFI_STATE_WITH_AP_IDLE;
        } else {
            *state = WIFI_STATE_IDLE;
            if (s_code == WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE){
                *state = WIFI_STATE_PSK_ERROR;
            } else if (s_code == WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL){
                *state = WIFI_STATE_NO_AP_FOUND;
            }
        }
    } else if (m_state == &stateConnecting) {
        if (wifiMgmr.inf_ap_enabled) {
            *state = WIFI_STATE_WITH_AP_CONNECTING;
        } else {
            *state = WIFI_STATE_CONNECTING;
            if (s_code == WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE){
                *state = WIFI_STATE_PSK_ERROR;
            } else if (s_code == WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL){
                *state = WIFI_STATE_NO_AP_FOUND;
            }
        }
    } else if (m_state == &stateConnectedIPNo) {
        if (wifiMgmr.inf_ap_enabled) {
            *state = WIFI_STATE_WITH_AP_CONNECTED_IP_GETTING;
        } else {
            *state = WIFI_STATE_CONNECTED_IP_GETTING;
        }
    } else if (m_state == &stateConnectedIPYes) {
        if (wifiMgmr.inf_ap_enabled) {
            *state = WIFI_STATE_WITH_AP_CONNECTED_IP_GOT;
        } else {
            *state = WIFI_STATE_CONNECTED_IP_GOT;
        }
    } else if (m_state == &stateDisconnect) {
        if (wifiMgmr.inf_ap_enabled) {
            *state = WIFI_STATE_WITH_AP_DISCONNECT;
        } else {
            *state = WIFI_STATE_DISCONNECT;
            if (s_code == WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE){
                *state = WIFI_STATE_PSK_ERROR;
            } else if (s_code == WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL){
                *state = WIFI_STATE_NO_AP_FOUND;
            }
        }
    } else if (m_state == &stateIfaceDown) {
        *state = WIFI_STATE_IFDOWN;
    } else if (m_state == &stateSniffer) {
        *state = WIFI_STATE_SNIFFER;
    } else {
        *state = WIFI_STATE_UNKNOWN;
    }

    return 0;
}

void wifi_mgmr_set_connect_stat_info(struct wifi_event_sm_connect_ind *ind, uint8_t type_ind)
{
    int i;
    wifiMgmr.wifi_mgmr_stat_info.status_code = ind->status_code;
    for(i = 0;i < 6; i++) {
        wifiMgmr.wifi_mgmr_stat_info.bssid[i] = ind->bssid[i];
    }
    wifiMgmr.wifi_mgmr_stat_info.chan_freq = ind->center_freq;
    wifiMgmr.wifi_mgmr_stat_info.chan_band = ind->band;
    wifiMgmr.wifi_mgmr_stat_info.type_ind = type_ind;

#if 0
    blog_info("[RX] wifi_mgmr_set_connect_stat_info, wifiMgmr.wifi_mgmr_stat_info:\r\n");
    blog_info("[RX]   status_code %u\r\n", wifiMgmr.wifi_mgmr_stat_info.status_code);
    blog_info("[RX]   MAC %02X:%02X:%02X:%02X:%02X:%02X\r\n",
             wifiMgmr.wifi_mgmr_stat_info.bssid[0],
             wifiMgmr.wifi_mgmr_stat_info.bssid[1],
             wifiMgmr.wifi_mgmr_stat_info.bssid[2],
             wifiMgmr.wifi_mgmr_stat_info.bssid[3],
             wifiMgmr.wifi_mgmr_stat_info.bssid[4],
             wifiMgmr.wifi_mgmr_stat_info.bssid[5]
    );
    blog_info("[RX]   band %u\r\n", wifiMgmr.wifi_mgmr_stat_info.chan_band);
    blog_info("[RX]   center_freq %u\r\n", wifiMgmr.wifi_mgmr_stat_info.chan_freq);
    blog_info("[RX]   type_ind %u\r\n", wifiMgmr.wifi_mgmr_stat_info.type_ind);
#endif
}

int wifi_mgmr_set_country_code_internal(char *country_code)
{
    bl_main_set_country_code(country_code);
    strncpy(wifiMgmr.country_code, country_code, sizeof(wifiMgmr.country_code));
    wifiMgmr.country_code[2] = '\0';
    wifiMgmr.channel_nums = bl_main_get_channel_nums();
    blog_info("country code:%s, support channel nums:%d\r\n", wifiMgmr.country_code, wifiMgmr.channel_nums);

    return 0;
}

int wifi_mgmr_ap_sta_cnt_get_internal(uint8_t *sta_cnt)
{
    bl_main_apm_sta_cnt_get(sta_cnt);
    return 0;
}

int wifi_mgmr_ap_sta_info_get_internal(wifi_mgmr_sta_basic_info_t *sta_info_internal, uint8_t idx)
{
    struct wifi_apm_sta_info apm_sta_info;
    memset(&apm_sta_info, 0, sizeof(struct wifi_apm_sta_info));
    bl_main_apm_sta_info_get(&apm_sta_info, idx);
    sta_info_internal->sta_idx = apm_sta_info.sta_idx;
    sta_info_internal->is_used = apm_sta_info.is_used;
    sta_info_internal->rssi = apm_sta_info.rssi;
    sta_info_internal->tsflo = apm_sta_info.tsflo;
    sta_info_internal->tsfhi = apm_sta_info.tsfhi;
    sta_info_internal->data_rate = apm_sta_info.data_rate;
    memcpy(sta_info_internal->sta_mac, apm_sta_info.sta_mac, 6);
    return 0;
}

int wifi_mgmr_ap_sta_delete_internal(uint8_t sta_idx)
{
    bl_main_apm_sta_delete(sta_idx);
    return 0;
}

int wifi_mgmr_scan_complete_notify()
{
    wifi_mgmr_scan_complete_callback();
    return 0;
}

