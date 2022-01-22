/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/network.h>
#include <hal/hal.h>

#ifdef WITH_LWIP
#include <lwip/priv/tcp_priv.h>
#include <lwip/udp.h>
#endif

#include "iot_export.h"

#include "netmgr.h"

#ifdef CONFIG_AOS_MESHYTS
#undef CONFIG_AOS_MESH
#endif

#ifdef CONFIG_AOS_MESH
#include "umesh.h"
#endif

#define TAG "netmgr"

#ifndef WIFI_SSID
#define DEMO_AP_SSID "cisco-15A7"
#define DEMO_AP_PASSWORD "12345678"
#else
#define DEMO_AP_SSID WIFI_SSID
#define DEMO_AP_PASSWORD WIFI_PWD
#endif

#define MAX_RETRY_CONNECT 120
#define RETRY_INTERVAL_MS 500

#define HOTSPOT_AP "aha"
#define ROUTER_AP "adha"

typedef struct {
    netmgr_ap_config_t         saved_conf;
    netmgr_ap_config_t         ap_config;
    hal_wifi_module_t          *wifi_hal_mod;
    autoconfig_plugin_t        *autoconfig_chain;
    int32_t                    ipv4_owned;
    int8_t                     disconnected_times;
    bool                       doing_smartconfig;
    bool                       ip_available;
    netmgr_wifi_scan_result_cb_t cb;
    bool                       wifi_scan_complete_cb_finished;
} netmgr_cxt_t;

extern autoconfig_plugin_t g_alink_smartconfig;
#ifdef MANUFACT_AP_FIND_ENABLE
extern manufact_ap_find_t g_manufact_ap_find_module;
manufact_ap_find_cb g_manuap_find_cb = NULL;
#endif

static netmgr_cxt_t        g_netmgr_cxt;
#if !defined(CONFIG_YWSS) || defined(CSP_LINUXHOST)
static autoconfig_plugin_t g_def_smartconfig;
#endif

static bool g_station_is_up = false;

static void netmgr_wifi_config_start(void);
static void add_autoconfig_plugin(autoconfig_plugin_t *plugin);
static int set_wifi_ssid(void);
static void get_wifi_ssid(void);

static void format_ip(uint32_t ip, char *buf)
{
    int i = 0;

    unsigned char octet[4]  = { 0, 0, 0, 0 };

    for (i = 0; i < 4; i++) {
        octet[i] = ( ip >> ((3 - i) * 8) ) & 0xFF;
    }

    sprintf(buf, "%d.%d.%d.%d", octet[3], octet[2], octet[1], octet[0]);
}

static void netmgr_connect_fail_event(hal_wifi_module_t *m, int err, void *arg)
{
    aos_post_event(EV_WIFI, CODE_WIFI_ON_CONNECT_FAILED, 0u);
}

#ifdef CONFIG_AOS_MESH
static void mesh_delayed_action(void *arg)
{
    umesh_set_mode((node_mode_t)arg);
    umesh_stop();
    umesh_start();
}
#endif

static void start_mesh(bool is_leader)
{
#ifdef CONFIG_AOS_MESH
    node_mode_t mode;

    mode = umesh_get_mode() & (~MODE_LEADER);
    if (is_leader) {
        mode |= MODE_LEADER;
    }

    aos_post_delayed_action(1000, mesh_delayed_action, (void *)(long)mode);
#endif
}

static void stop_mesh(void)
{
#ifdef CONFIG_AOS_MESH
    umesh_stop();
#endif
}

static int32_t translate_addr(char *str)
{
    int32_t a, b, c, d;
    int32_t address = 0;
    sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
    address |= d << 24;
    address |= c << 16;
    address |= b << 8;
    address |= a;

    return address;
}

static void netmgr_ip_got_event(hal_wifi_module_t *m,
                                hal_wifi_ip_stat_t *pnet, void *arg)
{
    LOGI(TAG, "Got ip : %s, gw : %s, mask : %s", pnet->ip, pnet->gate, pnet->mask);

    g_netmgr_cxt.ipv4_owned = translate_addr(pnet->ip);
    g_netmgr_cxt.ip_available = true;
    aos_post_event(EV_WIFI, CODE_WIFI_ON_PRE_GOT_IP, 0u);
    start_mesh(true);

    #ifdef DEV_OFFLINE_LOG_ENABLE
    char ip[17] = {'\0'};

    wifi_get_ip(ip);
    diagnosis_offline_log_erase_flash_desc(); //Erase flash log if got IP success
    diagnosis_offline_log(LOG_LEVEL_I, "IP:%s\r\n", ip);
    #endif
}

#ifdef WITH_LWIP
#ifdef LOCAL_PORT_ENHANCED_RAND

#define TCP_LOCAL_PORT_SEED "lport_seed"
#define SEED_HISTORAY_MAX 10

typedef struct {
    unsigned int hist[SEED_HISTORAY_MAX];
    int total_num;
    int start_idx;
} seed_history_t;

static void ensure_different_seed(unsigned int *seed, seed_history_t *history)
{
    int i, idx = 0;

    if (!history || !seed) {
        return;
    }

    for (i = 0; i < history->total_num; i ++) {
        idx = (i + history->start_idx) % SEED_HISTORAY_MAX;

        if (history->hist[idx] == *seed) {
            printf("Same seed found %d\r\n", history->hist[idx]);
            *seed = *seed + (unsigned int)aos_now();
        }
    }
}

static void update_seed_history(seed_history_t *history, unsigned int new_seed)
{
    int idx = 0;

    if (!history) {
        return;
    }

    if (history->total_num >= SEED_HISTORAY_MAX) {
        LOGD(TAG, "History is full, let's replace the first entry.");
        history->hist[history->start_idx] = new_seed;
        history->start_idx++;
        history->start_idx %= SEED_HISTORAY_MAX;
    } else {
        idx = (history->start_idx + history->total_num) % SEED_HISTORAY_MAX;
        history->hist[idx] = new_seed;
        history->total_num++;
    }
}

static void dump_seed_history(seed_history_t *history)
{
#ifdef DEBUG
    int i;

    if (!history) {
        return;
    }

    for (i = 0; i < history->total_num; i++) {
        printf("%d ", history->hist[(history->start_idx + i) % SEED_HISTORAY_MAX]);
    }

    printf("\r\n");
#endif
}
#endif /* LOCAL_PORT_ENHANCED_RAND */
#endif /* LWIP */

static void netmgr_stat_chg_event(hal_wifi_module_t *m, hal_wifi_event_t stat,
                                  void *arg)
{
#ifdef WITH_LWIP
    unsigned int ts = (unsigned int)aos_now();
    static uint8_t rand_flag = 0;
#ifdef LOCAL_PORT_ENHANCED_RAND
    int ret, len;
    seed_history_t seed_history;
#endif
#endif

    switch (stat) {
        case NOTIFY_STATION_UP:
#ifdef WITH_LWIP
            if (0 == rand_flag) { // Do the rand operation only once
#ifdef LOCAL_PORT_ENHANCED_RAND
                LOGD(TAG, "The ts generated from system time is %d", ts);
                len = sizeof(seed_history);
                memset(&seed_history, 0, sizeof(seed_history));
                ret = aos_kv_get(TCP_LOCAL_PORT_SEED, &seed_history, &len);
                if (ret == 0) {
                    LOGD(TAG, "Seed found in kv.");
                    dump_seed_history(&seed_history);
                    ensure_different_seed(&ts, &seed_history);
                }
#endif /* LOCAL_PORT_ENHANCED_RAND */
                LOGD(TAG, "The final seed to use is %d", ts);
                srand(ts);
                tcp_init();
                udp_init();
#ifdef LOCAL_PORT_ENHANCED_RAND
                update_seed_history(&seed_history, ts);
                LOGD(TAG, "The new seed history to be saved:");
                dump_seed_history(&seed_history);
                ret = aos_kv_set(TCP_LOCAL_PORT_SEED, &seed_history,
                                 sizeof(seed_history), 1);
#endif /* LOCAL_PORT_ENHANCED_RAND */
                rand_flag = 1;
            }
#endif /* LWIP */
            g_station_is_up = true;
            aos_post_event(EV_WIFI, CODE_WIFI_ON_CONNECTED,
                           (unsigned long)g_netmgr_cxt.ap_config.ssid);
            break;
        case NOTIFY_STATION_DOWN:
            g_station_is_up = false;
            unsigned long reason_code = (arg) ? (*(unsigned long *)arg) : 0;
            aos_post_event(EV_WIFI, CODE_WIFI_ON_DISCONNECT, reason_code);
            break;
        case NOTIFY_AP_UP:
            break;
        case NOTIFY_AP_DOWN:
            break;
        default:
            break;
    }
}

static void get_bssid(uint8_t *to_fill, int size)
{
    memset(to_fill, 0, size);
}

static void netmgr_scan_completed_event(hal_wifi_module_t *m,
                                        hal_wifi_scan_result_t *result,
                                        void *arg)
{
    netmgr_wifi_scan_result_cb_t cb = g_netmgr_cxt.cb;
    int i, last_ap = 0;
    uint8_t bssid[ETH_ALEN];

    if (g_netmgr_cxt.cb) {
        for (i = 0; i < (result->ap_num); i++) {
            LOGD(TAG, "AP to add: %s", result->ap_list[i].ssid);
            if (i == (result->ap_num - 1)) {
                last_ap = 1;
            }
            get_bssid(bssid, ETH_ALEN);
            cb(result->ap_list[i].ssid, bssid, NETMGR_AWSS_AUTH_TYPE_WPA2PSK,
               NETMGR_AWSS_ENC_TYPE_NONE, 0, 0, last_ap);
        }
        g_netmgr_cxt.wifi_scan_complete_cb_finished = true;
    }
}

static void netmgr_scan_adv_completed_event(hal_wifi_module_t *m,
                                            hal_wifi_scan_result_adv_t *result,
                                            void *arg)
{
    netmgr_wifi_scan_result_cb_t cb = g_netmgr_cxt.cb;
    int i, last_ap = 0;

    if (g_netmgr_cxt.cb) {
        for (i = 0; i < (result->ap_num); i++) {
            LOGD(TAG, "AP to add: %s", result->ap_list[i].ssid);
            if (i == (result->ap_num - 1)) {
                last_ap = 1;
            }
            cb(result->ap_list[i].ssid, (const uint8_t *)result->ap_list[i].bssid,
               (enum NETMGR_AWSS_AUTH_TYPE)result->ap_list[i].security, NETMGR_AWSS_ENC_TYPE_NONE,
               result->ap_list[i].channel, result->ap_list[i].ap_power, last_ap);
        }
        g_netmgr_cxt.wifi_scan_complete_cb_finished = true;
    }
}


static void netmgr_para_chg_event(hal_wifi_module_t *m,
                                  hal_wifi_ap_info_adv_t *ap_info,
                                  char *key, int key_len, void *arg)
{
    int ret;

    if (!ap_info) return;

    LOGI(TAG, "%s %d, bssid: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
         __func__, __LINE__,
         (uint8_t)(ap_info->bssid[0]), (uint8_t)(ap_info->bssid[1]), (uint8_t)(ap_info->bssid[2]),
         (uint8_t)(ap_info->bssid[3]), (uint8_t)(ap_info->bssid[4]), (uint8_t)(ap_info->bssid[5]));

    /* Update bssid information here */
    memcpy(g_netmgr_cxt.ap_config.bssid, ap_info->bssid,
           sizeof(g_netmgr_cxt.saved_conf.bssid));

    memcpy(g_netmgr_cxt.saved_conf.bssid, g_netmgr_cxt.ap_config.bssid,
           sizeof(g_netmgr_cxt.saved_conf.bssid));

    if (strlen(g_netmgr_cxt.saved_conf.ssid) < 1)
    {
        LOGE(TAG, "%s ssid err", __func__);
        return;
    }

#ifdef DISABLE_SECURE_STORAGE
    ret = aos_kv_set(NETMGR_WIFI_KEY, &g_netmgr_cxt.saved_conf,
                     sizeof(netmgr_ap_config_t), 1);
#else
    ret = aos_kv_set(NETMGR_WIFI_SS_KEY, &g_netmgr_cxt.saved_conf,
                     sizeof(netmgr_ap_config_t), 1);
#endif
    if (ret != 0) {
        LOGE(TAG, "%s failed", __func__);
        return;
    }
}

static void netmgr_fatal_err_event(hal_wifi_module_t *m, void *arg)
{
}

static const hal_wifi_event_cb_t g_wifi_hal_event = {
    .connect_fail        = netmgr_connect_fail_event,
    .ip_got              = netmgr_ip_got_event,
    .stat_chg            = netmgr_stat_chg_event,
    .scan_compeleted     = netmgr_scan_completed_event,
    .scan_adv_compeleted = netmgr_scan_adv_completed_event,
    .para_chg            = netmgr_para_chg_event,
    .fatal_err           = netmgr_fatal_err_event,
};

static void reconnect_wifi(void *arg)
{
    hal_wifi_module_t    *module;
    hal_wifi_init_type_t type;
    netmgr_ap_config_t   *ap_config = &(g_netmgr_cxt.ap_config);

    module = hal_wifi_get_default_module();

    memset(&type, 0, sizeof(type));
    type.wifi_mode = STATION;
    type.dhcp_mode = DHCP_CLIENT;
    strncpy(type.wifi_ssid, ap_config->ssid, sizeof(type.wifi_ssid) - 1);
#ifdef DISABLE_SECURE_STORAGE
    strncpy(type.wifi_key, ap_config->pwd, sizeof(type.wifi_key) - 1);
#else
    extern int iotx_ss_decrypt(const char* in_data, int in_len, char* out_data, int out_len);
    iotx_ss_decrypt(ap_config->pwd, MAX_PWD_SIZE, type.wifi_key, MAX_PWD_SIZE);
#endif
    hal_wifi_start(module, &type);
}

void netmgr_reconnect_wifi()
{
    g_netmgr_cxt.ip_available = false;
    reconnect_wifi(NULL);
}

static void get_wifi_ssid(void)
{
    memset(g_netmgr_cxt.ap_config.ssid, 0, sizeof(g_netmgr_cxt.ap_config.ssid));
    strncpy(g_netmgr_cxt.ap_config.ssid, g_netmgr_cxt.saved_conf.ssid,
            sizeof(g_netmgr_cxt.ap_config.ssid) - 1);

    memset(g_netmgr_cxt.ap_config.pwd, 0, sizeof(g_netmgr_cxt.ap_config.pwd));
#ifdef DISABLE_SECURE_STORAGE
    strncpy(g_netmgr_cxt.ap_config.pwd, g_netmgr_cxt.saved_conf.pwd,
            sizeof(g_netmgr_cxt.ap_config.pwd) - 1);
#else
    memcpy(g_netmgr_cxt.ap_config.pwd, g_netmgr_cxt.saved_conf.pwd,
            MAX_PWD_SIZE);
#endif

    memset(g_netmgr_cxt.ap_config.bssid, 0, sizeof(g_netmgr_cxt.ap_config.bssid));
    memcpy(g_netmgr_cxt.ap_config.bssid, g_netmgr_cxt.saved_conf.bssid,
           sizeof(g_netmgr_cxt.ap_config.bssid));
}

static int clear_wifi_ssid(void)
{
    int ret = 0;

    memset(g_netmgr_cxt.ap_config.ssid, 0, sizeof(g_netmgr_cxt.ap_config.ssid));
    memset(g_netmgr_cxt.ap_config.pwd, 0, sizeof(g_netmgr_cxt.ap_config.pwd));
    memset(g_netmgr_cxt.ap_config.bssid, 0, sizeof(g_netmgr_cxt.ap_config.bssid));

    memset(&g_netmgr_cxt.saved_conf, 0, sizeof(netmgr_ap_config_t));
#ifdef DISABLE_SECURE_STORAGE
    ret = aos_kv_del(NETMGR_WIFI_KEY); // use kv_del instead of kv_set in case kv is full
#else
    ret = aos_kv_del(NETMGR_WIFI_KEY); /* Remove unsecured config, if exists */
    ret = aos_kv_del(NETMGR_WIFI_SS_KEY); /* Remove secured config */
#endif
    return ret;
}

static int set_wifi_ssid(void)
{
    int ret = 0;

    memset(&g_netmgr_cxt.saved_conf, 0,
           sizeof(netmgr_ap_config_t));
    strncpy(g_netmgr_cxt.saved_conf.ssid,
            g_netmgr_cxt.ap_config.ssid,
            sizeof(g_netmgr_cxt.saved_conf.ssid) - 1);
#ifdef DISABLE_SECURE_STORAGE
    strncpy(g_netmgr_cxt.saved_conf.pwd,
            g_netmgr_cxt.ap_config.pwd,
            sizeof(g_netmgr_cxt.saved_conf.pwd) - 1);
#else
    memcpy(g_netmgr_cxt.saved_conf.pwd,
            g_netmgr_cxt.ap_config.pwd,
            MAX_PWD_SIZE);
#endif
    memcpy(g_netmgr_cxt.saved_conf.bssid,
           g_netmgr_cxt.ap_config.bssid,
           sizeof(g_netmgr_cxt.saved_conf.bssid));

    if (strlen(g_netmgr_cxt.saved_conf.ssid) < 1)
    {
        LOGE(TAG, "%s ssid err", __func__);
        return -1;
    }

#ifdef DISABLE_SECURE_STORAGE
    ret = aos_kv_set(NETMGR_WIFI_KEY, (unsigned char *)&g_netmgr_cxt.saved_conf,
                     sizeof(netmgr_ap_config_t), 1);
#else
    ret = aos_kv_set(NETMGR_WIFI_SS_KEY, (unsigned char *)&g_netmgr_cxt.saved_conf,
                     sizeof(netmgr_ap_config_t), 1);
    if(0 == ret) { /* Remove unsecured config only when secured config save success */
        aos_kv_del(NETMGR_WIFI_KEY); /* Remove unsecured config, if exists */
	}
#endif
    return ret;
}

static void handle_wifi_disconnect(void)
{
    g_netmgr_cxt.disconnected_times++;

    stop_mesh();

#if 0 // low level handle disconnect
    if (netmgr_has_valid_ap() == 1 && g_netmgr_cxt.disconnected_times < MAX_RETRY_CONNECT) {
        aos_post_delayed_action(RETRY_INTERVAL_MS, reconnect_wifi, NULL);
    } else {
        clear_wifi_ssid();
        netmgr_wifi_config_start();
    }
#endif
}
#ifdef AWSS_SUPPORT_STATIS
extern void awss_update_statis(int idx, int type);
#endif

static void netmgr_events_executor(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_WIFI) {
        return;
    }

    switch (eventinfo->code) {
        case CODE_WIFI_ON_CONNECTED:
            if (g_station_is_up == true) {
                g_netmgr_cxt.disconnected_times = 0;
            }
            break;
        case CODE_WIFI_ON_DISCONNECT:
            if (g_station_is_up == false) {
                handle_wifi_disconnect();
                g_netmgr_cxt.ip_available = false;
            }
            #ifdef DEV_OFFLINE_LOG_ENABLE
            diagnosis_offline_log_save_all();
            #endif
            break;
        case CODE_WIFI_ON_PRE_GOT_IP:
            if (g_netmgr_cxt.doing_smartconfig) {
                if (strcmp(g_netmgr_cxt.ap_config.ssid, HOTSPOT_AP) != 0 && \
                    strcmp(g_netmgr_cxt.ap_config.ssid, ROUTER_AP) != 0) {
                    LOGI(TAG, "Let's post GOT_IP event.");
                    g_netmgr_cxt.autoconfig_chain->config_result_cb(
                        0, g_netmgr_cxt.ipv4_owned);
                    g_netmgr_cxt.autoconfig_chain->autoconfig_stop();
                } else {
                    LOGI(TAG, "In hotspot/router mode, do not post GOT_IP event here.");
                }
            } else {
                aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP,
                               (unsigned long)(&g_netmgr_cxt.ipv4_owned));
            }
            break;
        case CODE_WIFI_ON_GOT_IP:
            if (g_netmgr_cxt.doing_smartconfig) {
                g_netmgr_cxt.doing_smartconfig = false;
            } else {
#ifdef AWSS_SUPPORT_STATIS
                awss_update_statis(0, 1);
#endif
            }
            set_wifi_ssid();
            break;
        case CODE_WIFI_CMD_RECONNECT:
            g_netmgr_cxt.disconnected_times = 0;
            g_netmgr_cxt.ip_available = false;
            LOGD(TAG, "reconnect wifi - %s", g_netmgr_cxt.ap_config.ssid);
#ifdef AWSS_SUPPORT_STATIS
            awss_update_statis(0, 0);
#endif
            reconnect_wifi(NULL);
            break;
        default :
            break;
    }
}

void wifi_get_ip(char ips[16])
{
    format_ip(g_netmgr_cxt.ipv4_owned, ips);
}

void netmgr_register_wifi_scan_result_callback(netmgr_wifi_scan_result_cb_t cb)
{
    g_netmgr_cxt.cb = cb;
    g_netmgr_cxt.wifi_scan_complete_cb_finished = false;
}

static void netmgr_wifi_config_start(void)
{
    autoconfig_plugin_t *valid_plugin = g_netmgr_cxt.autoconfig_chain;

    if (valid_plugin != NULL) {
        g_netmgr_cxt.doing_smartconfig = true;
        valid_plugin->autoconfig_start();
    } else {
        LOGW(TAG, "net mgr none config policy");
        start_mesh(false);
    }
}

// 0-no valid ap, 1-has valid ap
int32_t netmgr_has_valid_ap(void)
{
    int32_t len = strlen(g_netmgr_cxt.ap_config.ssid);

    if (len <= 0) {
        return 0;
    }

    return 1;
}

static void add_autoconfig_plugin(autoconfig_plugin_t *plugin)
{
    plugin->next = g_netmgr_cxt.autoconfig_chain;
    g_netmgr_cxt.autoconfig_chain = plugin;
}

int  netmgr_get_ap_config(netmgr_ap_config_t *config)
{
    if (netmgr_has_valid_ap() == 0) {
        return -1;
    }

    strncpy(config->ssid, g_netmgr_cxt.ap_config.ssid, MAX_SSID_SIZE);
#ifdef DISABLE_SECURE_STORAGE
    strncpy(config->pwd, g_netmgr_cxt.ap_config.pwd, MAX_PWD_SIZE);
#else
    memcpy(config->pwd, g_netmgr_cxt.ap_config.pwd, MAX_PWD_SIZE);
#endif
    memcpy(config->bssid, g_netmgr_cxt.ap_config.bssid, sizeof(config->bssid));

    return 0;
}

void netmgr_clear_ap_config(void)
{
    clear_wifi_ssid();
}

int netmgr_set_ap_config(netmgr_ap_config_t *config)
{
    int ret = 0;

    strncpy(g_netmgr_cxt.ap_config.ssid, config->ssid,
            sizeof(g_netmgr_cxt.ap_config.ssid) - 1);
#ifdef DISABLE_SECURE_STORAGE
    strncpy(g_netmgr_cxt.ap_config.pwd, config->pwd,
            sizeof(g_netmgr_cxt.ap_config.pwd) - 1);
#else
    extern int iotx_ss_encrypt(const char* in_data, int in_len, char* out_data, int out_len);
    iotx_ss_encrypt(config->pwd, MAX_PWD_SIZE, g_netmgr_cxt.ap_config.pwd, MAX_PWD_SIZE);
#endif
    memcpy(g_netmgr_cxt.ap_config.bssid, config->bssid,
           sizeof(g_netmgr_cxt.ap_config.bssid));

    return ret;
}

void netmgr_set_smart_config(autoconfig_plugin_t *plugin)
{
    add_autoconfig_plugin(plugin);
    netmgr_wifi_config_start();
}

static void read_persistent_conf(void)
{
    int ret;
    int len;

    len = sizeof(netmgr_ap_config_t);
    ret = aos_kv_get(NETMGR_WIFI_KEY, &g_netmgr_cxt.saved_conf, &len);
    if (ret < 0) {
#ifdef DISABLE_SECURE_STORAGE
        return;
#else
        LOGW(TAG, "try secured ap config!");
        ret = aos_kv_get(NETMGR_WIFI_SS_KEY, &g_netmgr_cxt.saved_conf, &len);
        if (ret < 0) {
            LOGE(TAG, "no valied ap config found!");
            return;
        }
    } else {
        LOGW(TAG, "unsecured ap config found!");
        netmgr_set_ap_config(&g_netmgr_cxt.saved_conf);
        memcpy(g_netmgr_cxt.saved_conf.pwd, g_netmgr_cxt.ap_config.pwd,
           sizeof(g_netmgr_cxt.saved_conf.pwd));
#endif
    }
    get_wifi_ssid();
}

static void handle_netmgr_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";
    if (strcmp(rtype, "clear") == 0) {
        netmgr_clear_ap_config();
    } else if (strcmp(rtype, "connect") == 0) {
        if (argc != 4) {
            return;
        }
#ifdef WIFI_PROVISION_ENABLED
        extern int awss_stop(void);
        awss_stop();
#endif
        netmgr_ap_config_t config;

        strncpy(config.ssid, argv[2], sizeof(config.ssid) - 1);
        strncpy(config.pwd, argv[3], sizeof(config.pwd) - 1);
        netmgr_set_ap_config(&config);
        netmgr_start(false);
    } else {
        netmgr_start(true);
    }
}

static struct cli_command ncmd = {
    .name = "netmgr",
    .help = "netmgr [start|clear|connect ssid password]",
    .function = handle_netmgr_cmd,
};

#ifdef MANUFACT_AP_FIND_ENABLE
int netmgr_manuap_info_set(char *p_ssid_manu, char *p_pwd_manu, manufact_ap_find_cb cb)
{
    g_manuap_find_cb = cb;
    return g_manufact_ap_find_module.pf_manufact_ap_info_set(p_ssid_manu, p_pwd_manu);
}
#endif

int netmgr_init(void)
{
    hal_wifi_module_t *module;

    aos_register_event_filter(EV_WIFI, netmgr_events_executor, NULL);
    aos_cli_register_command(&ncmd);

    module = hal_wifi_get_default_module();
    memset(&g_netmgr_cxt, 0, sizeof(g_netmgr_cxt));
    g_netmgr_cxt.ip_available = false;
    g_netmgr_cxt.wifi_scan_complete_cb_finished = false;
    g_netmgr_cxt.wifi_hal_mod = module;
#if defined(CONFIG_YWSS) && !defined(CSP_LINUXHOST)
    add_autoconfig_plugin(&g_alink_smartconfig);
#else
    add_autoconfig_plugin(&g_def_smartconfig);
#endif
    hal_wifi_install_event(g_netmgr_cxt.wifi_hal_mod, &g_wifi_hal_event);
    read_persistent_conf();

#ifdef CONFIG_AOS_MESH
    umesh_init(MODE_RX_ON);
#endif

    return 0;
}

void netmgr_deinit(void)
{
    memset(&g_netmgr_cxt, 0, sizeof(g_netmgr_cxt));
}

int netmgr_start(bool autoconfig)
{
#ifdef MANUFACT_AP_FIND_ENABLE
    int ret = 0;
    netmgr_ap_config_t manu_ap_config = {{0}, {0}, {0}};
#endif
    stop_mesh();

    if (netmgr_has_valid_ap() == 1) {
        aos_post_event(EV_WIFI, CODE_WIFI_CMD_RECONNECT, 0);
        return 0;
    }
#ifdef CONFIG_AOS_NETMGRYTS_NOSMARTCONFIG
    else {
        LOGI(TAG, "netmgr yts only supports valid AP connect test, "
             "please ensure you have correct AP/passwd information set"
             " in kv before you do this test.");
        return -1;
    }
#endif

#ifdef MANUFACT_AP_FIND_ENABLE
    /* manufact ap scanning and fiding. */
    ret = g_manufact_ap_find_module.pf_manufact_ap_find(manu_ap_config.ssid, manu_ap_config.pwd, manu_ap_config.bssid);
    if (g_manuap_find_cb) {
        g_manuap_find_cb(ret);
    }
    if (ret == 0) {
        // manufact ap found
        netmgr_set_ap_config(&manu_ap_config);
        aos_post_event(EV_WIFI, CODE_WIFI_CMD_RECONNECT, 0);
        return 0;
    }
#endif

    if (autoconfig) {
        netmgr_wifi_config_start();
        return 0;
    }

    start_mesh(false);
    return -1;
}

bool netmgr_get_ip_state()
{
    return g_netmgr_cxt.ip_available;
}

bool netmgr_get_scan_cb_finished()
{
    return g_netmgr_cxt.wifi_scan_complete_cb_finished;
}

/* Returned IP[16] is in dot format, eg. 192.168.1.1. */
void netmgr_wifi_get_ip(char ip[])
{
    if (!ip) {
        LOGE(TAG, "Invalid argument in %s", __func__);
    } else {
        format_ip(g_netmgr_cxt.ipv4_owned, ip);
    }
}

#if !defined(CONFIG_YWSS) || defined(CSP_LINUXHOST)
static int def_smart_config_start(void)
{
    netmgr_ap_config_t config;

    strncpy(config.ssid, DEMO_AP_SSID, sizeof(config.ssid) - 1);
    strncpy(config.pwd, DEMO_AP_PASSWORD, sizeof(config.pwd) - 1);
    netmgr_set_ap_config(&config);
    aos_post_event(EV_WIFI, CODE_WIFI_CMD_RECONNECT, 0);
    return 0;
}

static void def_smart_config_stop(void)
{
    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP,
                   (unsigned long)(&g_netmgr_cxt.ipv4_owned));
}

static void def_smart_config_result_cb(int result, uint32_t ip)
{
}

static autoconfig_plugin_t g_def_smartconfig = {
    .description = "def_smartconfig",
    .autoconfig_start = def_smart_config_start,
    .autoconfig_stop = def_smart_config_stop,
    .config_result_cb = def_smart_config_result_cb
};
#endif
