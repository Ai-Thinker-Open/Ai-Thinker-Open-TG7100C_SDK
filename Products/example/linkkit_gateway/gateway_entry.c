/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
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
#include "aos/kv.h"

#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

#include <k_api.h>

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
#include "ota_service.h"
#endif

#include "gateway_main.h"
#include "gateway_cmds.h"
#include "gateway_entry.h"
#include "gateway_ut.h"

static char gateway_started = 0;
static int gateway_duration_interval = 0;

void set_iotx_info();
void do_awss_active();

extern k_mm_head *g_kmm_head;
extern uint32_t dumpsys_mm_info_func(uint32_t len);
extern uint32_t dumpsys_task_func(char *buf, uint32_t len, int detail);
static void dump_task_and_mem_info(void)
{
#if defined(CONFIG_AOS_CLI)
    printf("======================================");
    dumpsys_task_func(NULL, 0, 1);
    printf("======================================");
#endif
    printf("============free heap size =%d==========\r\n", g_kmm_head->free_size);
}

#ifdef GATEWAY_UT_STABILITY_TESTING
static int switch_cmd = 0;
int send_addall_subdev(void)
{
    gateway_msg_t msg;

    msg.msg_type = GATEWAY_MSG_TYPE_ADDALL;
    msg.devid = 1;
#ifdef GATEWAY_UT_TESTING    
    gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
#endif
}
int send_delall_subdev(void)
{
    gateway_msg_t msg;

    msg.msg_type = GATEWAY_MSG_TYPE_DELALL;
    msg.devid = 1;
#ifdef GATEWAY_UT_TESTING    
    gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
#endif
}
#endif

static void gateway_duration_work(void *param)
{
    dump_task_and_mem_info();

#ifdef GATEWAY_UT_STABILITY_TESTING
    if (switch_cmd == 0)
    {
        send_addall_subdev();
        switch_cmd = 1;
    }
    else
    {
        send_delall_subdev();
        switch_cmd = 0;
    }
#endif

    aos_post_delayed_action(gateway_duration_interval, gateway_duration_work, NULL);
}

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    netmgr_ap_config_t config;

    if (event->type != EV_WIFI || event->code != CODE_WIFI_ON_GOT_IP)
    {
        return;
    }

    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);

    gateway_info("wifi_service_event config.ssid:%s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0)
    {
        // clear_wifi_ssid();
        return;
    }

    if (!gateway_started)
    {
        aos_task_new("gateway_main", (void (*)(void *))gateway_main, NULL, GATEWAY_MAIN_THREAD_STACKSZIE);
        gateway_started = 1;
    }
}

static void cloud_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_YUNIO)
    {
        return;
    }

    gateway_info("cloud_service_event %d", event->code);

    if (event->code == CODE_YUNIO_ON_CONNECTED)
    {
        gateway_info("user sub and pub here");
        return;
    }

    if (event->code == CODE_YUNIO_ON_DISCONNECTED)
    {
    }
}

/*
 * Note:
 * the user_event_monitor must not block and should run to complete fast
 * if user wants to do complex operation with much time,
 * user should post one task to do this, not implement complex operation in
 * user_event_monitor
 */

void user_event_monitor(int event)
{
    switch (event)
    {
    case IOTX_AWSS_START: // AWSS start without enbale, just supports device discover
        // operate led to indicate user
        gateway_info("IOTX_AWSS_START");
        break;
    case IOTX_AWSS_ENABLE: // AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled.
        gateway_info("IOTX_AWSS_ENABLE");
        // operate led to indicate user
        break;
    case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
        gateway_info("IOTX_AWSS_LOCK_CHAN");
        // operate led to indicate user
        break;
    case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
        gateway_info("IOTX_AWSS_PASSWD_ERR");
        // operate led to indicate user
        break;
    case IOTX_AWSS_GOT_SSID_PASSWD:
        gateway_info("IOTX_AWSS_GOT_SSID_PASSWD");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
        // discover, router solution)
        gateway_info("IOTX_AWSS_CONNECT_ADHA");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
        gateway_info("IOTX_AWSS_CONNECT_ADHA_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
        gateway_info("IOTX_AWSS_CONNECT_AHA");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
        gateway_info("IOTX_AWSS_CONNECT_AHA_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
        // (AP and router solution)
        gateway_info("IOTX_AWSS_SETUP_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
        gateway_info("IOTX_AWSS_CONNECT_ROUTER");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
        // router.
        gateway_info("IOTX_AWSS_CONNECT_ROUTER_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
        // ip address
        gateway_info("IOTX_AWSS_GOT_IP");
        // operate led to indicate user
        break;
    case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
        // sucess)
        gateway_info("IOTX_AWSS_SUC_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
        // support bind between user and device
        gateway_info("IOTX_AWSS_BIND_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_AWSS_ENABLE_TIMEOUT: // AWSS enable timeout
        // user needs to enable awss again to support get ssid & passwd of router
        gateway_info("IOTX_AWSS_ENALBE_TIMEOUT");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD: // Device try to connect cloud
        gateway_info("IOTX_CONN_CLOUD");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
        // net_sockets.h for error code
        gateway_info("IOTX_CONN_CLOUD_FAIL");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
        gateway_info("IOTX_CONN_CLOUD_SUC");
        // operate led to indicate user
        break;
    case IOTX_RESET: // Linkkit reset success (just got reset response from
        // cloud without any other operation)
        gateway_info("IOTX_RESET");
        break;
    case IOTX_CONN_REPORT_TOKEN_SUC:
        gateway_info("---- report token success ----");
        break;
    default:
        break;
    }
}

static void user_key_process(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_KEY)
    {
        return;
    }
    gateway_info("awss config press %d\n", eventinfo->value);

    if (eventinfo->code == CODE_BOOT)
    {
        if (eventinfo->value == VALUE_KEY_CLICK)
        {

            gateway_do_awss_active();
        }
        else if (eventinfo->value == VALUE_KEY_LTCLICK)
        {
            gateway_awss_reset();
        }
    }
}

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
static ota_service_t ctx = {0};

static int ota_init(void)
{
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};
    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    HAL_GetDeviceSecret(device_secret);
    memset(&ctx, 0, sizeof(ota_service_t));
    strncpy(ctx.pk, product_key, sizeof(ctx.pk) - 1);
    strncpy(ctx.dn, device_name, sizeof(ctx.dn) - 1);
    strncpy(ctx.ds, device_secret, sizeof(ctx.ds) - 1);
    ctx.trans_protcol = 0;
    ctx.dl_protcol = 3;
    ota_service_init(&ctx);

    return 0;
}
#endif

static int mqtt_connected_event_handler(void)
{
#if defined(OTA_ENABLED) && defined(BUILD_AOS)
    static bool ota_service_inited = false;

    if (ota_service_inited == true)
    {
        int ret = 0;

        LOG("MQTT reconnected, let's redo OTA upgrade");
        if ((ctx.h_tr) && (ctx.h_tr->upgrade))
        {
            LOG("Redoing OTA upgrade");
            ret = ctx.h_tr->upgrade(&ctx);
            if (ret < 0)
                LOG("Failed to do OTA upgrade");
        }

        return ret;
    }

    LOG("MQTT Construct OTA start to inform");
#ifdef DEV_OFFLINE_OTA_ENABLE
    ota_service_inform(&ctx);
#else
    ota_init();
#endif
    ota_service_inited = true;
#endif
    return 0;
}

static void load_device_meta_info(void)
{
    int len = 0;
    char key_buf[MAX_KEY_LEN];
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char product_secret[PRODUCT_SECRET_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};

    len = PRODUCT_KEY_LEN + 1;
    memset(key_buf, 0, MAX_KEY_LEN);
    memset(product_key, 0, sizeof(product_key));
    HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PK, 0);
    HAL_Kv_Get(key_buf, product_key, &len);

    len = PRODUCT_SECRET_LEN + 1;
    memset(key_buf, 0, MAX_KEY_LEN);
    memset(product_secret, 0, sizeof(product_secret));
    HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PS, 0);
    HAL_Kv_Get(key_buf, product_secret, &len);

    len = DEVICE_NAME_LEN + 1;
    memset(key_buf, 0, MAX_KEY_LEN);
    memset(device_name, 0, sizeof(device_name));
    HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DN, 0);
    HAL_Kv_Get(key_buf, device_name, &len);

    len = DEVICE_SECRET_LEN + 1;
    memset(key_buf, 0, MAX_KEY_LEN);
    memset(device_secret, 0, sizeof(device_secret));
    HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DS, 0);
    HAL_Kv_Get(key_buf, device_secret, &len);

    if ((strlen(product_key) > 0) && (strlen(product_secret) > 0) && (strlen(device_name) > 0) && (strlen(device_secret) > 0))
    {
        HAL_SetProductKey(product_key);
        HAL_SetProductSecret(product_secret);
        HAL_SetDeviceName(device_name);
        HAL_SetDeviceSecret(device_secret);
        printf("pk[%s]\r\n", product_key);
        printf("dn[%s]\r\n", device_name);
    }
    else
    {
        HAL_SetProductKey(PRODUCT_KEY);
        HAL_SetProductSecret(PRODUCT_SECRET);
        HAL_SetDeviceName(DEVICE_NAME);
        HAL_SetDeviceSecret(DEVICE_SECRET);
        printf("pk[%s]\r\n", PRODUCT_KEY);
        printf("dn[%s]\r\n", DEVICE_NAME);
    }
}

int application_start(int argc, char **argv)
{
    int len = 0;
    char interval[9] = {0};

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
#ifdef CSP_LINUXHOST
    IOT_SetLogLevel(IOT_LOG_DEBUG);
#else
    IOT_SetLogLevel(IOT_LOG_INFO);
#endif
#endif

    load_device_meta_info();

#ifdef DEV_OFFLINE_OTA_ENABLE
    ota_init();
#endif

    netmgr_init();
    dev_diagnosis_module_init();

    aos_register_event_filter(EV_KEY, user_key_process, NULL);
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_register_event_filter(EV_YUNIO, cloud_service_event, NULL);

    IOT_RegisterCallback(ITE_MQTT_CONNECT_SUCC, mqtt_connected_event_handler);

#ifdef CONFIG_AOS_CLI
    gateway_register_cmds();
#endif

#if defined(AWSS_SUPPORT_DEV_AP) && defined(RDA5981x)
    aos_task_new("dap_open", gateway_open_dev_ap, NULL, 4096);
#else
    aos_task_new("netmgr_start", gateway_start_netmgr, NULL, 4096);
#endif

    //This code just for debug
    len = sizeof(interval);
    if (0 == HAL_Kv_Get(GATEWAY_DUMP_INTERVAL_KV_KEY, interval, &len))
    {
        gateway_duration_interval = atoi(interval);

        if (gateway_duration_interval > 0)
        {
            aos_post_delayed_action(gateway_duration_interval, gateway_duration_work, NULL);
        }
    }

    aos_loop_run();

    return 0;
}
