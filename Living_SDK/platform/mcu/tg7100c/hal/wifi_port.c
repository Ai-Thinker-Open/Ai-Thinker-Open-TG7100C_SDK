/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

//#include "common.h"
#include "wifi_mgmr_ext.h"
#include <hal/base.h>
#include <hal/wifi.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <blog.h>

typedef enum {
  SCAN_NORMAL,
  SCAN_ADV
} scan_type_t;

static wifi_conf_t conf =
{
    .country_code = "CN",
};

static int is_suspend = 0;
static int is_init = 0;
#define SCAN_STATE_IDLE         (0)
#define SCAN_STATE_BUSY         (1)
#define SCAN_STATE_DONE         (2)
static volatile int g_scan_state = SCAN_STATE_IDLE;

hal_wifi_module_t sim_aos_wifi_tg7100c;
static monitor_data_cb_t cb_monitor = NULL;
static monitor_data_cb_t cb_mgmt = NULL;
wifi_interface_t wifi_interface;
char pmk[65] = "4967A6839076EADAC4FEE4B2F7F0A5AC4710C291581A13F4AC8927D4C79F6652";
static unsigned long g_reason_code = 0;

#define STACK_TASK_FW_WORD (1024+64)
ktask_t fw_task_fw;

cpu_stack_t fw_task_buf[STACK_TASK_FW_WORD];

void NetCallback(hal_wifi_ip_stat_t *pnet);
static void WifiStatusHandler(int status, unsigned long reason_code);
static int get_ip_stat(hal_wifi_module_t *m,
                       hal_wifi_ip_stat_t *out_net_para,
                       hal_wifi_type_t wifi_type);

static void _dump_hex(uint8_t *data, int len, int tab_num)
{
    int i;
    for (i = 0; i < len; i++) {
        os_printf("%02x ", data[i]);

        if (!((i + 1) % tab_num)) {
            os_printf("\r\n");
        }
    }

    os_printf("\r\n");
}
static void __event_cb_wifi_event(input_event_t *event, void *private_data)
{
    hal_wifi_ip_stat_t state;
    struct sm_connect_tlv_desc* ele = NULL;
#ifdef EN_COMBO_NET
    extern int32_t aiot_ais_report_awss_status(uint8_t status, uint16_t subcode);
#endif
    switch (event->code) {
        case CODE_WIFI_ON_DISCONNECT:
        {
            os_printf("[APP] [EVT] disconnect , Reason: %ld\r\n", event->value);
            WifiStatusHandler(NOTIFY_STATION_DOWN, event->value);
#ifdef EN_COMBO_NET
            extern int ms_cnt;
            int rssi = 0;
            // wifi_mgmr_rssi_get(&rssi);
            // os_printf("[APP] [EVT] rssi: %d\r\n", rssi);
            aiot_ais_report_awss_status(4, 0x0100 | event->value);
#endif
            /* Get every ele in diagnose tlv data */
            uint8_t* buff = NULL;
            buff = aos_malloc(512);
            while ((ele = wifi_mgmr_diagnose_tlv_get_ele()))
            {
                os_printf("[APP] [EVT] diagnose tlv id: %d, len: %d, data: %p\r\n",
                    ele->id, ele->len, ele->data);
                  // _dump_hex(ele->data, ele->len, 32);
#ifdef EN_COMBO_NET
                // if ( ms_cnt >= 32000 && ms_cnt <= 43000)
                if ( ms_cnt >= 2000 )
                {
                  // _dump_hex(ele->data, ele->len, 32);
                  uint8_t len = 3 + 2 + 2 + 1 + ele->len;
                  if (NULL != buff) {
                    buff[0] = 0x01;
                    buff[1] = 0x01;
                    buff[2] = 0x06;
                    buff[3] = 0x06;
                    buff[4] = 2 + 1 + ele->len;
                    buff[5] = (uint8_t)(ele->id&0xff);
                    buff[6] = (uint8_t)(ele->id>>8);
                    buff[7] = ele->len;
                    memcpy(&buff[8], ele->data, ele->len);
                    // _dump_hex(buff, len, 32);
                    int ret = 0;
                    do{
                      ret = breeze_post(buff, len);
                      aos_msleep(20);
                    } while (ret != 0);
                    // os_printf("[APP] [EVT] free=%p\r\n", buff);
                    os_printf("[APP] [EVT] send TLV\r\n");

                  }
                }
#endif
                /* MUST free diagnose tlv data ele */
                wifi_mgmr_diagnose_tlv_free_ele(ele);
            }
            if (NULL != buff) {
              os_printf("[APP] [EVT] free=%p\r\n", buff);
              aos_free(buff);
            }
// #ifdef EN_COMBO_NET
//                 if ( ms_cnt >= 32000 && ms_cnt <= 43000) {
//                   ms_cnt = 50000;
//                 }
// #endif
        }
        break;
        case CODE_WIFI_ON_CONNECTING:
        {
            os_printf("[APP] [EVT] Connecting %d\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_CONNECTED:
        {
            /* Get every ele in diagnose tlv data */
            while ((ele = wifi_mgmr_diagnose_tlv_get_ele()))
            {
                // os_printf("[APP] [EVT] diagnose tlv data %d, id: %d, len: %d, data: %p\r\n", aos_now_ms(),
                // ele->id, ele->len, ele->data);
                // _dump_hex(ele->data, ele->len, 32);

                /* MUST free diagnose tlv data */
                wifi_mgmr_diagnose_tlv_free_ele(ele);
            }

            os_printf("[APP] [EVT] connected %d\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_PRE_GOT_IP:
        {
            os_printf("[APP] [EVT] connected %d\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_GOT_IP:
        {
            if (!is_suspend) {
                os_printf("[APP] [EVT] GOT IP %d\r\n", aos_now_ms());
                get_ip_stat(NULL, &state, STATION);
                NetCallback(&state);
            }
        }
        break;
        case CODE_WIFI_ON_SCAN_DONE:
        {
            os_printf("[APP] [EVT] scan done %d\r\n", aos_now_ms());
            wifi_mgmr_cli_scanlist();
        }
        break;

        default:
        {
            /*nothing*/
        }
    }
}

static int wifi_init(hal_wifi_module_t *m) {
  if (is_init) {
    return 0;
  }

  tcpip_init(NULL, NULL);

  extern int bl_pm_init(void);
  bl_pm_init();
  extern void wifi_main(void *);
  krhino_task_create(&fw_task_fw, "task_fw", 0, 3, 0, fw_task_buf, STACK_TASK_FW_WORD, wifi_main, 1);

  aos_register_event_filter(TG7100C_EV_WIFI, __event_cb_wifi_event, (void *)NULL);

  // XXX(Zhuoran.rong) this sleep must exist
  // task_fw must run before wifi_mgmr_start_background
  // beacuse some unknow shcdule issue, the task_fw will
  // run later than wifi_mgmr_start_background.
  krhino_task_sleep(200);

  wifi_mgmr_start_background(&conf);

  aos_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);

  is_init = 1;

  printf("wifi init success!!\n");
  return 0;
};

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac) {
  bl_wifi_mac_addr_get(mac);
#if 0
  printf("------------------------------------- get MAC %02X:%02X:%02X:%02X:%02X:%02X\n",
         mac[0],
         mac[1],
         mac[2],
         mac[3],
         mac[4],
         mac[5]);
#endif
};

static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para) {
  int state;
  //printf("wifi_start ssid %s, key %s \r\n", init_para->wifi_ssid, init_para->wifi_key);
  if (NULL == m || NULL == init_para) {
    printf("wifi_start: invalid parameter\n");
    return -1;
  }

  blog_info("%s\r\n", __func__);
  if (init_para->wifi_mode == STATION) {
    printf("wifi_mode == STATION \n");

    wifi_mgmr_state_get(&state);

    if (WIFI_STATE_AP_IS_ENABLED(state)) {
      printf("first we should stop ap mode\n");
      wifi_mgmr_ap_stop(wifi_interface);
    } else {
      wifi_mgmr_sta_disable(wifi_interface);
    }

    if (WIFI_STATE_CONNECTED_IP_GOT == state
        || WIFI_STATE_CONNECTED_IP_GETTING == state) {
      wifi_mgmr_sta_disconnect();
      aos_msleep(1000);
      wifi_mgmr_sta_disable(NULL);
    }

    wifi_interface = wifi_mgmr_sta_enable();

    if (init_para->dhcp_mode != DHCP_CLIENT) {
        uint32_t ip, mask, gw, dns1, dns2;

        ip = ipaddr_addr(init_para->local_ip_addr);
        mask = ipaddr_addr(init_para->net_mask);
        gw = ipaddr_addr(init_para->gateway_ip_addr);
        dns1 = ipaddr_addr(init_para->dns_server_ip_addr);
        dns2 = 0;
        wifi_mgmr_sta_ip_set(ip, mask, gw, dns1, dns2);
    }

    if (strcmp(init_para->wifi_ssid, "aha") == 0 && strcmp(init_para->wifi_key, "12345678") == 0) {
      //printf("connect aha, pmk: %x %x\n", pmk[0], pmk[1]);
      wifi_mgmr_sta_connect(wifi_interface, init_para->wifi_ssid, init_para->wifi_key, pmk, NULL, 0, 0);
      //wifi_mgmr_sta_connect(wifi_interface, init_para->wifi_ssid, pmk, NULL, NULL);
    } else {
      wifi_mgmr_sta_connect(wifi_interface, init_para->wifi_ssid, init_para->wifi_key, NULL, NULL, 0, 0);
    }
    wifi_mgmr_sta_autoconnect_enable();

  } else if (init_para->wifi_mode == SOFT_AP) {
    printf("wifi_mode == SOFT_AP \n");
    wifi_interface = wifi_mgmr_ap_enable();
    wifi_mgmr_ap_start(wifi_interface, init_para->wifi_ssid, 0, init_para->wifi_key, 1);
  }

  return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv) {
  blog_info("%s\r\n", __func__);
  return -1;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type) {
  uint32_t ip, gw, mask, dns1, dns2;
  uint8_t mac[6];

  if (wifi_type == STATION) {
    ip = 0;
    gw = 0;
    mask = 0;
    dns1 = 0;
    dns2 = 0;
    memset(out_net_para, 0, sizeof(hal_wifi_ip_stat_t));

    out_net_para->dhcp = 1;
    wifi_mgmr_sta_ip_get(&ip, &gw, &mask);
    wifi_mgmr_sta_mac_get(mac);
    snprintf(out_net_para->ip, sizeof(out_net_para->ip), "%lu.%lu.%lu.%lu",
             (ip & 0x000000FF) >> 0,
             (ip & 0x0000FF00) >> 8,
             (ip & 0x00FF0000) >> 16,
             (ip & 0xFF000000) >> 24);
    snprintf(out_net_para->gate, sizeof(out_net_para->gate), "%lu.%lu.%lu.%lu",
             (gw & 0x000000FF) >> 0,
             (gw & 0x0000FF00) >> 8,
             (gw & 0x00FF0000) >> 16,
             (gw & 0xFF000000) >> 24);
    snprintf(out_net_para->mask, sizeof(out_net_para->mask), "%lu.%lu.%lu.%lu",
             (mask & 0x000000FF) >> 0,
             (mask & 0x0000FF00) >> 8,
             (mask & 0x00FF0000) >> 16,
             (mask & 0xFF000000) >> 24);
    snprintf(out_net_para->dns, sizeof(out_net_para->dns), "%lu.%lu.%lu.%lu",
             (dns1 & 0x000000FF) >> 0,
             (dns1 & 0x0000FF00) >> 8,
             (dns1 & 0x00FF0000) >> 16,
             (dns1 & 0xFF000000) >> 24);
    snprintf(out_net_para->mac, sizeof(out_net_para->mac), "%02X%02X%02X%02X%02X%02x",
             mac[0],
             mac[1],
             mac[2],
             mac[3],
             mac[4],
             mac[5]);
  }
  return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat) {
  int state;
  int channel;

  if (wifi_mgmr_state_get(&state)) {
    return -1;
  }

  switch (state) {
  case WIFI_STATE_CONNECTED_IP_GOT:
  case WIFI_STATE_CONNECTED_IP_GETTING:
  {
    wifi_mgmr_sta_connect_ind_stat_info_t info;

    wifi_mgmr_sta_connect_ind_stat_get(&info);
    wifi_mgmr_channel_get(&channel);
    wifi_mgmr_rssi_get(&(out_stat->wifi_strength));

    memcpy(out_stat->ssid, info.ssid, sizeof(info.ssid));
    memcpy(out_stat->bssid, info.bssid, sizeof(info.bssid));

    out_stat->is_connected = 1;
    out_stat->channel = channel;
  } break;

  case WIFI_STATE_CONNECTING:
  case WIFI_STATE_IDLE:
  case WIFI_STATE_DISCONNECT:
  case WIFI_STATE_UNKNOWN:
  case WIFI_STATE_WITH_AP_IDLE:
  case WIFI_STATE_WITH_AP_CONNECTING:
  case WIFI_STATE_WITH_AP_CONNECTED_IP_GETTING:
  case WIFI_STATE_WITH_AP_CONNECTED_IP_GOT:
  case WIFI_STATE_WITH_AP_DISCONNECT:
  case WIFI_STATE_IFDOWN:
  case WIFI_STATE_SNIFFER:
  case WIFI_STATE_PSK_ERROR:
  case WIFI_STATE_NO_AP_FOUND:
  default: {
    out_stat->is_connected = 0;
  }
  }

  return 0;
}

int _cb_scan_item(void *arg1, void *arg2, char *ssid, uint8_t bssid[6], int8_t rssi, int channel, uint16_t auth) {
  ap_list_t *ap_list;
  hal_wifi_scan_result_t *result = (hal_wifi_scan_result_t *)arg1;
  int counter = *(int *)arg2;

  if (!(counter < result->ap_num)) {
    printf("break loop now: counter=%d, result->ap_num=%d\n", counter, result->ap_num);
    /*break loop now*/
    return 1;
  }

  printf("_cb_scan_item: ssid=%s, rssi=%d\n", ssid, rssi);

  ap_list = &(result->ap_list[counter]);
  memcpy(ap_list->ssid, ssid, sizeof(ap_list->ssid));
  ap_list->ap_power = rssi;

  counter++;
  *(int *)arg2 = counter;

  return 0;
}

int _cb_scan_item_adv(void *arg1, void *arg2, char *ssid, uint8_t bssid[6], int8_t rssi, int channel, uint16_t auth) {
  ap_list_adv_t *ap_list;
  hal_wifi_scan_result_adv_t *result = (hal_wifi_scan_result_adv_t *)arg1;
  int counter = *(int *)arg2;

  //printf("counter=%d, result->ap_num=%d\n", counter, result->ap_num);
  if (!(counter < result->ap_num)) {
    //printf("break loop now: counter=%d, result->ap_num=%d\n", result->ap_num);
    /*break loop now*/
    return 1;
  }

  printf("_cb_scan_item_adv: ssid=%s, rssi=%d\n", ssid, rssi);

  ap_list = &(result->ap_list[counter]);
  memcpy(ap_list->ssid, ssid, sizeof(ap_list->ssid));
  ap_list->ap_power = rssi;
  memcpy(ap_list->bssid, bssid, sizeof(ap_list->bssid));
  ap_list->channel = channel;
  ap_list->security = auth;

  printf("!!!!ssid=%s, rssi=%d, channel=%d\n", ap_list->ssid, ap_list->ap_power, ap_list->channel);

  counter++;
  *(int *)arg2 = counter;

  return 0;
}

void _scanlist_dump(hal_wifi_module_t *m, scan_type_t t) {
  int i, ap_num;
  hal_wifi_scan_result_t *result = NULL;
  hal_wifi_scan_result_adv_t *result_adv = NULL;

  if (t != SCAN_NORMAL && t != SCAN_ADV)
    return;

  if (wifi_mgmr_cli_scanlist_num(&ap_num)) {
    return;
  }

  printf("---------------->ap_num=%d\n", ap_num);

  if (t == SCAN_NORMAL) {
    result = aos_malloc(sizeof(hal_wifi_scan_result_t));
    if (!result)
      goto end;
    result->ap_num = ap_num;
    result->ap_list = aos_malloc(ap_num * sizeof(ap_list_t));
    if (!result->ap_list)
      goto end;
  } else {
    result_adv = aos_malloc(sizeof(hal_wifi_scan_result_adv_t));
    if (!result_adv)
      goto end;
    result_adv->ap_num = ap_num;
    result_adv->ap_list = aos_malloc(ap_num * sizeof(ap_list_adv_t));
    if (!result_adv->ap_list)
      goto end;
  }

  i = 0;
  if (t == SCAN_NORMAL) {
    wifi_mgmr_cli_scanlist_foreach(_cb_scan_item, result, &i);
    result->ap_num = i; //Re-fix ap_num just in case
  } else {
    wifi_mgmr_cli_scanlist_foreach(_cb_scan_item_adv, result_adv, &i);
    result_adv->ap_num = i; //Re-fix ap_num just in case
  }

  if (t == SCAN_NORMAL) {
    if (m->ev_cb && m->ev_cb->scan_compeleted) {
      printf("start_scan: call scan_compeleted\n");
      m->ev_cb->scan_compeleted(m, result, NULL);
    }
  } else {
    if (m->ev_cb && m->ev_cb->scan_adv_compeleted) {
      printf("start_scan_adv: call scan_adv_compeleted\n");
      m->ev_cb->scan_adv_compeleted(m, result_adv, NULL);
    }
  }

end:
  if (result) {
    if (result->ap_list)
      aos_free(result->ap_list);
    aos_free(result);
  }
  if (result_adv) {
    if (result_adv->ap_list)
      aos_free(result_adv->ap_list);
    aos_free(result_adv);
  }
}

static void scan_done_cb(void *data, void *param) {
  g_scan_state = SCAN_STATE_DONE;
}

static void start_scan(hal_wifi_module_t *m)
{
  blog_info("%s start\r\n", __func__);

  while (SCAN_STATE_IDLE != g_scan_state) {
      printf("g_scan_state s = %d\r\n", g_scan_state);
      aos_msleep(10);
  }
  g_scan_state = SCAN_STATE_BUSY;

  /*first we trigger scan*/
  wifi_mgmr_scan(NULL, scan_done_cb);

  while (SCAN_STATE_DONE != g_scan_state) {
      printf("g_scan_state e = %d\r\n", g_scan_state);
      aos_msleep(10);
  }

  _scanlist_dump(m, SCAN_NORMAL);
  g_scan_state = SCAN_STATE_IDLE;
  blog_info("%s done\r\n", __func__);
}

static void start_scan_adv(hal_wifi_module_t *m)
{
  blog_info("%s start\r\n", __func__);

  while (SCAN_STATE_IDLE != g_scan_state) {
      printf("start_scan_adv s = %d\r\n", g_scan_state);
      aos_msleep(10);
  }
  g_scan_state = SCAN_STATE_BUSY;

  /*first we trigger scan*/
  wifi_mgmr_scan(NULL, scan_done_cb);

  while (SCAN_STATE_DONE != g_scan_state) {
      printf("start_scan_adv e = %d\r\n", g_scan_state);
      aos_msleep(10);
  }

  _scanlist_dump(m, SCAN_ADV);
  g_scan_state = SCAN_STATE_IDLE;
  blog_info("%s done\r\n", __func__);
}

static int power_off(hal_wifi_module_t *m) {
  printf("[TG7100C] TODO power_off\r\n");
  return 0;
}

static int power_on(hal_wifi_module_t *m) {
  printf("[TG7100C] TODO power_on\r\n");
  return 0;
}

static int suspend(hal_wifi_module_t *m) {
  int state;

  printf("[TG7100C] TODO suspend\r\n");
  if (NULL == m) {
    printf("wifi_start: invalid parameter\n");
    return -1;
  }

  blog_info("%s\r\n", __func__);

  is_suspend = 1;

  wifi_mgmr_state2_get(&state);
  
  if (state == WIFI_STATE_SNIFFER) {
    wifi_mgmr_sniffer_disable();
    wifi_mgmr_sniffer_unregister(NULL);
  }
  
  if (WIFI_STATE_AP_IS_ENABLED(state)) {
    printf("first we should stop ap mode\n");
    wifi_mgmr_ap_stop(wifi_interface);
  }
  
  wifi_mgmr_sta_autoconnect_disable();
  
  while (state != WIFI_STATE_IDLE) {
    wifi_mgmr_state2_get(&state);
    if (state == WIFI_STATE_DISCONNECT || state == WIFI_STATE_CONNECTED_IP_GOT) {
        wifi_mgmr_sta_disconnect();
        aos_msleep(1000);
        wifi_mgmr_sta_disable(wifi_interface);
        aos_msleep(500);
    }
    aos_msleep(1000); 
  }

  is_suspend = 0;

  return 0;
}

static int suspend_station(hal_wifi_module_t *m) {
  int state;

  printf("[TG7100C] TODO suspend_station\r\n");
  if (NULL == m) {
    printf("wifi_start: invalid parameter\n");
    return -1;
  }

  blog_info("%s\r\n", __func__);

  is_suspend = 1;

  wifi_mgmr_state2_get(&state);
  
  if (state == WIFI_STATE_SNIFFER) {
    wifi_mgmr_sniffer_disable();
    wifi_mgmr_sniffer_unregister(NULL);
  }
  
  if (WIFI_STATE_AP_IS_ENABLED(state)) {
    printf("first we should stop ap mode\n");
    wifi_mgmr_ap_stop(wifi_interface);
  }
  
  wifi_mgmr_sta_autoconnect_disable();
  
  while (state != WIFI_STATE_IDLE) {
    wifi_mgmr_state2_get(&state);
    if (state == WIFI_STATE_DISCONNECT || state == WIFI_STATE_CONNECTED_IP_GOT) {
        wifi_mgmr_sta_disconnect();
        aos_msleep(1000);
        wifi_mgmr_sta_disable(wifi_interface);
        aos_msleep(500);
    }
    aos_msleep(1000); 
  }

  is_suspend = 0;

  return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m) {
  printf("[TG7100C] TODO suspend_soft_ap\r\n");
  return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch) {
  if (wifi_mgmr_channel_set(ch, 0)) {
    return -1;
  }

  return 0;
}

static void sniffer_cb(void *env, uint8_t *pkt, int len, int rssi) {
  hal_wifi_link_info_t info;

  info.rssi = rssi; //TODO add rssi in sniffer cb
  if (cb_monitor) {
    cb_monitor(pkt, len, &info); // exclude wifi fcs
  }
}

static void sniffer_mgmt(void *env, uint8_t *pkt, int len, int rssi) {
  hal_wifi_link_info_t info;

  info.rssi = rssi; //TODO add rssi in sniffer cb
  if (cb_mgmt) {
    //printf("----------------------------------------- sniffer mgmt cb %d\r\n", len);
    cb_mgmt(pkt, len, &info); // exclude wifi fcs
  }
}

static void start_monitor(hal_wifi_module_t *m) {
  blog_info("%s\r\n", __func__);
  wifi_mgmr_sta_disconnect();
  aos_msleep(1000);
  wifi_mgmr_sta_disable(NULL);
  aos_msleep(500);
  wifi_mgmr_sta_autoconnect_disable();
  wifi_mgmr_sniffer_enable();
  wifi_mgmr_sniffer_register(NULL, sniffer_cb);
}

static void stop_monitor(hal_wifi_module_t *m) {
  blog_info("%s\r\n", __func__);
  wifi_mgmr_sniffer_unregister(NULL);
  cb_monitor = NULL;
  wifi_mgmr_sniffer_disable();
}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn) {
  cb_monitor = fn;
}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn) {
  //printf("[1] ----------------------------------------- register sniffer mgmt cb %p\r\n", fn);
  cb_mgmt = fn;
  wifi_mgmr_sniffer_register(NULL, sniffer_mgmt);
}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len) {
  int i;

#if 0
    printf("------------------------- raw send pakt, buf %p, len %d\r\n", buf, len);
    for (i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    puts("\r\n");
#endif
  wifi_mgmr_raw_80211_send(buf, len);
}

void NetCallback(hal_wifi_ip_stat_t *pnet) {
  if (sim_aos_wifi_tg7100c.ev_cb == NULL)
    return;
  if (sim_aos_wifi_tg7100c.ev_cb->ip_got == NULL)
    return;

  sim_aos_wifi_tg7100c.ev_cb->ip_got(&sim_aos_wifi_tg7100c, pnet, NULL);
}

static void WifiStatusHandler(int status, unsigned long reason_code) {
  if (sim_aos_wifi_tg7100c.ev_cb == NULL)
    return;
  if (sim_aos_wifi_tg7100c.ev_cb->stat_chg == NULL)
    return;
  printf("event %d notify to netmgr, reason_code: %ld\r\n", status, reason_code);
  g_reason_code = reason_code;
  sim_aos_wifi_tg7100c.ev_cb->stat_chg(&sim_aos_wifi_tg7100c, status, &g_reason_code);
}

void start_debug_mode(hal_wifi_module_t *m) {
  /*Nothing here*/
}

void stop_debug_mode(hal_wifi_module_t *m) {
  /*Nothing here*/
}

static int start_ap(hal_wifi_module_t *m, const char *ssid, const char *passwd, int interval, int hide) {
  blog_info("%s\r\n", __func__);
  wifi_interface = wifi_mgmr_ap_enable();
  wifi_mgmr_ap_start(wifi_interface, (char *)ssid, 0, (char *)passwd, 9);

  return 0;
}

static int stop_ap(hal_wifi_module_t *m) {
  blog_info("%s\r\n", __func__);
  wifi_mgmr_ap_stop(wifi_interface);

  return 0;
}

static int coex_mode_set(hal_wifi_module_t *m, hal_wifi_coexmode_t coex_mode)
{
  int ret = 0;
  switch (coex_mode)
  {
  case WIFI_COEXMODE_NONE: /* wifi combo chip none coexistence mode */
  {
    if (wifi_mgmr_ps_mode_get() == 1)
    {
      wifi_mgmr_exit_ps_mode();
    }
  }
  break;

  case WIFI_COEXMODE_WIFI_ONLY: /* wifi combo chip only wifi mode, ble mesh force closed */
  {
    if (wifi_mgmr_ps_mode_get() == 1)
    {
      wifi_mgmr_exit_ps_mode();
    }
    coex_pta_force_autocontrol_set((void *)1);
  }
  break;

  case WIFI_COEXMODE_BLE_ONLY: /* wifi combo chip only ble&mesh mode, wifi force closed */
  {
    if (wifi_mgmr_ps_mode_get() == 1)
    {
      wifi_mgmr_exit_ps_mode();
    }
    coex_pta_force_autocontrol_set((void *)2);
  }
  break;

  case WIFI_COEXMODE_WIFI_MESH: /* wifi combo chip ble-mesh & wifi RF coexistence */
  {
    if (wifi_mgmr_ps_mode_get() == 0)
    {
      wifi_mgmr_enter_ps_mode();
    }
  }
  break;

  case WIFI_COEXMODE_MAX:
  default:
  {
    ret = -1;
  }
  break;
  }

  if (ret < 0)
  {
    printf("%s, invalid coexistence mode or mode set fail\r\n", __func__);
  }
  return ret;
}

static int coex_param_set(hal_wifi_module_t *m, hal_wifi_coexmode_t coex_mode, uint16_t param1, uint16_t param2)
{
  int ret = 0;
  switch (coex_mode)
  {
  case WIFI_COEXMODE_NONE: /* wifi combo chip none coexistence mode */
  {
    ret = -1;
  }
  break;

  case WIFI_COEXMODE_WIFI_ONLY: /* wifi combo chip only wifi mode, ble mesh force closed */
  {
    ret = -1;
  }
  break;

  case WIFI_COEXMODE_BLE_ONLY: /* wifi combo chip only ble&mesh mode, wifi force closed */
  {
    ret = -1;
  }
  break;

  case WIFI_COEXMODE_WIFI_MESH: /* wifi combo chip ble-mesh & wifi RF coexistence */
  {
    if (wifi_mgmr_ps_mode_get() == 1)
    {
      ret = wifi_mgmr_set_wifi_active_time(param1);
    }
    else
    {
      ret = -1;
    }
  }
  break;

  case WIFI_COEXMODE_MAX:
  default:
  {
    ret = -1;
  }
  break;
  }

  if (ret < 0)
  {
    printf("%s %d param not support or set fail\r\n", __func__, coex_mode);
  }
  return ret;
}

hal_wifi_module_t sim_aos_wifi_tg7100c = {
    .base.name = "sim_aos_wifi_tg7100c",
    .init = wifi_init,
    .get_mac_addr = wifi_get_mac_addr,
    .start = wifi_start,
    .start_adv = wifi_start_adv,
    .get_ip_stat = get_ip_stat,
    .get_link_stat = get_link_stat,
    .start_scan = start_scan,
    .start_scan_adv = start_scan_adv,
    .power_off = power_off,
    .power_on = power_on,
    .suspend = suspend,
    .suspend_station = suspend_station,
    .suspend_soft_ap = suspend_soft_ap,
    .set_channel = set_channel,
    .start_monitor = start_monitor,
    .stop_monitor = stop_monitor,
    .start_ap = start_ap,
    .stop_ap = stop_ap,
    .register_monitor_cb = register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,

    /* wifi & ble coexistence related */
    .coex_mode_set = coex_mode_set,
    .coex_param_set = coex_param_set,

    /* debug related */
    .start_debug_mode = start_debug_mode,
    .stop_debug_mode = stop_debug_mode
};
