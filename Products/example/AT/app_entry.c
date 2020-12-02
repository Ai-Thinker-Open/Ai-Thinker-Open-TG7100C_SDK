/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"
#include "iot_export.h"
#include "iot_import.h"
#include "app_entry.h"
#include "aos/kv.h"
#include "net_state_manger.h"
#include "at_main.h"
#include "vendor.h"
#include "property_handle.h"
#include "living_platform_cmds.h"

#ifdef EN_COMBO_NET
#include "combo_net.h"
#endif

#ifdef AOS_TIMER_SERVICE
#include "iot_export_timer.h"
#endif
#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

#include <k_api.h>

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
#include "ota_service.h"
#endif


static aos_task_t task_key_detect;
static aos_task_t task_msg_process;
static aos_task_t task_property_report;
static aos_task_t task_linkkit_reset;
static aos_task_t task_reboot_device;

static int is_connect = 0;
static char linkkit_started = 0;


#ifdef EN_COMBO_NET
char awss_running = 0;
#else
static char awss_running = 0;
#endif

void do_awss_active(void);

#ifdef CONFIG_PRINT_HEAP
void print_heap()
{
    extern k_mm_head *g_kmm_head;
    int               free = g_kmm_head->free_size;
    LOG("============free heap size =%d==========", free);
}
#endif

int my_get_is_connect(void)
{
    return is_connect;
}


static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    LOG("wifi_service_event(), event->code=%d", event->code);
    if (event->code == CODE_WIFI_ON_CONNECTED) {
        LOG("CODE_WIFI_ON_CONNECTED");
        uart_push_data("WIFI CONNECTED");
    } else if (event->code == CODE_WIFI_ON_DISCONNECT) {
        LOG("CODE_WIFI_ON_DISCONNECT");
        #ifdef EN_COMBO_NET
        combo_set_ap_state(COMBO_AP_DISCONNECTED);
        #endif

    } else if (event->code == CODE_WIFI_ON_CONNECT_FAILED) {
        LOG("CODE_WIFI_ON_CONNECT_FAILED");
    } else if (event->code == CODE_WIFI_ON_GOT_IP) {
        LOG("CODE_WIFI_ON_GOT_IP");
        #ifdef EN_COMBO_NET
        combo_set_ap_state(COMBO_AP_CONNECTED);
        #endif
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }
    uart_push_data("WIFI GOT IP");
    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi_service_event config.ssid %s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0) {
        // clear_wifi_ssid();
        return;
    }
    set_net_state(GOT_AP_SSID);
    #ifdef EN_COMBO_NET
    combo_ap_conn_notify();
    #endif

    if (!linkkit_started) {
#ifdef CONFIG_PRINT_HEAP
        print_heap();
#endif
#if (defined (TG7100CEVB))
        aos_task_new("linkkit", (void (*)(void *))linkkit_main, NULL, 1024 * 8);
#else
        aos_task_new("linkkit", (void (*)(void *))linkkit_main, NULL, 1024 * 6);
#endif
        linkkit_started = 1;
    }
}

static void cloud_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_YUNIO) {
        return;
    }

    LOG("cloud_service_event %d", event->code);

    if (event->code == CODE_YUNIO_ON_CONNECTED) {
        LOG("user sub and pub here");
        return;
    }

    if (event->code == CODE_YUNIO_ON_DISCONNECTED) {
    }
}

/*
 * Note:
 * the linkkit_event_monitor must not block and should run to complete fast
 * if user wants to do complex operation with much time,
 * user should post one task to do this, not implement complex operation in
 * linkkit_event_monitor
 */

static void linkkit_event_monitor(int event)
{
    // printf("\n\n\n\n### event=%d ###\n\n\n\n",event);
    switch (event) {
        case IOTX_AWSS_START: // AWSS start without enbale, just supports device discover
            // operate led to indicate user
            LOG("IOTX_AWSS_START");
            break;
        case IOTX_AWSS_ENABLE: // AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled.
            LOG("IOTX_AWSS_ENABLE");
            // operate led to indicate user
            break;
        case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
            LOG("IOTX_AWSS_LOCK_CHAN");
            // operate led to indicate user
            break;
        case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
            LOG("IOTX_AWSS_PASSWD_ERR");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_SSID_PASSWD:
            LOG("IOTX_AWSS_GOT_SSID_PASSWD");
            // operate led to indicate user
            set_net_state(GOT_AP_SSID);
            break;
        case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
            // discover, router solution)
            LOG("IOTX_AWSS_CONNECT_ADHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
            LOG("IOTX_AWSS_CONNECT_ADHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
            LOG("IOTX_AWSS_CONNECT_AHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
            LOG("IOTX_AWSS_CONNECT_AHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
            // (AP and router solution)
            LOG("IOTX_AWSS_SETUP_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
            LOG("IOTX_AWSS_CONNECT_ROUTER");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
            // router.
            LOG("IOTX_AWSS_CONNECT_ROUTER_FAIL");
            set_net_state(CONNECT_AP_FAILED);
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
            // ip address
            LOG("IOTX_AWSS_GOT_IP");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
            // sucess)
            LOG("IOTX_AWSS_SUC_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
            // support bind between user and device
            LOG("IOTX_AWSS_BIND_NOTIFY");
            // operate led to indicate user
            user_example_ctx_t *user_example_ctx = user_example_get_ctx();
            user_example_ctx->bind_notified = 1;
            break;
        case IOTX_AWSS_ENABLE_TIMEOUT: // AWSS enable timeout
            // user needs to enable awss again to support get ssid & passwd of router
            LOG("IOTX_AWSS_ENALBE_TIMEOUT");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD: // Device try to connect cloud
            LOG("IOTX_CONN_CLOUD");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
            // net_sockets.h for error code
            LOG("IOTX_CONN_CLOUD_FAIL");
            uart_push_data("CLOUD DISCONNECTED");
            is_connect = 0;
            #ifdef EN_COMBO_NET
            combo_set_cloud_state(0);
            #endif
            set_net_state(CONNECT_CLOUD_FAILED);
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            LOG("IOTX_CONN_CLOUD_SUC");
            uart_push_data("CLOUD CONNECTED");
            is_connect = 1;
            #ifdef EN_COMBO_NET
            combo_set_cloud_state(1);
            #endif
            set_net_state(CONNECT_CLOUD_SUCCESS);
            // operate led to indicate user
            break;
        case IOTX_RESET: // Linkkit reset success (just got reset response from
            // cloud without any other operation)
            LOG("IOTX_RESET");
            break;
        case IOTX_CONN_REPORT_TOKEN_SUC:
            #ifdef EN_COMBO_NET
            combo_token_report_notify();
            #endif
            LOG("---- report token success ----");
            break;
        default:
            break;
    }
}

#ifdef AWSS_BATCH_DEVAP_ENABLE
#define DEV_AP_ZCONFIG_TIMEOUT_MS  120000 // (ms)
extern void awss_set_config_press(uint8_t press);
extern uint8_t awss_get_config_press(void);
extern void zconfig_80211_frame_filter_set(uint8_t filter, uint8_t fix_channel);
void do_awss_dev_ap();

static aos_timer_t dev_ap_zconfig_timeout_timer;
static uint8_t g_dev_ap_zconfig_timer = 0; // this timer create once and can restart
static uint8_t g_dev_ap_zconfig_run = 0;

static void timer_func_devap_zconfig_timeout(void *arg1, void *arg2)
{
    LOG("%s run\n", __func__);

    if (awss_get_config_press()) {
        // still in zero wifi provision stage, should stop and switch to dev ap
        do_awss_dev_ap();
    } else {
        // zero wifi provision finished
    }

    awss_set_config_press(0);
    zconfig_80211_frame_filter_set(0xFF, 0xFF);
    g_dev_ap_zconfig_run = 0;
    aos_timer_stop(&dev_ap_zconfig_timeout_timer);
}

static void awss_dev_ap_switch_to_zeroconfig(void *p)
{
    LOG("%s run\n", __func__);
    // Stop dev ap wifi provision
    awss_dev_ap_stop();
    // Start and enable zero wifi provision
    iotx_event_regist_cb(linkkit_event_monitor);
    awss_set_config_press(1);

    // Start timer to count duration time of zero provision timeout
    if (!g_dev_ap_zconfig_timer) {
        aos_timer_new(&dev_ap_zconfig_timeout_timer, timer_func_devap_zconfig_timeout, NULL, DEV_AP_ZCONFIG_TIMEOUT_MS, 0);
        g_dev_ap_zconfig_timer = 1;
    }
    aos_timer_start(&dev_ap_zconfig_timeout_timer);

    // This will hold thread, when awss is going
    netmgr_start(true);

    LOG("%s exit\n", __func__);
    aos_task_exit(0);
}

int awss_dev_ap_modeswitch_cb(uint8_t awss_new_mode, uint8_t new_mode_timeout, uint8_t fix_channel)
{
    if ((awss_new_mode == 0) && !g_dev_ap_zconfig_run) {
        g_dev_ap_zconfig_run = 1;
        // Only receive zero provision packets
        zconfig_80211_frame_filter_set(0x00, fix_channel);
        LOG("switch to awssmode %d, mode_timeout %d, chan %d\n", 0x00, new_mode_timeout, fix_channel);
        // switch to zero config
        aos_task_new("devap_to_zeroconfig", awss_dev_ap_switch_to_zeroconfig, NULL, 2048);
    }
}
#endif

static void awss_close_dev_ap(void *p)
{
    awss_dev_ap_stop();
    aos_task_exit(0);
}

void awss_open_dev_ap(void *p)
{
    iotx_event_regist_cb(linkkit_event_monitor);
    /*if (netmgr_start(false) != 0) */{
        //aos_msleep(2000);
#ifdef AWSS_BATCH_DEVAP_ENABLE
        awss_dev_ap_reg_modeswit_cb(awss_dev_ap_modeswitch_cb);
#endif
        awss_dev_ap_start();
    }
    aos_task_exit(0);
}

void stop_netmgr(void *p)
{
    awss_stop();
    aos_task_exit(0);
}

void start_netmgr(void *p)
{
    iotx_event_regist_cb(linkkit_event_monitor);
    netmgr_start(true);
    aos_task_exit(0);
}

void do_awss_active(void)
{
    LOG("do_awss_active %d\n", awss_running);
    awss_running = 1;
#ifdef WIFI_PROVISION_ENABLED
    extern int awss_config_press();
    awss_config_press();
#endif
}


#ifdef EN_COMBO_NET
void ble_awss_open(void *p)
{
    iotx_event_regist_cb(linkkit_event_monitor);
    combo_set_awss_state(1);
    aos_task_exit(0);
}

static void ble_awss_close(void *p)
{
    combo_set_awss_state(0);
    aos_task_exit(0);
}

void do_ble_awss()
{
    aos_task_new("netmgr_stop", stop_netmgr, NULL, 4096);
    //aos_msleep(2000);
#if (defined (TG7100CEVB))
    aos_task_new("ble_awss_open", ble_awss_open, NULL, 5120);
#else
    aos_task_new("ble_awss_open", ble_awss_open, NULL, 4096);
#endif
}
#endif



void do_awss_dev_ap()
{
    aos_task_new("netmgr_stop", stop_netmgr, NULL, 4096);
    aos_task_new("dap_open", awss_open_dev_ap, NULL, 4096);
}

void do_awss()
{
#if (defined (TG7100CEVB))
    aos_task_new("dap_close", awss_close_dev_ap, NULL, 2048 + 2 * 1024);
#else
    aos_task_new("dap_close", awss_close_dev_ap, NULL, 2048);
#endif
    aos_task_new("netmgr_start", start_netmgr, NULL, 5120);
}

void linkkit_reset(void *p)
{
    aos_msleep(2000);
#ifdef AOS_TIMER_SERVICE
    timer_service_clear();
#endif
    iotx_sdk_reset_local();
    netmgr_clear_ap_config();
#ifdef EN_COMBO_NET
    breeze_clear_bind_info();
#endif
    HAL_Reboot();
    aos_task_exit(0);
}

extern int iotx_sdk_reset(iotx_vendor_dev_reset_type_t *reset_type);
iotx_vendor_dev_reset_type_t reset_type = IOTX_VENDOR_DEV_RESET_TYPE_UNBIND_ONLY;
void do_awss_reset(void)
{
#ifdef WIFI_PROVISION_ENABLED
    aos_task_new("reset", (void (*)(void *))iotx_sdk_reset, &reset_type, 6144);  // stack taken by iTLS is more than taken by TLS.
#endif
    aos_task_new_ext(&task_linkkit_reset, "reset task", linkkit_reset, NULL, 1024, 0);
    //aos_post_delayed_action(3000, linkkit_reset, NULL);
}

void reboot_device(void *p)
{
    aos_msleep(500);
    HAL_Reboot();
    aos_task_exit(0);
}

void do_awss_reboot(void)
{
    int ret;
    unsigned char awss_flag = 1;
    int len = sizeof(awss_flag);

    ret = aos_kv_set("awss_flag", &awss_flag, len, 1);
    if (ret != 0)
        LOG("KV Setting failed");

    aos_task_new_ext(&task_reboot_device, "reboot task", reboot_device, NULL, 1024, 0);
    //aos_post_delayed_action(500, reboot_device, NULL);
}

void linkkit_key_process(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_KEY) {
        return;
    }
    LOG("awss config press %d\n", eventinfo->value);

    if (eventinfo->code == CODE_BOOT) {
        if (eventinfo->value == VALUE_KEY_CLICK) {

            do_awss_active();
        } else if (eventinfo->value == VALUE_KEY_LTCLICK) {
            do_awss_reset();
        }
    }
}

#ifdef MANUFACT_AP_FIND_ENABLE
void manufact_ap_find_process(int result)
{
    // Informed manufact ap found or not.
    // If manufact ap found, lower layer will auto connect the manufact ap
    // IF manufact ap not found, lower layer will enter normal awss state
    if (result == 0) {
        LOG("%s ap found.\n", __func__);
    } else {
        LOG("%s ap not found.\n", __func__);
    }
}
#endif

#ifdef CONFIG_AOS_CLI
static void handle_reset_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call((aos_call_t)do_awss_reset, NULL);
}

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call((aos_call_t)do_awss_active, NULL);
}

static void handle_dev_ap_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call((aos_call_t)do_awss_dev_ap, NULL);
}

#ifdef EN_COMBO_NET
static void handle_ble_awss_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call((aos_call_t)do_ble_awss, NULL);
}
#endif

static void handle_linkkey_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc == 1) {
        int len = 0;
        char product_key[PRODUCT_KEY_LEN + 1] = { 0 };
        char product_secret[PRODUCT_SECRET_LEN + 1] = { 0 };
        char device_name[DEVICE_NAME_LEN + 1] = { 0 };
        char device_secret[DEVICE_SECRET_LEN + 1] = { 0 };
        char pidStr[9] = { 0 };

        len = PRODUCT_KEY_LEN + 1;
        aos_kv_get(KV_KEY_PK, product_key, &len);

        len = PRODUCT_SECRET_LEN + 1;
        aos_kv_get(KV_KEY_PS, product_secret, &len);

        len = DEVICE_NAME_LEN + 1;
        aos_kv_get(KV_KEY_DN, device_name, &len);

        len = DEVICE_SECRET_LEN + 1;
        aos_kv_get(KV_KEY_DS, device_secret, &len);

        aos_cli_printf("Product Key=%s.\r\n", product_key);
        aos_cli_printf("Device Name=%s.\r\n", device_name);
        aos_cli_printf("Device Secret=%s.\r\n", device_secret);
        aos_cli_printf("Product Secret=%s.\r\n", product_secret);
        len = sizeof(pidStr);
        if (aos_kv_get(KV_KEY_PD, pidStr, &len) == 0) {
            aos_cli_printf("Product Id=%d.\r\n", atoi(pidStr));
        }
    } else if (argc == 5 || argc == 6) {
        aos_kv_set(KV_KEY_PK, argv[1], strlen(argv[1]) + 1, 1);
        aos_kv_set(KV_KEY_DN, argv[2], strlen(argv[2]) + 1, 1);
        aos_kv_set(KV_KEY_DS, argv[3], strlen(argv[3]) + 1, 1);
        aos_kv_set(KV_KEY_PS, argv[4], strlen(argv[4]) + 1, 1);
        if (argc == 6)
            aos_kv_set(KV_KEY_PD, argv[5], strlen(argv[5]) + 1, 1);
        aos_cli_printf("Done");
    } else {
        aos_cli_printf("Error: %d\r\n", __LINE__);
        return;
    }
}

static void handle_awss_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss, NULL);
}

static struct cli_command resetcmd = {
    .name = "reset",
    .help = "factory reset",
    .function = handle_reset_cmd
};

static struct cli_command awss_enable_cmd = {
    .name = "active_awss",
    .help = "active_awss [start]",
    .function = handle_active_cmd
};

static struct cli_command awss_dev_ap_cmd = {
    .name = "dev_ap",
    .help = "awss_dev_ap [start]",
    .function = handle_dev_ap_cmd
};

static struct cli_command awss_cmd = {
    .name = "awss",
    .help = "awss [start]",
    .function = handle_awss_cmd
};

#ifdef EN_COMBO_NET
static struct cli_command awss_ble_cmd = {
    .name = "ble_awss",
    .help = "ble_awss [start]",
    .function = handle_ble_awss_cmd
};
#endif

static struct cli_command linkkeycmd = {
    .name = "linkkey",
    .help = "set/get linkkit keys. linkkey [<Product Key> <Device Name> <Device Secret> <Product Secret>]",
    .function = handle_linkkey_cmd
};

#endif

#ifdef CONFIG_PRINT_HEAP
static void duration_work(void *p)
{
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);
}
#endif

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
static int ota_init(void);
static ota_service_t ctx = {0};
#endif
static int mqtt_connected_event_handler(void)
{
#if defined(OTA_ENABLED) && defined(BUILD_AOS)
    static bool ota_service_inited = false;

    if (ota_service_inited == true) {
        int ret = 0;

        LOG("MQTT reconnected, let's redo OTA upgrade");
        if ((ctx.h_tr) && (ctx.h_tr->upgrade)) {
            LOG("Redoing OTA upgrade");
            ret = ctx.h_tr->upgrade(&ctx);
            if (ret < 0) LOG("Failed to do OTA upgrade");
        }

        return ret;
    }

    LOG("MQTT Construct  OTA start to inform");
#ifdef DEV_OFFLINE_OTA_ENABLE
    ota_service_inform(&ctx);
#else
    ota_init();
#endif
    ota_service_inited = true;
#endif
    return 0;
}

static int ota_init(void)
{
#if defined(OTA_ENABLED) && defined(BUILD_AOS)
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};
    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    HAL_GetDeviceSecret(device_secret);
    memset(&ctx, 0, sizeof(ota_service_t));
    strncpy(ctx.pk, product_key, sizeof(ctx.pk)-1);
    strncpy(ctx.dn, device_name, sizeof(ctx.dn)-1);
    strncpy(ctx.ds, device_secret, sizeof(ctx.ds)-1);
    ctx.trans_protcol = 0;
    ctx.dl_protcol = 3;
    ota_service_init(&ctx);
#endif
    return 0;
}

#ifdef EN_COMBO_NET
void combo_open(void)
{
    combo_net_init();
}
#endif

// static void show_firmware_version(void)
// {
//     printf("\r\n################################################################\r\n");
//     printf("\r\n--------Firmware info--------");
//     printf("\r\nHost: %s", COMPILE_HOST);
//     printf("\r\nBranch: %s", GIT_BRANCH);
//     printf("\r\nHash: %s", GIT_HASH);
//     printf("\r\nDate: %s %s", __DATE__, __TIME__);
//     printf("\r\nKernel: %s", aos_get_kernel_version());
//     printf("\r\nLinkKit: %s", LINKKIT_VERSION);
//     printf("\r\nAPP: %s", aos_get_app_version());

//     printf("\r\nRegion env: %s\r\n\r\n", REGION_ENV_STRING);
//     printf("\r\n################################################################\r\n");
// }

static void show_firmware_version(void)
{
    uint8_t mac[6] = {0};
    char buf[100] = {0};
    bl_flash_efuse_read_mac(mac);
    uart_push_data("\r\n################################################\r\n");
    uart_push_data("\r\narch:CHIP_TG7100C,0x00000001");
    uart_push_data("\r\ncompany:Ai-Thinker|B&T");
    sprintf(buf,"\r\nwifi_mac:%02X%02X%02X%02X%02X%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    uart_push_data(buf);
    memset(buf,0,sizeof(buf));
    sprintf(buf,"\r\nsdk_version:%s",SDK_VERSION);
    uart_push_data(buf);
    memset(buf,0,sizeof(buf));
    sprintf(buf,"\r\nfirmware_version:%s",aos_get_app_version());
    uart_push_data(buf);
    memset(buf,0,sizeof(buf));
    sprintf(buf,"\r\ncompile_time:%s %s", __DATE__, __TIME__);
    uart_push_data(buf);
    uart_push_data("\r\n\r\nready");
    uart_push_data("\r\n\r\n################################################\r\n");
}


#ifdef MANUFACT_AP_FIND_ENABLE
void manufact_ap_find_process(int result)
{
    // Informed manufact ap found or not.
    // If manufact ap found, lower layer will auto connect the manufact ap
    // IF manufact ap not found, lower layer will enter normal awss state
    if (result == 0) {
        LOG("%s ap found.\n", __func__);
    } else {
        LOG("%s ap not found.\n", __func__);
    }
}
#endif



void media_to_kv(void)
{
    char product_key[PRODUCT_KEY_LEN + 1] = { 0 };
    char *p_product_key = NULL;
    char product_secret[PRODUCT_SECRET_LEN + 1] = { 0 };
    char *p_product_secret = NULL;
    char device_name[DEVICE_NAME_LEN + 1] = { 0 };
    char *p_device_name = NULL;
    char device_secret[DEVICE_SECRET_LEN + 1] = { 0 };
    char *p_device_secret = NULL;
    char pidStr[9] = { 0 };
    char *p_pidStr = NULL;
    int len;

    int res;

    /* check media valid, and update p */
    res = ali_factory_media_get(
                &p_product_key,
                &p_product_secret,
                &p_device_name,
                &p_device_secret,
                &p_pidStr);
    if (0 != res) {
        printf("ali_factory_media_get res = %d\r\n", res);
        return;
    }

    /* compare kv media */
    len = sizeof(product_key);
    aos_kv_get(KV_KEY_PK, product_key, &len);
    len = sizeof(product_secret);
    aos_kv_get(KV_KEY_PS, product_secret, &len);
    len = sizeof(device_name);
    aos_kv_get(KV_KEY_DN, device_name, &len);
    len = sizeof(device_secret);
    aos_kv_get(KV_KEY_DS, device_secret, &len);
    len = sizeof(pidStr);
    aos_kv_get(KV_KEY_PD, pidStr, &len);

    if (p_product_key) {
        if (0 != memcmp(product_key, p_product_key, strlen(p_product_key))) {
            printf("memcmp p_product_key different. set kv: %s\r\n", p_product_key);
            aos_kv_set(KV_KEY_PK, p_product_key, strlen(p_product_key), 1);
        }
    }
    if (p_product_secret) {
        if (0 != memcmp(product_secret, p_product_secret, strlen(p_product_secret))) {
            printf("memcmp p_product_secret different. set kv: %s\r\n", p_product_secret);
            aos_kv_set(KV_KEY_PS, p_product_secret, strlen(p_product_secret), 1);
        }
    }
    if (p_device_name) {
        if (0 != memcmp(device_name, p_device_name, strlen(p_device_name))) {
            printf("memcmp p_device_name different. set kv: %s\r\n", p_device_name);
            aos_kv_set(KV_KEY_DN, p_device_name, strlen(p_device_name), 1);
        }
    }
    if (p_device_secret) {
        if (0 != memcmp(device_secret, p_device_secret, strlen(p_device_secret))) {
            printf("memcmp p_device_secret different. set kv: %s\r\n", p_device_secret);
            aos_kv_set(KV_KEY_DS, p_device_secret, strlen(p_device_secret), 1);
        }
    }
    if (p_pidStr) {
        if (0 != memcmp(pidStr, p_pidStr, strlen(p_pidStr))) {
            printf("memcmp p_pidStr different. set kv: %s\r\n", p_pidStr);
            aos_kv_set(KV_KEY_PD, p_pidStr, strlen(p_pidStr), 1);
        }
    }
}

void print_heap(void *arg)
{
    extern k_mm_head *g_kmm_head;
    int free; 
    while(1){
        free = g_kmm_head->free_size;
        printf("============free heap size =%d==========", free);
        aos_msleep(1000);
    }
}


int application_start(int argc, char **argv)
{
    // media_to_kv();
#ifdef CONFIG_PRINT_HEAP
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);     //查询剩余内存空间
#endif

#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);//信号处理程序
#endif

#ifdef WITH_SAL
    sal_init(); 
#endif

#ifdef MDAL_MAL_ICA_TEST
    HAL_MDAL_MAL_Init();
#endif

#ifdef DEFAULT_LOG_LEVEL_DEBUG
    IOT_SetLogLevel(IOT_LOG_NONE);
#else
    IOT_SetLogLevel(IOT_LOG_NONE);   //日志等级
#endif
    // aos_task_new("print_heap", print_heap, NULL, 4096);

    show_firmware_version();    //固件信息
    /* Must call set_device_meta_info before netmgr_init */
    set_device_meta_info(); //设置三元组信息
    netmgr_init();
#ifdef DEV_OFFLINE_OTA_ENABLE
    ota_init();
#endif

#ifdef MANUFACT_AP_FIND_ENABLE
    //测试使用
    netmgr_manuap_info_set("Test03", "123456798", manufact_ap_find_process);
#endif
    vendor_product_init();
    init_msg_queue();

    aos_register_event_filter(EV_KEY, linkkit_key_process, NULL);
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);   //获取路由IP后执行这个回调，回调里启动main线程
    aos_register_event_filter(EV_YUNIO, cloud_service_event, NULL);
    IOT_RegisterCallback(ITE_MQTT_CONNECT_SUCC,mqtt_connected_event_handler);
   
    // #ifdef CONFIG_AOS_CLI
    // aos_cli_register_command(&linkkeycmd);
    // #endif

    #ifdef EN_COMBO_NET
    combo_open();
    #endif

    //开发板按键、LED测试
    aos_task_new("detect key pressed", key_detect_event_task, NULL, 2 * 1024);
    at_register_cmds();
    aos_task_new("indicate net state", indicate_net_state_task, NULL, 1024 + 2 * 1024);
    //属性上报任务
    aos_task_new_ext(&task_property_report, "property report", user_port_recv, NULL, 2048 + 1024, AOS_DEFAULT_APP_PRI);
  
    netmgr_ap_config_t ap_config;
    memset(&ap_config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&ap_config);
    if(strlen(ap_config.ssid) > 0){
        set_net_state(GOT_AP_SSID);
        printf("have ssid %s\n",ap_config.ssid);
        printf("passwd = %s\n",ap_config.pwd);
        aos_task_new("netmgr_start", start_netmgr, NULL, 5120);
    }

    aos_loop_run();

    return 0;
}
