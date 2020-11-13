/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "rda59xx_daemon.h"
#include "rda59xx_wifi_include.h"
#include "hal/wifi.h"
#include "lwip/ip4_addr.h"
#include "lwip/inet.h"

typedef enum {
    SCAN_NORMAL,
    SCAN_ADV
} scan_type_t;
hal_wifi_ip_stat_t ip_stat;

unsigned int filter_backup = 0;

static rda59xx_sta_info sta_info = {0};
static bool sta_try_reconnect = false;
static void handle_connect_fail(void *arg)
{
    LOG("%s\r\n", __func__);
    aos_msleep(500);
    if (sta_try_reconnect) {
        rda59xx_sta_connect(&sta_info);
    }
    // rda59xx_sta_connect will auto retry 4 times, need 25 seconds
    // when ap connect first fail, retry reconnect(actual retry 4 times)
    // then stop reconnect, to avoid blocking upper applications
    sta_try_reconnect = false;
}

void wifi_event_cb(WIFI_EVENT evt, void* info)
{
    hal_wifi_module_t *m = hal_wifi_get_default_module();
    hal_wifi_ip_stat_t ip_stat;
    hal_wifi_ap_info_adv_t ap_info;

    rda59xx_bss_info bss_info;
    switch (evt) {
        case EVENT_STA_GOT_IP: {
#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
            m->enter_powersave(m, WIFI_CONFIG_RECEIVE_DTIM);
#endif
            rda59xx_sta_get_bss_info(&bss_info);
            memcpy(ip_stat.ip, ip4addr_ntoa(&(bss_info.ipaddr)), sizeof(ip_stat.ip));
            memcpy(ip_stat.gate, ip4addr_ntoa(&(bss_info.gateway)), sizeof(ip_stat.gate));
            memcpy(ip_stat.mask, ip4addr_ntoa(&(bss_info.mask)), sizeof(ip_stat.mask));
            memcpy(ip_stat.dns, ip4addr_ntoa(&(bss_info.dns1)), sizeof(ip_stat.dns));
            snprintf(ip_stat.mac, 12, "%02x%02x%02x%02x%02x%02x",
                    bss_info.bssid[0], bss_info.bssid[1], bss_info.bssid[2], bss_info.bssid[3], bss_info.bssid[4], bss_info.bssid[5]);
            if (m->ev_cb && m->ev_cb->ip_got) {
                m->ev_cb->ip_got(m, &ip_stat, NULL);
            }
            memcpy(ap_info.bssid, bss_info.bssid, 6);
            memcpy(ap_info.ssid, bss_info.ssid, bss_info.ssid_len);
            ap_info.channel = bss_info.channel;
            ap_info.security = bss_info.secure;
            if (m->ev_cb && m->ev_cb->para_chg) {
                m->ev_cb->para_chg(m, &ap_info, NULL, 0, NULL);
            }
            break;
        }
        case EVENT_STA_CONNECTTED: {
            if (m->ev_cb && m->ev_cb->stat_chg) {
                m->ev_cb->stat_chg(m, NOTIFY_STATION_UP, NULL);
            }
            break;
        }
        case EVENT_STA_DISCONNECTTED: {
            if (m->ev_cb && m->ev_cb->stat_chg) {
                m->ev_cb->stat_chg(m, NOTIFY_STATION_DOWN, NULL);
            }
            break;
        }
        case EVENT_STA_CONNECT_FAIL: {
            LOG("%s EVENT_STA_CONNECT_FAIL\r\n", __func__);
            if (m->ev_cb && m->ev_cb->connect_fail) {
                m->ev_cb->connect_fail(m, 0, NULL);
            }
            if (sta_try_reconnect) {
                aos_schedule_call(handle_connect_fail, NULL);
            }
            break;
        }
        default: {
            break;
        }
    }

    return;
}

/**
    @brief Wifi scan ready
*/
static void scan_done(hal_wifi_module_t *m, scan_type_t t)
{
    uint16_t ap_num = 16;
    rda59xx_scan_result *ap_records;
    int i;
    hal_wifi_scan_result_t *result = NULL;
    hal_wifi_scan_result_adv_t *result_adv = NULL;

    if (t != SCAN_NORMAL && t != SCAN_ADV) return;

    ap_num = rda59xx_get_scan_num();
    if (ap_num > 16)
        ap_num = 16;

    ap_records = malloc(ap_num * sizeof(*ap_records));
    if (!ap_records)
        return;

    if (t == SCAN_NORMAL) {
        result = malloc(sizeof(hal_wifi_scan_result_t));
        if (!result) goto end;
        result->ap_num = ap_num;
        result->ap_list = malloc(ap_num * sizeof(ap_list_t));
        if (!result->ap_list) goto end;
    } else {
        result_adv = malloc(sizeof(hal_wifi_scan_result_adv_t));
        if (!result_adv) goto end;
        result_adv->ap_num = ap_num;
        result_adv->ap_list = malloc(ap_num * sizeof(ap_list_adv_t));
        if (!result_adv->ap_list) goto end;
    }

    rda59xx_get_scan_result(ap_records, ap_num);

    for (i = 0; i < ap_num; i++) {
        rda59xx_scan_result *r = ap_records + i;

        if (t == SCAN_NORMAL) {
            ap_list_t *res = result->ap_list + i;
            memcpy(res->ssid, r->SSID, sizeof(res->ssid));
            res->ap_power = r->RSSI;
        } else {
            ap_list_adv_t *res_adv = result_adv->ap_list + i;
            memcpy(res_adv->ssid, r->SSID, sizeof(res_adv->ssid));
            res_adv->ap_power = r->RSSI;
            memcpy(res_adv->bssid, r->BSSID, sizeof(res_adv->bssid));
            res_adv->channel = r->channel;
            res_adv->security = r->secure_type;
        }
    }

    if (t == SCAN_NORMAL) {
        if (m->ev_cb && m->ev_cb->scan_compeleted) {
            m->ev_cb->scan_compeleted(m, result, NULL);
        }
    } else {
        if (m->ev_cb && m->ev_cb->scan_adv_compeleted) {
            m->ev_cb->scan_adv_compeleted(m, result_adv, NULL);
        }
    }

end:
    if (ap_records)
        free(ap_records);
    if (result) {
        if (result->ap_list)
            free(result->ap_list);
        free(result);
    }
    if (result_adv) {
        if (result_adv->ap_list)
            free(result_adv->ap_list);
        free(result_adv);
    }
}

static monitor_data_cb_t data_cb = NULL;
static monitor_data_cb_t mngt_data_cb = NULL;
//static monitor_data_cb_t mesh_data_cb = NULL;

typedef enum {
    HAL_WIFI_MODE_STATION,
    HAL_WIFI_MODE_SOFTAP
} hal_wifi_mode_t;

__WEAK int board_wifi_init_hook(hal_wifi_module_t *m)
{
    return 0;
}

static int wifi_init(hal_wifi_module_t *m)
{
    static int inited;

    if (inited) return 0;
    inited = 1;

    board_wifi_init_hook(m);
    rda59xx_wifi_init();
    rda59xx_wifi_set_event_cb(wifi_event_cb);

    return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    rda59xx_get_macaddr(mac, 0);
};

static void wifi_set_mac_addr(hal_wifi_module_t *m, const uint8_t *mac)
{
    rda59xx_set_macaddr(mac, 0);
};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    sta_try_reconnect = true;
    memset(&sta_info, 0, sizeof(rda59xx_sta_info));
    memcpy(sta_info.ssid, init_para->wifi_ssid, 32+1);
    memcpy(sta_info.pw, init_para->wifi_key, 64+1);
    if(init_para->dhcp_mode){
        sta_info.dhcp = 1;
    }else{
        sta_info.dhcp = 0;
        ip4addr_aton((const char *)(init_para->local_ip_addr), (ip4_addr_t*)&(sta_info.ip));
        ip4addr_aton((const char *)(init_para->net_mask), (ip4_addr_t*)&(sta_info.netmask));
        ip4addr_aton((const char *)(init_para->gateway_ip_addr), (ip4_addr_t*)&(sta_info.gateway));
    }
    rda59xx_sta_connect(&sta_info);
    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m,
                          hal_wifi_init_type_adv_t *init_para_adv)
{
    LOG("WiFi HAL %s not implemeted yet!\r\n", __func__);
    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m,
                       hal_wifi_ip_stat_t *out_net_para,
                       hal_wifi_type_t wifi_type)
{
    rda59xx_bss_info bss_info;
    unsigned int state = 0;
    state = rda59xx_get_module_state();
    if ((wifi_type == STATION) && (state & STATE_STA)){
        rda59xx_sta_get_bss_info(&bss_info);
        strcpy(out_net_para->ip, ip4addr_ntoa(&bss_info.ipaddr));
        strcpy(out_net_para->mask, ip4addr_ntoa(&bss_info.mask));
        strcpy(out_net_para->gate, ip4addr_ntoa(&bss_info.gateway));
        strcpy(out_net_para->dns, ip4addr_ntoa(&bss_info.dns1));
        r_u32 broadcastip;
        broadcastip = (bss_info.mask&bss_info.ipaddr)|(~bss_info.mask);
        strcpy(out_net_para->broadcastip, ip4addr_ntoa(&broadcastip));
        snprintf(out_net_para->mac, 12, "%02x%02x%02x%02x%02x%02x",
                bss_info.bssid[0], bss_info.bssid[1], bss_info.bssid[2], bss_info.bssid[3], bss_info.bssid[4], bss_info.bssid[5]);
    }
    return 0;
}

static int get_link_stat(hal_wifi_module_t *m,
                         hal_wifi_link_stat_t *out_stat)
{
    rda59xx_bss_info bss_info;
    unsigned int state = 0;
    state = rda59xx_get_module_state();

    out_stat->is_connected = 0;
    if(state & STATE_STA){
        rda59xx_sta_get_bss_info(&bss_info);
        out_stat->is_connected = 1;
        out_stat->channel = bss_info.channel;
        out_stat->wifi_strength = bss_info.rssi;
        memcpy(out_stat->bssid, bss_info.bssid, 6);
        memcpy(out_stat->ssid, bss_info.ssid, sizeof(bss_info.ssid));
    }
    return 0;
}

static void wifi_scan(hal_wifi_module_t *m)
{
    rda59xx_scan_info scan_info;
    memset(&scan_info, 0, sizeof(rda59xx_scan_info));
    scan_info.scan_mode = 0;
    scan_info.scan_time = 1;
    rda59xx_scan(&scan_info);
    scan_done(m, SCAN_NORMAL);;
}

static void start_scan_adv(hal_wifi_module_t *m)
{
    rda59xx_scan_info scan_info;
    memset(&scan_info, 0, sizeof(rda59xx_scan_info));
    scan_info.scan_mode = 1;
    scan_info.scan_time = 2;
    rda59xx_scan(&scan_info);
    scan_done(m, SCAN_ADV);
}

static void start_scan_direct(hal_wifi_module_t *m)
{
    start_scan_adv(m);
}

static int power_off(hal_wifi_module_t *m)
{
    LOG("WiFi HAL %s not implemeted yet!\r\n", __func__);
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    LOG("WiFi HAL %s not implemeted yet!\r\n", __func__);
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    sta_try_reconnect = false;
    rda59xx_sta_disconnect();
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    sta_try_reconnect = false;
    rda59xx_sta_disconnect();
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{
    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    //unsigned int mode = rda59xx_get_module_state();
    //if(mode & STATE_SNIFFER)
    //    rda59xx_sniffer_set_channel(ch);
    //else
        rda59xx_set_channel(ch);
    return 0;
}

static int get_channel(hal_wifi_module_t *m)
{
    rda59xx_bss_info bss_info;
    unsigned int state = 0;

    if(state & STATE_STA){
        rda59xx_sta_get_bss_info(&bss_info);
        return bss_info.channel;
    }
    return 0;
}

int sniffer_cb(void *data, unsigned short data_len)
{
    if(data_cb != NULL) {
        (*data_cb)((uint8_t*)data, (int)data_len, (hal_wifi_link_info_t *)((int8_t*)data + data_len));
    }
    return 0;
}


static void start_monitor(hal_wifi_module_t *m)
{
    //if softap smartconfig failed, it will start monitor dirictly
    //so add disconnect to end last link
    sta_try_reconnect = false;
    rda59xx_sta_disconnect();
    rda59xx_sniffer_enable(sniffer_cb);
    rda59xx_sniffer_set_filter(1, 1, 0x27e77);
    filter_backup = 0x27e77;
}

static int start_ap(hal_wifi_module_t *m, const char *ssid, const char *passwd, int interval, int hide)
{
    rda59xx_ap_info aws_ap_info;
    memset((void*)&aws_ap_info, 0, sizeof(rda59xx_ap_info));

    strncpy(aws_ap_info.ssid, ssid, sizeof(aws_ap_info.ssid));
    strncpy(aws_ap_info.pw, passwd, sizeof(aws_ap_info.pw));

    aws_ap_info.channel = 6;
    aws_ap_info.hidden = hide;
    aws_ap_info.beacon = interval;

	aws_ap_info.dhcps=inet_addr("10.10.100.1");
	aws_ap_info.dhcpe=inet_addr("10.10.100.255");
	aws_ap_info.ip=inet_addr("10.10.100.1");
	aws_ap_info.gateway=inet_addr("10.10.100.1");
	aws_ap_info.netmask=inet_addr("255.255.255.0");

	return rda59xx_ap_enable(&aws_ap_info);
}

static int stop_ap(hal_wifi_module_t *m)
{
    return rda59xx_ap_disable();
}

static void stop_monitor(hal_wifi_module_t *m)
{
    rda59xx_sniffer_disable();
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    data_cb = fn;
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m,
                                          monitor_data_cb_t fn)
{
    data_cb = fn;
    if (data_cb) {
        rda59xx_sniffer_enable(sniffer_cb);
        rda59xx_sniffer_set_filter(1, 1, 0x7fe77);
        filter_backup = 0x7fe77;
    } else {
        rda59xx_sniffer_disable();
    }
    return 0;
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m,
                                     uint8_t *buf, int len)
{
    rda59xx_send_rawdata(buf, len);
    return 0;
}

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
static int set_listeninterval(hal_wifi_module_t *m, uint8_t listen_interval)
{
    rda5981_set_sta_listen_interval(listen_interval);
    return 0;
}

static int enter_powersave(hal_wifi_module_t *m, uint8_t recvDTIMs)
{
    printf("enter_powersave\n");
    wland_set_sta_sleep(1);
    return 0;
}

static int exit_powersave(hal_wifi_module_t *m)
{
    wland_set_sta_sleep(0);
    return 0;
}

#endif

hal_wifi_module_t aos_wifi_rda59xx = {
    .base.name           = "aos_wifi_rda59xx",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .set_mac_addr        =  wifi_set_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  wifi_scan,
    .start_scan_adv      =  start_scan_adv,
    .start_scan_direct   =  start_scan_direct,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .get_channel         =  get_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .start_ap            =  start_ap,
    .stop_ap             =  stop_ap,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    .set_listeninterval =  set_listeninterval,
    .enter_powersave    =  enter_powersave,
    .exit_powersave     =  exit_powersave,
#endif

    /* mesh related */
    //.mesh_register_cb    =  register_mesh_cb,
    //.mesh_enable         =  mesh_enable,
    //.mesh_disable        =  mesh_disable,
};


