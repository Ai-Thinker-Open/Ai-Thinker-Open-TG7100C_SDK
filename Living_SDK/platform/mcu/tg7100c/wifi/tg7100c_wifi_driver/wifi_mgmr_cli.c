#include <stdio.h>
#include <string.h>
//#include <FreeRTOS.h>
//#include <task.h>
#include <aos/cli.h>

#include <blog.h>
#include <bl_wifi.h>
#include <hal_sys.h>
#include <tg710x_fw_api.h>
#include <wifi_mgmr.h>
#include <wifi_mgmr_api.h>
#include <utils_hexdump.h>
#include <wifi_mgmr_ext.h>

#if 0
#define WIFI_AP_DATA_RATE_1Mbps      0x00
#define WIFI_AP_DATA_RATE_2Mbps      0x01
#define WIFI_AP_DATA_RATE_5_5Mbps    0x02
#define WIFI_AP_DATA_RATE_11Mbps     0x03
#define WIFI_AP_DATA_RATE_6Mbps      0x0b
#define WIFI_AP_DATA_RATE_9Mbps      0x0f
#define WIFI_AP_DATA_RATE_12Mbps     0x0a
#define WIFI_AP_DATA_RATE_18Mbps     0x0e
#define WIFI_AP_DATA_RATE_24Mbps     0x09
#define WIFI_AP_DATA_RATE_36Mbps     0x0d
#define WIFI_AP_DATA_RATE_48Mbps     0x08
#define WIFI_AP_DATA_RATE_54Mbps     0x0c

struct wifi_ap_data_rate {
    uint8_t data_rate;
    const char *val;
};

static const struct wifi_ap_data_rate data_rate_list[] = {
    {WIFI_AP_DATA_RATE_1Mbps, "1.0 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_2Mbps, "2.0 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_5_5Mbps, "5.5 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_11Mbps, "11.0 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_6Mbps, "6.0 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_9Mbps, "9.0 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_12Mbps, "12.0 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_18Mbps, "18.0 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_24Mbps, "24.0 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_36Mbps, "36.0 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_48Mbps, "48.0 Mbit/s, 20Mhz"},
    {WIFI_AP_DATA_RATE_54Mbps, "54.0 Mbit/s, 20Mhz"},
};

static unsigned char char_to_hex(char asccode)
{
    unsigned char ret;

    if('0'<=asccode && asccode<='9')
        ret=asccode-'0';
    else if('a'<=asccode && asccode<='f')
        ret=asccode-'a'+10;
    else if('A'<=asccode && asccode<='F')
        ret=asccode-'A'+10;
    else
        ret=0;

    return ret;
}

static void chan_str_to_hex(uint8_t *sta_num, char *sta_str)
{
    int i, str_len, base=1;
    uint16_t val = 0;
    char *q;

    str_len = strlen(sta_str);
    q = sta_str;
    q[str_len] = '\0';
    for (i=0; i< str_len; i++) {
        val = val + char_to_hex(q[str_len-1-i]) * base;
        base = base * 10;
    }
    (*sta_num) = val;
    printf("sta_str: %s, str_len: %d, sta_num: %d, q: %s\r\n", sta_str, str_len, (*sta_num), q);

}

static void wifi_ap_sta_list_get_cmd(char *buf, int len, int argc, char **argv)
{
    int state = WIFI_STATE_UNKNOWN;
    uint8_t sta_cnt = 0, i, j;
    struct wifi_sta_basic_info sta_info;
    long long sta_time;
    uint8_t index = 0;

    wifi_mgmr_state_get(&state);
    if (!WIFI_STATE_AP_IS_ENABLED(state)){
        printf("wifi AP is not enabled, state = %d\r\n", state);
        return;
    }

    wifi_mgmr_ap_sta_cnt_get(&sta_cnt);
    if (!sta_cnt){
        printf("no sta connect current AP, sta_cnt = %d\r\n", sta_cnt);
        return;
    }

    memset(&sta_info, 0, sizeof(struct wifi_sta_basic_info));
    printf("sta list:\r\n");
    printf("-----------------------------------------------------------------------------------\r\n");
    printf("No.      StaIndex      Mac-Address       Signal      DateRate            TimeStamp\r\n");
    printf("-----------------------------------------------------------------------------------\r\n");
    for(i = 0;i < sta_cnt;i++){
        wifi_mgmr_ap_sta_info_get(&sta_info, i);
        if (!sta_info.is_used || (sta_info.sta_idx == 0xef)){
            continue;
        }

        sta_time = (long long)sta_info.tsfhi;
        sta_time = (sta_time << 32) | sta_info.tsflo;

        for(j = 0;j < sizeof(data_rate_list)/sizeof(data_rate_list[0]);j++) {
            if(data_rate_list[j].data_rate == sta_info.data_rate) {
                index = j;
                break;
            }
        }

        printf(" %u       "
            "   %u        "
            "%02X:%02X:%02X:%02X:%02X:%02X    "
            "%d      "
            "%s      "
            "0x%llx"
            "\r\n",
            i,
            sta_info.sta_idx,
            sta_info.sta_mac[0],
            sta_info.sta_mac[1],
            sta_info.sta_mac[2],
            sta_info.sta_mac[3],
            sta_info.sta_mac[4],
            sta_info.sta_mac[5],
            sta_info.rssi,
            data_rate_list[index].val,
            sta_time
        );
    }
}

static void wifi_ap_sta_delete_cmd(char *buf, int len, int argc, char **argv)
{
    int state = WIFI_STATE_UNKNOWN;
    uint8_t sta_cnt = 0;
    struct wifi_sta_basic_info sta_info;
    uint8_t sta_num = 0;

    if (2 != argc) {
        printf("[USAGE]: %s sta_num\r\n", argv[0]);
        return;
    }

    wifi_mgmr_state_get(&state);
    if (!WIFI_STATE_AP_IS_ENABLED(state)){
        printf("wifi AP is not enabled, state = %d\r\n", state);
        return;
    }

    printf("Delete Sta No.%s \r\n", argv[1]);
    chan_str_to_hex(&sta_num, argv[1]);
    printf("sta num = %d \r\n", sta_num);

    wifi_mgmr_ap_sta_cnt_get(&sta_cnt);
    if (!sta_cnt || (sta_num > sta_cnt)){
        printf("no valid sta in list or sta idx(%d) is invalid\r\n", sta_cnt);
        return;
    }

    memset(&sta_info, 0, sizeof(struct wifi_sta_basic_info));
    wifi_mgmr_ap_sta_info_get(&sta_info, sta_num);
    if (!sta_info.is_used || (sta_info.sta_idx == 0xef)){
        printf("No.%d sta is invalid\r\n", sta_num);
        return;
    }

    printf("sta info: No.%u,"
        "sta_idx = %u,"
        "mac = %02X:%02X:%02X:%02X:%02X:%02X,"
        "rssi = %d"
        "\r\n",
        sta_num,
        sta_info.sta_idx,
        sta_info.sta_mac[0],
        sta_info.sta_mac[1],
        sta_info.sta_mac[2],
        sta_info.sta_mac[3],
        sta_info.sta_mac[4],
        sta_info.sta_mac[5],
        sta_info.rssi
    );
    wifi_mgmr_ap_sta_delete(sta_info.sta_idx);
}

static void wifi_edca_dump_cmd(char *buf, int len, int argc, char **argv)
{
    uint8_t aifs = 0, cwmin = 0, cwmax = 0;
    uint16_t txop = 0;

    puts("EDCA Statistic:\r\n");

    bl60x_edca_get(API_AC_BK, &aifs, &cwmin, &cwmax, &txop);
    puts("  AC_BK:");
    printf("aifs %3u, cwmin %3u, cwmax %3u, txop %4u\r\n",
        aifs, cwmin, cwmax, txop
    );

    bl60x_edca_get(API_AC_BE, &aifs, &cwmin, &cwmax, &txop);
    puts("  AC_BE:");
    printf("aifs %3u, cwmin %3u, cwmax %3u, txop %4u\r\n",
        aifs, cwmin, cwmax, txop
    );

    bl60x_edca_get(API_AC_VI, &aifs, &cwmin, &cwmax, &txop);
    puts("  AC_VI:");
    printf("aifs %3u, cwmin %3u, cwmax %3u, txop %4u\r\n",
        aifs, cwmin, cwmax, txop
    );

    bl60x_edca_get(API_AC_VO, &aifs, &cwmin, &cwmax, &txop);
    puts("  AC_VO:");
    printf("aifs %3u, cwmin %3u, cwmax %3u, txop %4u\r\n",
        aifs, cwmin, cwmax, txop
    );
}

int wifi_mgmr_cli_powersaving_on()
{
    wifi_mgmr_api_fw_powersaving(2);
    return 0;
}
#endif

int wifi_mgmr_cli_scanlist_num(int *num) {
  int i, j;

  for (i = 0, j = 0; i < sizeof(wifiMgmr.scan_items) / sizeof(wifiMgmr.scan_items[0]); i++) {
    if (wifiMgmr.scan_items[i].is_used) {
      j++;
    }
  }
  *num = j;

  return 0;
}

int wifi_mgmr_cli_scanlist_foreach(int (*cb)(void *arg1, void *arg2, char *ssid, uint8_t bssid[6], int8_t rssi, int channel, uint16_t auth), void *arg1, void *arg2) {
  int i;

  for (i = 0; i < sizeof(wifiMgmr.scan_items) / sizeof(wifiMgmr.scan_items[0]); i++) {
    if (wifiMgmr.scan_items[i].is_used) {
      if (cb(arg1, arg2, wifiMgmr.scan_items[i].ssid,
             wifiMgmr.scan_items[i].bssid,
             wifiMgmr.scan_items[i].rssi,
             wifiMgmr.scan_items[i].channel,
             wifiMgmr.scan_items[i].auth)) {
        /*break on non zero*/
        break;
      }
    }
  }

  return 0;
}

#if 0
int wifi_mgmr_cli_scanlist(void)
{
    int i;

    printf("cached scan list\r\n");
    printf("****************************************************************************************************\r\n");
    for (i = 0; i < sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0]); i++) {
        if (wifiMgmr.scan_items[i].is_used) {
            printf("index[%02d]: channel %02u, bssid %02X:%02X:%02X:%02X:%02X:%02X, rssi %3d, ppm abs:rel %3d : %3d, auth %15s, pair_key:%s, grp_key:%s, SSID %s\r\n",
                    i,
                    wifiMgmr.scan_items[i].channel,
                    wifiMgmr.scan_items[i].bssid[0],
                    wifiMgmr.scan_items[i].bssid[1],
                    wifiMgmr.scan_items[i].bssid[2],
                    wifiMgmr.scan_items[i].bssid[3],
                    wifiMgmr.scan_items[i].bssid[4],
                    wifiMgmr.scan_items[i].bssid[5],
                    wifiMgmr.scan_items[i].rssi,
                    wifiMgmr.scan_items[i].ppm_abs,
                    wifiMgmr.scan_items[i].ppm_rel,
                    wifi_mgmr_auth_to_str(wifiMgmr.scan_items[i].auth),
                    wifi_mgmr_cipher_to_str(wifiMgmr.scan_items[i].rsn_ucstCipher),
                    wifi_mgmr_cipher_to_str(wifiMgmr.scan_items[i].rsn_mcstCipher),
                    wifiMgmr.scan_items[i].ssid
            );
        } else {
            printf("index[%02d]: empty\r\n", i);
        }
    }
    printf("----------------------------------------------------------------------------------------------------\r\n");
    return 0;
}

static void cmd_rf_dump(char *buf, int len, int argc, char **argv)
{
    //bl60x_fw_dump_data();
}

static void wifi_capcode_cmd(char *buf, int len, int argc, char **argv)
{
    int capcode = 0;

    if (2 != argc && 1 != argc) {
        printf("Usage: %s capcode\r\n", argv[0]);
        return;
    }

    /*get capcode*/
    if (1 == argc) {
        printf("Capcode %u is being used\r\n", hal_sys_capcode_get());
        return;
    }

    /*set capcode*/
    capcode = atoi(argv[1]);
    printf("Setting capcode to %d\r\n", capcode);

    if (capcode > 0) {
        hal_sys_capcode_update(capcode, capcode);
    }
}

static void wifi_scan_cmd(char *buf, int len, int argc, char **argv)
{
    wifi_mgmr_scan(NULL, NULL);
}

static void wifi_ip_info(char *buf, int len, int argc, char **argv)
{
    ip4_addr_t ip, gw, mask;
    wifi_mgmr_sta_ip_get(&ip.addr, &gw.addr, &mask.addr);
    printf("IP  :%s \r\n", ip4addr_ntoa(&ip) );
    printf("GW  :%s \r\n", ip4addr_ntoa(&gw));
    printf("MASK:%s \r\n", ip4addr_ntoa(&mask));
}

static uint8_t packet_raw[] = {
    0x48, 0x02,
    0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00
};

static void cmd_wifi_raw_send(char *buf, int len, int argc, char **argv)
{
    static uint32_t seq = 0;

    packet_raw[sizeof(packet_raw) - 2] = ((seq << 4) & 0xFF);
    packet_raw[sizeof(packet_raw) - 1] = (((seq << 4) & 0xFF00) >> 8);
    seq++;

    if (wifi_mgmr_raw_80211_send(packet_raw, sizeof(packet_raw))) {
        puts("Raw send failed\r\n");
    } else {
        puts("Raw send succeed\r\n");
    }
}

static void wifi_disconnect_cmd(char *buf, int len, int argc, char **argv)
{
    wifi_mgmr_sta_disconnect();
    /*XXX Must make sure sta is already disconnect, otherwise sta disable won't work*/
    krhino_task_sleep(1000);
    wifi_mgmr_sta_disable(NULL);
}

static void wifi_connect_cmd(char *buf, int len, int argc, char **argv)
{
    wifi_interface_t wifi_interface;

    if (3 != argc) {
        printf("[USAGE]: %s [ssid] [password]\r\n", argv[0]);
        return;
    }

    wifi_interface = wifi_mgmr_sta_enable();
    wifi_mgmr_sta_connect(wifi_interface, argv[1], argv[2], NULL, NULL, 0, 0);
}

static void wifi_sta_get_state_cmd(char *buf, int len, int argc, char **argv)
{
    int state = 0;

    wifi_mgmr_state_get(&state);

    printf("%s:wifi state = 0x%x\r\n", __func__, state);
    if(state == WIFI_STATE_UNKNOWN){
        printf("wifi current state: WIFI_STATE_UNKNOWN\r\n");
    } else if(state == WIFI_STATE_IDLE) {
        printf("wifi current state: WIFI_STATE_IDLE\r\n");
    } else if(state == WIFI_STATE_CONNECTING) {
        printf("wifi current state: WIFI_STATE_CONNECTING\r\n");
    } else if(state == WIFI_STATE_CONNECTED_IP_GETTING) {
        printf("wifi current state: WIFI_STATE_CONNECTED_IP_GETTING\r\n");
    } else if(state == WIFI_STATE_CONNECTED_IP_GOT) {
        printf("wifi current state: WIFI_STATE_CONNECTED_IP_GOT\r\n");
    } else if(state == WIFI_STATE_DISCONNECT) {
        printf("wifi current state: WIFI_STATE_DISCONNECT\r\n");
    } else if(state == WIFI_STATE_WITH_AP_IDLE) {
        printf("wifi current state: WIFI_STATE_WITH_AP_IDLE\r\n");
    } else if(state == WIFI_STATE_WITH_AP_CONNECTING) {
        printf("wifi current state: WIFI_STATE_WITH_AP_CONNECTING\r\n");
    } else if(state == WIFI_STATE_WITH_AP_CONNECTED_IP_GETTING) {
        printf("wifi current state: WIFI_STATE_WITH_AP_CONNECTED_IP_GETTING\r\n");
    } else if(state == WIFI_STATE_WITH_AP_CONNECTED_IP_GOT) {
        printf("wifi current state: WIFI_STATE_WITH_AP_CONNECTED_IP_GOT\r\n");
    } else if(state == WIFI_STATE_WITH_AP_DISCONNECT) {
        printf("wifi current state: WIFI_STATE_WITH_AP_DISCONNECT\r\n");
    } else if(state == WIFI_STATE_IFDOWN) {
        printf("wifi current state: WIFI_STATE_IFDOWN\r\n");
    } else if(state == WIFI_STATE_SNIFFER) {
        printf("wifi current state: WIFI_STATE_SNIFFER\r\n");
    } else if(state == WIFI_STATE_PSK_ERROR) {
        printf("wifi current state: WIFI_STATE_PSK_ERROR\r\n");
    } else if(state == WIFI_STATE_NO_AP_FOUND) {
        printf("wifi current state: WIFI_STATE_NO_AP_FOUND\r\n");
    } else {
        printf("wifi current state: invalid\r\n");
    }
}

static void wifi_disable_autoreconnect_cmd(char *buf, int len, int argc, char **argv)
{
    wifi_mgmr_sta_autoconnect_disable();
}

static void wifi_enable_autoreconnect_cmd(char *buf, int len, int argc, char **argv)
{
    wifi_mgmr_sta_autoconnect_enable();
}

static void wifi_rc_fixed_enable(char *buf, int len, int argc, char **argv)
{
    uint8_t mcs = 0;
    uint8_t gi = 0;
    uint16_t rc = 0x1000; //format mode is HT_MF only

    if (argc != 3) {
        printf("rc_fix_en [MCS] [GI]");
        return;
    }
    mcs = atoi(argv[1]);
    gi = atoi(argv[2]);

    rc |= gi << 9 | mcs;

    wifi_mgmr_rate_config(rc);
}

static void wifi_rc_fixed_disable(char *buf, int len, int argc, char **argv)
{
    uint16_t rc = 0xFFFF;

    wifi_mgmr_rate_config(rc);
}
}

#if 0
static void wifi_capcode_update(char *buf, int len, int argc, char **argv)
{
    uint8_t cap_in, cap_out;

    if (argc == 1) {
        bl60x_fw_xtal_capcode_get(&cap_in, &cap_out);
        printf("[RF] [CAP] Dump capcode in:out %u:%u\r\n", cap_in, cap_out);
        return;
    }
    if (argc != 3) {
        printf("%s [capcode_in] [capcode_out]\r\n", argv[0]);
        return;
    }

    bl60x_fw_xtal_capcode_get(&cap_in, &cap_out);
    printf("[RF] [CAP] Dump capcode in:out %u:%u\r\n", cap_in, cap_out);
    cap_in = atoi(argv[1]);
    cap_out = atoi(argv[2]);
    printf("[RF] [CAP] Updating capcode to in:out %u:%u\r\n", cap_in, cap_out);
    bl60x_fw_xtal_capcode_update(cap_in, cap_out);
    bl60x_fw_xtal_capcode_get(&cap_in, &cap_out);
    printf("[RF] [CAP] Dump Again capcode in:out %u:%u\r\n", cap_in, cap_out);
}
#endif

static void wifi_power_saving_on_cmd(char *buf, int len, int argc, char **argv)
{
    wifi_mgmr_sta_powersaving(1);
}

static void wifi_power_saving_off_cmd(char *buf, int len, int argc, char **argv)
{
    wifi_mgmr_sta_powersaving(0);
}

static void sniffer_cb(void *env, uint8_t *pkt, int len)
{
    static unsigned int sniffer_counter, sniffer_last;
    static unsigned int last_tick;

    (void)sniffer_last;
    (void)sniffer_counter;

    sniffer_counter++;
    if ((int)krhino_sys_tick_get() - (int)last_tick > 10 * 1000) {
        blog_info("[SNIFFER] PKT Number is %d\r\n",
                (int)sniffer_counter - (int)sniffer_last
        );
        last_tick = krhino_sys_tick_get();
        sniffer_last = sniffer_counter;
    }
}

static void wifi_mon_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc > 1) {
        blog_debug("Enable Sniffer Mode\r\n");
        wifi_mgmr_sniffer_enable();
    } else {
        blog_debug("Register Sniffer cb\r\n");
        wifi_mgmr_sniffer_register(NULL, sniffer_cb);
    }
}

static void wifi_sniffer_on_cmd(char *buf, int len, int argc, char **argv)
{
    wifi_mgmr_sniffer_enable();
    wifi_mgmr_sniffer_register(NULL, sniffer_cb);
}

static void wifi_sniffer_off_cmd(char *buf, int len, int argc, char **argv)
{
    wifi_mgmr_sniffer_disable();
    wifi_mgmr_sniffer_unregister(NULL);
}

static void cmd_wifi_ap_start(char *buf, int len, int argc, char **argv)
{
    uint8_t mac[6];
    char ssid_name[32];
    int channel;
    wifi_interface_t wifi_interface;

    memset(mac, 0, sizeof(mac));
    bl_wifi_mac_addr_get(mac);
    memset(ssid_name, 0, sizeof(ssid_name));
    snprintf(ssid_name, sizeof(ssid_name), "BL60X_uAP_%02X%02X%02X", mac[3], mac[4], mac[5]);
    ssid_name[sizeof(ssid_name) - 1] = '\0';

    wifi_interface = wifi_mgmr_ap_enable();
    if (1 == argc) {
        /*no password when only one param*/
        wifi_mgmr_ap_start(wifi_interface, ssid_name, 0, NULL, 1);
    } else {
        /*hardcode password*/
        channel = atoi(argv[1]);
        if (channel <=0 || channel > 11) {
            return;
        }
        wifi_mgmr_ap_start(wifi_interface, ssid_name, 0, "12345678", channel);
    }
}

static void cmd_wifi_ap_stop(char *buf, int len, int argc, char **argv)
{
    wifi_mgmr_ap_stop(NULL);
    printf("--->>> cmd_wifi_ap_stop\r\n");
}

static void cmd_wifi_dump(char *buf, int len, int argc, char **argv)
{
    CPSR_ALLOC();
    if (argc > 1) {
        puts("[CLI] Dump statistic use forced mode\r\n");
        RHINO_CRITICAL_ENTER();
        bl60x_fw_dump_statistic(1);
        RHINO_CRITICAL_EXIT();
    } else {
        puts("[CLI] Dump statistic use normal mode\r\n");
        RHINO_CRITICAL_ENTER();
        bl60x_fw_dump_statistic(0);
        RHINO_CRITICAL_EXIT();
    }
}

static void cmd_wifi_mib(char *buf, int len, int argc, char **argv)
{
void hal_mib_dump();
    hal_mib_dump();
    utils_hexdump(argv[0], 30);
}
#endif

static int pkt_counter = 0;
int wifi_mgmr_ext_dump_needed()
{
    if (pkt_counter > 0) {
        pkt_counter--;
        return 1;
    }
    return 0;
}

#if 0
static void cmd_dump_reset(char *buf, int len, int argc, char **argv)
{
    pkt_counter = 10;
}

void coex_wifi_rf_forece_enable(int enable);
static void cmd_wifi_coex_rf_force_on(char *buf, int len, int argc, char **argv)
{
    coex_wifi_rf_forece_enable(1);
}

static void cmd_wifi_coex_rf_force_off(char *buf, int len, int argc, char **argv)
{
    coex_wifi_rf_forece_enable(0);
}

void coex_wifi_pti_forece_enable(int enable);
static void cmd_wifi_coex_pti_force_on(char *buf, int len, int argc, char **argv)
{
    coex_wifi_pti_forece_enable(1);
}

static void cmd_wifi_coex_pti_force_off(char *buf, int len, int argc, char **argv)
{
    coex_wifi_pti_forece_enable(0);
}

void coex_wifi_pta_forece_enable(int enable);
static void cmd_wifi_coex_pta_force_on(char *buf, int len, int argc, char **argv)
{
    coex_wifi_pta_forece_enable(1);
}

static void cmd_wifi_coex_pta_force_off(char *buf, int len, int argc, char **argv)
{
    coex_wifi_pta_forece_enable(0);
}

static void cmd_wifi_state_get(char *buf, int len, int argc, char **argv)
{
    int state = WIFI_STATE_UNKNOWN;
    wifi_mgmr_state_get(&state);

    switch (state) {
        case WIFI_STATE_UNKNOWN:
            printf("wifi state unknown\r\n");
            break;
        case WIFI_STATE_IDLE:
            printf("wifi state idle\r\n");
            break;
        case WIFI_STATE_CONNECTING:
            printf("wifi state connecting\r\n");
            break;
        case WIFI_STATE_CONNECTED_IP_GETTING:
            printf("wifi state connected ip getting\r\n");
            break;
        case WIFI_STATE_CONNECTED_IP_GOT:
            printf("wifi state connected ip got\r\n");
            break;
        case WIFI_STATE_DISCONNECT:
            printf("wifi state disconnect\r\n");
            break;
        case WIFI_STATE_WITH_AP_IDLE:
            printf("wifi state with ap idle\r\n");
            break;
        case WIFI_STATE_WITH_AP_CONNECTING:
            printf("wifi state with ap connecting\r\n");
            break;
        case WIFI_STATE_WITH_AP_CONNECTED_IP_GETTING:
            printf("wifi state with ap connected ip getting\r\n");
            break;
        case WIFI_STATE_WITH_AP_CONNECTED_IP_GOT:
            printf("wifi state with ap connected ip got\r\n");
            break;
        case WIFI_STATE_WITH_AP_DISCONNECT:
            printf("wifi state with ap disconnect\r\n");
            break;
        case WIFI_STATE_IFDOWN:
            printf("wifi state ifdown\r\n");
            break;
        case WIFI_STATE_SNIFFER:
            printf("wifi state sniffer\r\n");
            break;
        case WIFI_STATE_PSK_ERROR:
            printf("wifi state psk error\r\n");
            break;
        case WIFI_STATE_NO_AP_FOUND:
            printf("wifi state no ap found\r\n");
            break;
        default:
            break;
    }
}

// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
#ifndef STATIC_CLI_CMD_ATTRIBUTE 
#define STATIC_CLI_CMD_ATTRIBUTE 
#endif
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "rf_dump", "rf dump", cmd_rf_dump},
        { "wifi_capcode", "wifi capcode", wifi_capcode_cmd},
        { "wifi_scan", "wifi scan", wifi_scan_cmd},
        { "wifi_ip_info", "wifi scan", wifi_ip_info},
        { "wifi_mon", "wifi monitor", wifi_mon_cmd},
        { "wifi_raw_send", "wifi raw send test", cmd_wifi_raw_send},
        { "wifi_sta_disconnect", "wifi station disconnect", wifi_disconnect_cmd},
        { "wifi_sta_connect", "wifi station connect", wifi_connect_cmd},
        { "wifi_sta_get_state", "wifi sta get state", wifi_sta_get_state_cmd},
        { "wifi_sta_autoconnect_enable", "wifi station enable auto reconnect", wifi_enable_autoreconnect_cmd},
        { "wifi_sta_autoconnect_disable", "wifi station disable auto reconnect", wifi_disable_autoreconnect_cmd},
        { "rc_fix_en", "wifi rate control fixed rate enable", wifi_rc_fixed_enable},
        { "rc_fix_dis", "wifi rate control fixed rate diable", wifi_rc_fixed_disable},
        { "wifi_sta_ps_on", "wifi power saving mode ON", wifi_power_saving_on_cmd},
        { "wifi_sta_ps_off", "wifi power saving mode OFF", wifi_power_saving_off_cmd},
        { "wifi_sniffer_on", "wifi sniffer mode on", wifi_sniffer_on_cmd},
        { "wifi_sniffer_off", "wifi sniffer mode off", wifi_sniffer_off_cmd},
        { "wifi_ap_start", "start Ap mode", cmd_wifi_ap_start},
        { "wifi_ap_stop", "start Ap mode", cmd_wifi_ap_stop},
        { "wifi_dump", "dump fw statistic", cmd_wifi_dump},
        { "wifi_mib", "dump mib statistic", cmd_wifi_mib},
        { "wifi_pkt", "wifi dump needed", cmd_dump_reset},
        { "wifi_coex_rf_force_on", "wifi coex RF forece on", cmd_wifi_coex_rf_force_on},
        { "wifi_coex_rf_force_off", "wifi coex RF forece off", cmd_wifi_coex_rf_force_off},
        { "wifi_coex_pti_force_on", "wifi coex PTI forece on", cmd_wifi_coex_pti_force_on},
        { "wifi_coex_pti_force_off", "wifi coex PTI forece off", cmd_wifi_coex_pti_force_off},
        { "wifi_coex_pta_force_on", "wifi coex PTA forece on", cmd_wifi_coex_pta_force_on},
        { "wifi_coex_pta_force_off", "wifi coex PTA forece off", cmd_wifi_coex_pta_force_off},
        { "wifi_sta_list", "get sta list in AP mode", wifi_ap_sta_list_get_cmd},
        { "wifi_sta_del", "delete one sta in AP mode", wifi_ap_sta_delete_cmd},
        { "wifi_edca_dump", "dump EDCA data", wifi_edca_dump_cmd},
        { "wifi_state", "get wifi_state", cmd_wifi_state_get},
};                                                                                   

int wifi_mgmr_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}
#endif

