/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef NETMGR_H
#define NETMGR_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#define MAX_SSID_SIZE  32
#define MAX_PWD_SIZE   64

#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN (MAX_SSID_SIZE+1)
#endif
#ifndef ETH_ALEN
#define ETH_ALEN                        (6)
#endif

#define NETMGR_WIFI_KEY "wifi"
#define NETMGR_WIFI_SS_KEY "wifi_ss"

/* 1 bigger than actual size for holding \0 */
typedef struct {
    char ssid[MAX_SSID_SIZE + 1];
    uint8_t bssid[ETH_ALEN];
    char pwd[MAX_PWD_SIZE + 1];
} netmgr_ap_config_t;

typedef struct autoconfig_plugin_s {
    struct autoconfig_plugin_s *next;
    const char                 *description;

    int  (*autoconfig_start)(void);
    void (*autoconfig_stop)(void);
    void (*config_result_cb)(int result, uint32_t ip);
} autoconfig_plugin_t;

#ifdef MANUFACT_AP_FIND_ENABLE
typedef struct manufact_ap_find_s {
    int (*pf_manufact_ap_info_set)(char *p_ssid_manu, char *p_pwd);
    int (*pf_manufact_ap_find)(char *p_ssid, char *p_pwd, uint8_t *p_bssid);
} manufact_ap_find_t;

// result: 0 manufacterer ap found, -1 manufacterer ap not found
typedef void (*manufact_ap_find_cb)(int result);
#endif

/* auth type */
enum NETMGR_AWSS_AUTH_TYPE {
    NETMGR_AWSS_AUTH_TYPE_OPEN,
    NETMGR_AWSS_AUTH_TYPE_SHARED,
    NETMGR_AWSS_AUTH_TYPE_WPAPSK,
    NETMGR_AWSS_AUTH_TYPE_WPA8021X,
    NETMGR_AWSS_AUTH_TYPE_WPA2PSK,
    NETMGR_AWSS_AUTH_TYPE_WPA28021X,
    NETMGR_AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
    NETMGR_AWSS_AUTH_TYPE_MAX = NETMGR_AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
    NETMGR_AWSS_AUTH_TYPE_INVALID = 0xff,
};

/* encry type */
enum NETMGR_AWSS_ENC_TYPE {
    NETMGR_AWSS_ENC_TYPE_NONE,
    NETMGR_AWSS_ENC_TYPE_WEP,
    NETMGR_AWSS_ENC_TYPE_TKIP,
    NETMGR_AWSS_ENC_TYPE_AES,
    NETMGR_AWSS_ENC_TYPE_TKIPAES,
    NETMGR_AWSS_ENC_TYPE_MAX = NETMGR_AWSS_ENC_TYPE_TKIPAES,
    NETMGR_AWSS_ENC_TYPE_INVALID = 0xff,
};

/*refer to platform_wifi_scan_result_cb_t definition in alink*/
typedef int (*netmgr_wifi_scan_result_cb_t)(
    const char ssid[MAX_SSID_LEN],
    const uint8_t bssid[ETH_ALEN],
    enum NETMGR_AWSS_AUTH_TYPE auth,
    enum NETMGR_AWSS_ENC_TYPE encry,
    uint8_t channel, signed char rssi,
    int is_last_ap);

void wifi_get_ip(char ips[16]);
int  netmgr_set_ap_config(netmgr_ap_config_t *config);
int  netmgr_get_ap_config(netmgr_ap_config_t *config);
void netmgr_clear_ap_config(void);
void netmgr_set_smart_config(autoconfig_plugin_t *plugin);
void netmgr_register_wifi_scan_result_callback(netmgr_wifi_scan_result_cb_t cb);
bool netmgr_get_scan_cb_finished(void);
bool netmgr_get_ip_state(void);
void netmgr_reconnect_wifi(void);
void netmgr_wifi_get_ip(char *ip);
int32_t netmgr_has_valid_ap(void);              // 0-no valid ap, 1-has valid ap

#ifdef MANUFACT_AP_FIND_ENABLE
int netmgr_manuap_info_set(char *p_ssid_manu, char *p_pwd_manu, manufact_ap_find_cb cb);
#endif

int netmgr_init(void);
void netmgr_deinit(void);
int netmgr_start(bool autoconfig);

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_H */
