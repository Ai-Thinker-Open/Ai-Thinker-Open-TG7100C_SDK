/**
 ****************************************************************************************
 *
 * @file lega_wifi_api.h
 *
 * @brief WiFi API.
 *
 * Copyright (C) ASR
 *
 ****************************************************************************************
 */
#ifndef _LEGA_WIFI_API_H_
#define _LEGA_WIFI_API_H_

#include <stdint.h>
#include "lwip/inet.h"
#include "lega_cm4.h"

/**
 *  @brief  wlan network interface enumeration definition.
 */
typedef enum {
  SOFTAP,   /*Act as an access point, and other station can connect, 4 stations Max*/
  STA,      /*Act as a station which can connect to an access point*/
} lega_wlan_type_e;

enum {
    WLAN_DHCP_DISABLE = 0,
    WLAN_DHCP_CLIENT,
    WLAN_DHCP_SERVER,
};

typedef enum {
    EVENT_STATION_UP = 1,  /*used in station mode,
                            indicate station associated in open mode or 4-way-handshake done in WPA/WPA2*/
    EVENT_STATION_DOWN,    /*used in station mode, indicate station deauthed*/
    EVENT_AP_UP,           /*used in softap mode, indicate softap enabled*/
    EVENT_AP_DOWN,         /*used in softap mode, indicate softap disabled*/
} lega_wifi_event_e;
typedef enum {
    CONNECT_SUCC,
    CONNECT_SCAN_FAIL,
    CONNECT_CONN_FAIL,
} lega_start_adv_results_e;

/**
 *  @brief  Scan result using normal scan.
 */
typedef struct {
  uint8_t is_scan_adv;
  char ap_num;       /**< The number of access points found in scanning. */
  struct {
    char    ssid[32+1];  /*ssid max len:32. +1 is for '\0'. when ssidlen is 32  */
    char    ap_power;   /**< Signal strength, min:0, max:100. */
    char    bssid[6];     /* The BSSID of an access point. */
    char    channel;      /* The RF frequency, 1-13 */
    uint8_t security;     /* Security type, @ref wlan_sec_type_t */
  } * ap_list;
} lega_wlan_scan_result_t;

typedef enum {
    WLAN_SECURITY_OPEN,                   //NONE
    WLAN_SECURITY_WEP,                    //WEP
    WLAN_SECURITY_WPA,                    //WPA
    WLAN_SECURITY_WPA2,                   //WPA2
    WLAN_SECURITY_AUTO,                   //WPA or WPA2
    WLAN_SECURITY_MAX,
} lega_wlan_security_e;

/*used in event callback of station mode, indicate softap informatino which is connected*/
typedef struct {
    int     rssi;           /* rssi */
    char    ssid[32+1];     /* ssid max len:32. +1 is for '\0' when ssidlen is 32  */
    char    pwd[64+1];      /* pwd max len:64. +1 is for '\0' when pwdlen is 64 */
    char    bssid[6];       /* BSSID of the wlan needs to be connected.*/
    char    ssid_len;       /*ssid length*/
    char    pwd_len;        /*password length*/
    char    channel;       /* wifi channel 0-13.*/
    char    security;      /*refer to lega_wlan_security_e*/
} lega_wlan_ap_info_adv_t;

/*only used in station mode*/
typedef struct {
    char    dhcp;             /* no use currently */
    char    macaddr[16];      /* mac address on the target wlan interface, ASCII*/
    char    ip[16];           /* Local IP address on the target wlan interface, ASCII*/
    char    gate[16];         /* Router IP address on the target wlan interface, ASCII */
    char    mask[16];         /* Netmask on the target wlan interface, ASCII*/
    char    dns[16];          /* no use currently , ASCII*/
    char    broadcastip[16];  /* no use currently , ASCII*/
} lega_wlan_ip_stat_t;

/*only used in station mode*/
typedef struct {
    int     is_connected;  /* The link to wlan is established or not, 0: disconnected, 1: connected. */
    int     wifi_strength; /* Signal strength of the current connected AP */
    char    ssid[32+1];      /* ssid max len:32. +1 is for '\0'. when ssidlen is 32  */
    char    bssid[6];      /* BSSID of the current connected wlan */
    int     channel;       /* Channel of the current connected wlan */
} lega_wlan_link_stat_t;

/*used in open cmd of hal_wifi_module_t*/
typedef struct {
    char    wifi_mode;              /* refer to hal_wifi_type_t*/
    char    security;               /* security mode */
    char    wifi_ssid[32];          /* in station mode, indicate SSID of the wlan needs to be connected.
                                       in softap mode, indicate softap SSID*/
    char    wifi_key[64];           /* in station mode, indicate Security key of the wlan needs to be connected,
                                       in softap mode, indicate softap password.(ignored in an open system.) */
    char    local_ip_addr[16];      /* used in softap mode to config ip for dut */
    char    net_mask[16];           /* used in softap mode to config gateway for dut */
    char    gateway_ip_addr[16];    /* used in softap mode to config netmask for dut */
    char    dns_server_ip_addr[16]; /* no use currently */
    char    dhcp_mode;              /* no use currently */
    char    channel;                /* softap channel in softap mode; connect channel in sta mode*/
    char    mac_addr[6];            /* connect bssid in sta mode*/
    char    reserved[32];           /* no use currently */
    int     wifi_retry_interval;    /* no use currently */
    int     interval;               /* used in softap mode to config beacon listen interval */
    int     hide;                   /* used in softap mode to config hidden SSID */
} lega_wlan_init_type_t;

/**
 * @brief sta ip and mac address used in softap mode
 * sta_ip_addr: e.g. when ip addr==192.168.1.1<-->sta_ip_addr == 0x0101A8C0
 */
typedef struct{
    uint32_t sta_ip_addr;                           /* station ip addr  */
    uint8_t  sta_mac_addr[6];                       /* station mac addr */
}lega_wlan_client_addr_info_t;

/*store linked station info*/
typedef struct{
    int client_num;                                 /* linked station number */
    lega_wlan_client_addr_info_t sta[4];            /* linked station entry, max client number is 4 */
}lega_wlan_ap_client_info_t;

/** @brief  wifi init functin, user should call it before use any wifi cmd
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_init(void);

/** @brief  wifi deinit functin, call it when donot use wifi any more to free resources
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_deinit(void);

/** @brief  used in station and softap mode, open wifi cmd
 *
 * @param init_info    : refer to lega_wlan_init_type_t
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_open(lega_wlan_init_type_t* init_info);

/** @brief  used in station and softap mode, close wifi cmd
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_close(void);

/** @brief  used in station mode, scan cmd
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_start_scan(void);

/** @brief  used in station mode, scan cmd
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_start_scan_adv(void);

/** @brief  used in station mode, scan cmd
 *
 * @param ssid    : target ssid to scan
 * @param is_scan_advance :scan to get bssid, channel and security
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_start_scan_active(const char *ssid, uint8_t is_scan_advance);

/** @brief  used in station and softap mode, get mac address(in hex mode) of WIFI device
 *
 * @param mac_addr    : pointer to get the mac address
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_get_mac_address(uint8_t *mac_addr);

/** @brief  used in station and softap mode, set mac address for WIFI device
 *
 *  @param mac_addr    : 6 bytes src mac array to set, DOES NOT support string or pointer
 *
 *  @return    0       : on success.
 *  @return    -1      : NULL pointer
 *  @return    -2      : mac address already exist in efuse, can't set
 *  @return    -3      : param is invalid
 */
int lega_wlan_set_mac_address(uint8_t *mac_addr);


/** @brief  used in station mode, get the ip information
 *
 * @param void
 * @return    NULL    : error occurred.
 * @return    pointer : ip status got.
 */
lega_wlan_ip_stat_t * lega_wlan_get_ip_status(void);


/** @brief  used in station mode, get link status information
 *
 * @param link_status    : refer to lega_wlan_link_stat_t
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_get_link_status(lega_wlan_link_stat_t *link_status);

/** @brief  used in station mode, get the associated ap information
 *
 * @param void
 * @return    NULL    : error occurred.
 * @return    pointer : associated ap info got.
 */
lega_wlan_ap_info_adv_t *lega_wlan_get_associated_apinfo(void);

/*used in sniffer mode, open sniffer mode
*  @return    0       : on success.
*  @return    other   : error occurred
*/
int lega_wlan_start_monitor(void);

/*used in sniffer mode, close sniffer mode
*  @return    0       : on success.
*  @return    other   : error occurred
*/
int lega_wlan_stop_monitor(void);

/** @brief  used in sniffer mode, set the sniffer channel, should call this cmd after call start_monitor cmd
 *
 * @param channel    : WIFI channel(1-13)
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_monitor_set_channel(int channel);

/** @brief  used to get current channel both in sta and ap mode
 *
 *  @return    1-14 : channel number.
 *  @return    0   :  no valid channel
 */
int lega_wlan_get_channel(void);

/** @brief  used in sta mode, set the ps bc mc and listen interval, called before connect to ap.
 *
 * @param listen_bc_mc    : true or false
 * @param listen_interval :1, 3, 10
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_set_ps_options(uint8_t listen_bc_mc, uint16_t listen_interval);

/** @brief  used in sta mode, set ps mode on/off
 *
 * @param ps_on    : true or false
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_set_ps_mode(uint8_t ps_on);

/*when use monitor mode, user should register this type of callback function to get the received MPDU*/
typedef void (*monitor_cb_t)(uint8_t*data, int len, int rssi);

/*when use monitor-ap mode, user should register this type of callback function */
typedef void (*monitor_ap_cb_t)();

/** @brief  used in sniffer mode, callback function to get received MPDU, should register before start_monitor
 *
 * @param fn    : refer to monitor_data_cb_t
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_register_monitor_cb(monitor_cb_t fn);

/** @brief  used in sniffer-ap mode, callback function for application
 *
 * @param fn    : refer to monitor_ap_cb_t
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_register_monitor_ap_cb(monitor_ap_cb_t fn);

/* start adv callback function, notify the connect results*/
typedef void (*start_adv_cb_t)(lega_start_adv_results_e status);

/** @brief  used in sta mode, callback function to notify the connecting results
 *
 * @param fn    : refer to start_adv_cb_t
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_register_start_adv_cb(start_adv_cb_t fn);


/** @brief  used in station mode or sniffer mode, call this cmd to send a MPDU constructed by user
 *
 * @param buf    :  mac header pointer of the MPDU
 * @param len    :  length of the MPDU
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_send_raw_frame(uint8_t *buf, int len);

/*enable WIFI stack log, will be output by uart
*
* @return    0       : on success.
* @return    other   : error occurred
*/
int lega_wlan_start_debug_mode(void);

/*disable WIFI stack log
*
* @return    0       : on success.
* @return    other   : error occurred
*/
int lega_wlan_stop_debug_mode(void);

/*
 * The event callback function called at specific wifi events occurred by wifi stack.
 * user should register these callback if want to use the informatin.
 *
 * @note For HAL implementors, these callbacks must be
 *       called under normal task context, not from interrupt.
 */
typedef void (*lega_wlan_cb_ip_got)(lega_wlan_ip_stat_t *ip_status);
/** @brief  used in station mode, WIFI stack call this cb when get ip
 *
 * @param fn    : cb function type, refer to lega_wlan_ip_stat_t
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_ip_got_cb_register(lega_wlan_cb_ip_got fn);

typedef void (*lega_wlan_cb_stat_chg)(lega_wifi_event_e wlan_event);
/** @brief  used in station and softap mode,
 *          WIFI stack call this cb when status change, refer to lega_wifi_event_e
 *
 * @param fn    : cb function type
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_stat_chg_cb_register(lega_wlan_cb_stat_chg fn);

typedef void (*lega_wlan_cb_scan_compeleted)(lega_wlan_scan_result_t *result);
/** @brief  used in station mode,
 *          WIFI stack call this cb when scan complete
 *
 * @param fn    : cb function type
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_scan_compeleted_cb_register(lega_wlan_cb_scan_compeleted fn);

typedef void (*lega_wlan_cb_associated_ap)(lega_wlan_ap_info_adv_t *ap_info);
/** @brief  used in station mode,
 *          WIFI stack call this cb when associated with an AP, and tell the AP information
 *
 * @param fn    : cb function type
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_associated_ap_cb_register(lega_wlan_cb_associated_ap fn);

/** @brief  calibration RCO clock for RTC
 *
 */
void lega_drv_rco_cal(void);

/** @brief  config to close DCDC PFM mode
 *
 */
void lega_drv_close_dcdc_pfm(void);

/** @brief  config to support smartconfig in MIMO scenario
 *
 */
void lega_wlan_smartconfig_mimo_enable(void);

/** @brief  set country code to update country code, different country may have different channel list
 *   called after hal_wifi_init
 */
void lega_wlan_set_country_code(char *country);

/** @brief  start monitor and ap coexist mode
 *
 * @param init_info    : refer to lega_wlan_init_type_t
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_start_monitor_ap(lega_wlan_init_type_t* init_info);

/** @brief  stop monitor and ap coexist mode
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_stop_monitor_ap();

/** @brief  get linked station ip and address in ap mode
 *  @param            : input param to get sta ip and mac addr
 */
void lega_get_client_ip_mac(lega_wlan_ap_client_info_t* sta_addr);


/** @brief  get current temperature (C degree)
 *  called after hal_wifi_init
 *
 * @param p_temp    : input param to get temperature, memory managed by caller
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int16_t  lega_rf_get_temperature(int16_t *p_temp);

/* temperature get callback function, notify the current temperature*/
typedef void (*temperature_get_cb_t)(int16_t temperature);

/** @brief  set the timer to get temperature (in second)
 *  called after hal_wifi_init
 *
 * @param timer_in_sec    : the timer in second
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_set_temperature_get_timer(uint64_t timer_in_sec);

/** @brief  register the temperature get callback function
 *  called after hal_wifi_init
 *
 * @param func    : the function will called to notify the temperature.
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_register_temperature_get_cb(temperature_get_cb_t func);

/* efuse info update callback function, pass customer efuse info to ASR*/
typedef void (*efuse_info_update_cb_t)(efuse_info_t *efuse_info);

/** @brief  register the efuse info update callback function if efuse layout not same as ASR
 *  called before hal_wifi_init
 *
 * @param func    : the function will be called to pass customer efuse info to ASR.
 *
 * @return    0       : on success.
 * @return    other   : error occurred
 */
int lega_wlan_register_efuse_info_update_cb(efuse_info_update_cb_t func);

/* used to report STA's ip and mac when STA connected to softap and got ip */
typedef void (*ap_assign_ip_cb_t)(ip_addr_t ip, uint8_t *mac);

/** @brief  used in softap mode, callback function to report STA's ip and mac
 *
 * @param fn    : refer to ap_assign_ip_cb_t
 *
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_register_ap_assign_ip_cb(ap_assign_ip_cb_t fn);

/** @brief  used in sta mode, set ap lost time in milliseconds
 *
 *  @param time_ms     : time in milliseconds, range is [3000, 25000] and DELTIM is 100
 *  @return    0       : on success.
 *  @return    other   : error occurred
 */
int lega_wlan_set_ap_lost_time(uint32_t time_ms);

#endif  //_LEGA_WIFI_API_H_
