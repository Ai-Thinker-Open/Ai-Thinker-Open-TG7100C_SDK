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
#include "vendor.h"
#include "device_state_manger.h"
#include "smart_outlet.h"
#include "msg_process_center.h"
#include "property_report.h"

#if defined(AOS_TIMER_SERVICE)||defined(AIOT_DEVICE_TIMER_ENABLE)
#include "iot_export_timer.h"
#endif
#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

#include <k_api.h>

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
#include "ota_service.h"
#endif

#ifdef EN_COMBO_NET
#include "breeze_export.h"
#include "combo_net.h"
#endif

static aos_task_t task_key_detect;
static aos_task_t task_msg_process;
static aos_task_t task_property_report;
static aos_task_t task_linkkit_reset;
static aos_task_t task_reboot_device;

static char linkkit_started = 0;
static char awss_dev_ap_started = 0;

extern int init_awss_flag(void);

void do_awss_active(void);

#ifdef CONFIG_PRINT_HEAP
void print_heap()
{
    extern k_mm_head *g_kmm_head;
    int               free = g_kmm_head->free_size;
    LOG("============free heap size =%d==========", free);
}
#endif

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    LOG("wifi_service_event(), event->code=%d", event->code);
    if (event->code == CODE_WIFI_ON_CONNECTED) {
        LOG("CODE_WIFI_ON_CONNECTED");
#ifdef EN_COMBO_NET
        aiot_ais_report_awss_status(AIS_AWSS_STATUS_PROGRESS_REPORT, AIS_AWSS_PROGRESS_CONNECT_AP_SUCCESS);
#endif
    } else if (event->code == CODE_WIFI_ON_DISCONNECT) {
        unsigned long reason_code = event->value;
        LOG("CODE_WIFI_ON_DISCONNECT, reason_code = %d", reason_code);
#ifdef EN_COMBO_NET
        combo_set_ap_state(COMBO_AP_DISCONNECTED);
        aiot_ais_report_awss_status(AIS_AWSS_STATUS_PROGRESS_REPORT, AIS_AWSS_PROGRESS_CONNECT_AP_FAILED | reason_code);
#endif
    } else if (event->code == CODE_WIFI_ON_CONNECT_FAILED) {
        LOG("CODE_WIFI_ON_CONNECT_FAILED");
#ifdef EN_COMBO_NET
        aiot_ais_report_awss_status(AIS_AWSS_STATUS_PROGRESS_REPORT, AIS_AWSS_PROGRESS_CONNECT_AP_FAILED);
#endif
    } else if (event->code == CODE_WIFI_ON_GOT_IP) {
        LOG("CODE_WIFI_ON_GOT_IP");
#ifdef EN_COMBO_NET
        combo_set_ap_state(COMBO_AP_CONNECTED);
        aiot_ais_report_awss_status(AIS_AWSS_STATUS_PROGRESS_REPORT, AIS_AWSS_PROGRESS_GET_IP_SUCCESS);
#endif
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

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
#ifdef EN_COMBO_NET
            combo_set_cloud_state(0);
#endif
            set_net_state(CONNECT_CLOUD_FAILED);
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            LOG("IOTX_CONN_CLOUD_SUC");
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
#ifdef AWSS_SUPPORT_DEV_AP
    awss_dev_ap_stop();
#endif
    awss_dev_ap_started = 0;
    aos_task_exit(0);
}

void awss_open_dev_ap(void *p)
{
    iotx_event_regist_cb(linkkit_event_monitor);
    /*if (netmgr_start(false) != 0) */{
        awss_dev_ap_started = 1;
        //aos_msleep(2000);
#ifdef AWSS_BATCH_DEVAP_ENABLE
        awss_dev_ap_reg_modeswit_cb(awss_dev_ap_modeswitch_cb);
#endif
#ifdef AWSS_SUPPORT_DEV_AP
        awss_dev_ap_start();
#endif
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
    /* wait for dev_ap mode stop done */
    do {
        aos_msleep(100);
    } while (awss_dev_ap_started);
    iotx_event_regist_cb(linkkit_event_monitor);
    netmgr_start(true);
    aos_task_exit(0);
}

void do_awss_active(void)
{
    LOG("do_awss_active");
#ifdef WIFI_PROVISION_ENABLED
    extern int awss_config_press();
    awss_config_press();
#endif
}

#ifdef EN_COMBO_NET

void combo_open(void)
{
    combo_net_init();
}

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
    aos_task_new("ble_awss_open", ble_awss_open, NULL, 2048);
}
#endif

void do_awss_dev_ap()
{
    // Enter dev_ap awss mode
    aos_task_new("netmgr_stop", stop_netmgr, NULL, 4096);
    aos_task_new("dap_open", awss_open_dev_ap, NULL, 4096);
}

void do_awss()
{
    // Enter smart_config awss mode
    aos_task_new("dap_close", awss_close_dev_ap, NULL, 4096);
    aos_task_new("netmgr_start", start_netmgr, NULL, 5120);
}

void linkkit_reset(void *p)
{
    aos_msleep(2000);
#ifdef AOS_TIMER_SERVICE
    timer_service_clear();
#endif
#ifdef AIOT_DEVICE_TIMER_ENABLE
    aiot_device_timer_clear();
#endif
    aos_kv_del(KV_KEY_SWITCH_STATE);
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
        aos_kv_get("linkkit_product_key", product_key, &len);

        len = PRODUCT_SECRET_LEN + 1;
        aos_kv_get("linkkit_product_secret", product_secret, &len);

        len = DEVICE_NAME_LEN + 1;
        aos_kv_get("linkkit_device_name", device_name, &len);

        len = DEVICE_SECRET_LEN + 1;
        aos_kv_get("linkkit_device_secret", device_secret, &len);

        aos_cli_printf("Product Key=%s.\r\n", product_key);
        aos_cli_printf("Device Name=%s.\r\n", device_name);
        aos_cli_printf("Device Secret=%s.\r\n", device_secret);
        aos_cli_printf("Product Secret=%s.\r\n", product_secret);
        len = sizeof(pidStr);
        if (aos_kv_get("linkkit_product_id", pidStr, &len) == 0) {
            aos_cli_printf("Product Id=%d.\r\n", atoi(pidStr));
        }
    } else if (argc == 5 || argc == 6) {
        aos_kv_set("linkkit_product_key", argv[1], strlen(argv[1]) + 1, 1);
        aos_kv_set("linkkit_device_name", argv[2], strlen(argv[2]) + 1, 1);
        aos_kv_set("linkkit_device_secret", argv[3], strlen(argv[3]) + 1, 1);
        aos_kv_set("linkkit_product_secret", argv[4], strlen(argv[4]) + 1, 1);
        if (argc == 6)
            aos_kv_set("linkkit_product_id", argv[5], strlen(argv[5]) + 1, 1);
        aos_cli_printf("Done");
    } else {
        aos_cli_printf("Error: %d\r\n", __LINE__);
        return;
    }
}

static void handle_awss_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call((aos_call_t)do_awss, NULL);
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

static void handle_ble_status_report_cmd(char *pwbuf, int blen, int argc, char **argv) {

    if (argc == 3) {
        aiot_ais_report_awss_status(atoi(argv[1]), atoi(argv[2]));
    }
}

static struct cli_command ble_status_report_cmd = {
    .name = "ble_report",
    .help = "ble_report [status] [code]",
    .function = handle_ble_status_report_cmd
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
static bool ota_service_inited = false;
#ifdef CERTIFICATION_TEST_MODE
void *ct_entry_get_uota_ctx(void)
{
    if (ota_service_inited == true)
    {
        return (void *)&ctx;
    }
    else
    {
        return NULL;
    }
}
#endif
#endif
static int mqtt_connected_event_handler(void)
{
#if defined(OTA_ENABLED) && defined(BUILD_AOS)
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

#ifdef OTA_MULTI_MODULE_DEBUG
    extern ota_hal_module_t ota_hal_module1;
    extern ota_hal_module_t ota_hal_module2;
    iotx_ota_module_info_t module;
    char module_name_key[MODULE_NAME_LEN + 1] = {0};
    char module_version_key[MODULE_VERSION_LEN + 1] = {0};
    char module_name_value[MODULE_NAME_LEN + 1] = {0};
    char module_version_value[MODULE_VERSION_LEN + 1] = {0};
    char buffer_len = 0;
    int ret = 0;

    for(int i = 1; i <= 2; i++){
        memset(module_name_key, 0, MODULE_NAME_LEN);
        memset(module_version_key, 0, MODULE_VERSION_LEN);
        memset(module_name_value, 0, MODULE_NAME_LEN);
        memset(module_version_value, 0, MODULE_VERSION_LEN);
        HAL_Snprintf(module_name_key, MODULE_NAME_LEN, "ota_m_name_%d", i);
        HAL_Snprintf(module_version_key, MODULE_VERSION_LEN, "ota_m_version_%d", i);
        HAL_Printf("module_name_key is %s\n",module_name_key);
        HAL_Printf("module_version_key is %s\n",module_version_key);
        buffer_len = MODULE_NAME_LEN;
        ret = HAL_Kv_Get(module_name_key,module_name_value, &buffer_len);
        buffer_len = MODULE_VERSION_LEN;
        ret |= HAL_Kv_Get(module_version_key,module_version_value, &buffer_len);
        memcpy(module.module_name, module_name_value, MODULE_NAME_LEN);
        memcpy(module.module_version, module_version_value, MODULE_VERSION_LEN);
        memcpy(module.product_key, ctx.pk, sizeof(ctx.pk)-1);
        memcpy(module.device_name, ctx.dn, sizeof(ctx.dn)-1);
        if(!ret){
            if(i == 1){
                module.hal = &ota_hal_module1;
            }else{
                module.hal = &ota_hal_module2;
            }
            ota_service_set_module_info(&ctx, &module);
        }
        HAL_Printf("module_name_value is %s\n",module_name_value);
        HAL_Printf("module_version_value is %s\n",module_version_value);
    }

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

static void show_firmware_version(void)
{
    printf("\r\n--------Firmware info--------");
    printf("\r\napp: %s,  board: %s", APP_NAME, PLATFORM);
    printf("\r\nHost: %s", COMPILE_HOST);
    printf("\r\nBranch: %s", GIT_BRANCH);
    printf("\r\nHash: %s", GIT_HASH);
    printf("\r\nDate: %s %s", __DATE__, __TIME__);
    printf("\r\nKernel: %s", aos_get_kernel_version());
    printf("\r\nLinkKit: %s", LINKKIT_VERSION);
    printf("\r\nAPP: %s", aos_get_app_version());

    printf("\r\nRegion env: %s\r\n\r\n", REGION_ENV_STRING);
}

#if 0//(defined (TG7100CEVB))
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
    aos_kv_get("linkkit_product_key", product_key, &len);
    len = sizeof(product_secret);
    aos_kv_get("linkkit_product_secret", product_secret, &len);
    len = sizeof(device_name);
    aos_kv_get("linkkit_device_name", device_name, &len);
    len = sizeof(device_secret);
    aos_kv_get("linkkit_device_secret", device_secret, &len);
    len = sizeof(pidStr);
    aos_kv_get("linkkit_product_id", pidStr, &len);

    if (p_product_key) {
        if (0 != memcmp(product_key, p_product_key, strlen(p_product_key))) {
            printf("memcmp p_product_key different. set kv: %s\r\n", p_product_key);
            aos_kv_set("linkkit_product_key", p_product_key, strlen(p_product_key), 1);
        }
    }
    if (p_product_secret) {
        if (0 != memcmp(product_secret, p_product_secret, strlen(p_product_secret))) {
            printf("memcmp p_product_secret different. set kv: %s\r\n", p_product_secret);
            aos_kv_set("linkkit_product_secret", p_product_secret, strlen(p_product_secret), 1);
        }
    }
    if (p_device_name) {
        if (0 != memcmp(device_name, p_device_name, strlen(p_device_name))) {
            printf("memcmp p_device_name different. set kv: %s\r\n", p_device_name);
            aos_kv_set("linkkit_device_name", p_device_name, strlen(p_device_name), 1);
        }
    }
    if (p_device_secret) {
        if (0 != memcmp(device_secret, p_device_secret, strlen(p_device_secret))) {
            printf("memcmp p_device_secret different. set kv: %s\r\n", p_device_secret);
            aos_kv_set("linkkit_device_secret", p_device_secret, strlen(p_device_secret), 1);
        }
    }
    if (p_pidStr) {
        if (0 != memcmp(pidStr, p_pidStr, strlen(p_pidStr))) {
            printf("memcmp p_pidStr different. set kv: %s\r\n", p_pidStr);
            aos_kv_set("linkkit_product_id", p_pidStr, strlen(p_pidStr), 1);
        }
    }
}
#endif

int application_start(int argc, char **argv)
{
#if (defined (TG7100CEVB))
    //media_to_kv();
#endif

#ifdef CONFIG_PRINT_HEAP
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);
#endif

#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif

#ifdef WITH_SAL
    sal_init();
#endif

#ifdef MDAL_MAL_ICA_TEST
    HAL_MDAL_MAL_Init();
#endif

#ifdef DEFAULT_LOG_LEVEL_DEBUG
    IOT_SetLogLevel(IOT_LOG_DEBUG);
#else
    IOT_SetLogLevel(IOT_LOG_WARNING);
#endif

    show_firmware_version();
    /* Must call set_device_meta_info before netmgr_init */
    set_device_meta_info();
    netmgr_init();
    vendor_product_init();
    dev_diagnosis_module_init();
#ifdef DEV_OFFLINE_OTA_ENABLE
    ota_init();
#endif

#ifdef MANUFACT_AP_FIND_ENABLE
    netmgr_manuap_info_set("TEST_AP", "TEST_PASSWORD", manufact_ap_find_process);
#endif

    aos_register_event_filter(EV_KEY, linkkit_key_process, NULL);
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_register_event_filter(EV_YUNIO, cloud_service_event, NULL);
    IOT_RegisterCallback(ITE_MQTT_CONNECT_SUCC,mqtt_connected_event_handler);

#ifdef CONFIG_AOS_CLI
    aos_cli_register_command(&resetcmd);
    aos_cli_register_command(&awss_enable_cmd);
    aos_cli_register_command(&awss_dev_ap_cmd);
    aos_cli_register_command(&awss_cmd);
#ifdef EN_COMBO_NET
    aos_cli_register_command(&awss_ble_cmd);
    aos_cli_register_command(&ble_status_report_cmd);
#endif
    aos_cli_register_command(&linkkeycmd);
#endif

#ifdef EN_COMBO_NET
    combo_open();
#endif

    init_awss_flag();
#if (defined (TG7100CEVB))
    aos_task_new_ext(&task_key_detect, "detect key pressed", key_detect_event_task, NULL, 1024 + 1024, AOS_DEFAULT_APP_PRI);
#else
    aos_task_new_ext(&task_key_detect, "detect key pressed", key_detect_event_task, NULL, 1024, AOS_DEFAULT_APP_PRI);
#endif

    init_msg_queue();
    aos_task_new_ext(&task_msg_process, "cmd msg process", msg_process_task, NULL, 2048, AOS_DEFAULT_APP_PRI - 1);
#ifdef REPORT_MULTHREAD
#if (defined (TG7100CEVB))
    aos_task_new_ext(&task_property_report, "property report", process_property_report_task, NULL, 2048 + 1024, AOS_DEFAULT_APP_PRI);
#else
    aos_task_new_ext(&task_property_report, "property report", process_property_report_task, NULL, 2048, AOS_DEFAULT_APP_PRI);
#endif
#endif
    check_factory_mode();
    aos_loop_run();

    return 0;
}
