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

#include "living_platform_rawdata_main.h"
#include "app_entry.h"
#include "living_platform_rawdata_cmds.h"
#include "living_platform_rawdata_ut.h"

static char living_platform_rawdata_started = 0;
static int living_platform_rawdata_duration_interval = 0;

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

static void living_platform_rawdata_duration_work(void *param)
{
    dump_task_and_mem_info();

    aos_post_delayed_action(living_platform_rawdata_duration_interval, living_platform_rawdata_duration_work, NULL);
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

    living_platform_rawdata_info("wifi_service_event config.ssid:%s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0)
    {
        // clear_wifi_ssid();
        return;
    }

    if (!living_platform_rawdata_started)
    {
        aos_task_new("linkkit", (void (*)(void *))living_platform_rawdata_main, NULL, LIVING_PLATFORM_RAWDATA_MAIN_THREAD_STACKSZIE);
        living_platform_rawdata_started = 1;
    }
}

static void cloud_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_YUNIO)
    {
        return;
    }

    living_platform_rawdata_info("cloud_service_event %d", event->code);

    if (event->code == CODE_YUNIO_ON_CONNECTED)
    {
        living_platform_rawdata_info("user sub and pub here");
        return;
    }

    if (event->code == CODE_YUNIO_ON_DISCONNECTED)
    {
    }
}

/*
 * Note:
 * the linkkit_event_monitor must not block and should run to complete fast
 * if user wants to do complex operation with much time,
 * user should post one task to do this, not implement complex operation in
 * linkkit_event_monitor
 */

void linkkit_event_monitor(int event)
{
    switch (event)
    {
    case IOTX_AWSS_START: // AWSS start without enbale, just supports device discover
        // operate led to indicate user
        living_platform_rawdata_info("IOTX_AWSS_START");
        break;
    case IOTX_AWSS_ENABLE: // AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled.
        living_platform_rawdata_info("IOTX_AWSS_ENABLE");
        // operate led to indicate user
        break;
    case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
        living_platform_rawdata_info("IOTX_AWSS_LOCK_CHAN");
        // operate led to indicate user
        break;
    case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
        living_platform_rawdata_info("IOTX_AWSS_PASSWD_ERR");
        // operate led to indicate user
        break;
    case IOTX_AWSS_GOT_SSID_PASSWD:
        living_platform_rawdata_info("IOTX_AWSS_GOT_SSID_PASSWD");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
        // discover, router solution)
        living_platform_rawdata_info("IOTX_AWSS_CONNECT_ADHA");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
        living_platform_rawdata_info("IOTX_AWSS_CONNECT_ADHA_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
        living_platform_rawdata_info("IOTX_AWSS_CONNECT_AHA");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
        living_platform_rawdata_info("IOTX_AWSS_CONNECT_AHA_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
        // (AP and router solution)
        living_platform_rawdata_info("IOTX_AWSS_SETUP_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
        living_platform_rawdata_info("IOTX_AWSS_CONNECT_ROUTER");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
        // router.
        living_platform_rawdata_info("IOTX_AWSS_CONNECT_ROUTER_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
        // ip address
        living_platform_rawdata_info("IOTX_AWSS_GOT_IP");
        // operate led to indicate user
        break;
    case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
        // sucess)
        living_platform_rawdata_info("IOTX_AWSS_SUC_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
        // support bind between user and device
        living_platform_rawdata_info("IOTX_AWSS_BIND_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_AWSS_ENABLE_TIMEOUT: // AWSS enable timeout
        // user needs to enable awss again to support get ssid & passwd of router
        living_platform_rawdata_info("IOTX_AWSS_ENALBE_TIMEOUT");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD: // Device try to connect cloud
        living_platform_rawdata_info("IOTX_CONN_CLOUD");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
        // net_sockets.h for error code
        living_platform_rawdata_info("IOTX_CONN_CLOUD_FAIL");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
        living_platform_rawdata_info("IOTX_CONN_CLOUD_SUC");
        // operate led to indicate user
        break;
    case IOTX_RESET: // Linkkit reset success (just got reset response from
        // cloud without any other operation)
        living_platform_rawdata_info("IOTX_RESET");
        break;
    case IOTX_CONN_REPORT_TOKEN_SUC:
        living_platform_rawdata_info("---- report token success ----");
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
 
    living_platform_rawdata_info("awss config press %d\n", eventinfo->value);

    if (eventinfo->code == CODE_BOOT)
    {
        if (eventinfo->value == VALUE_KEY_CLICK)
        {
            living_platform_rawdata_do_awss_active(NULL);
        }
        else if (eventinfo->value == VALUE_KEY_LTCLICK)
        {
            living_platform_rawdata_awss_reset(NULL);
        }
    }
}

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
static ota_service_t ctx = {0};
static bool ota_service_inited = false;

void *living_platform_rawdata_entry_get_uota_ctx(void)
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

    if ((strlen(product_key) > 0) && (strlen(product_secret) > 0) && (strlen(device_name) > 0))
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

#ifdef POWER_CYCLE_AWSS

#ifndef DEFAULT_CYCLE_TIMEOUT
#define DEFAULT_CYCLE_TIMEOUT 5000
#endif
static int network_start(void)
{
    netmgr_ap_config_t my_config = {0};
    netmgr_get_ap_config(&my_config);
    if (strlen(my_config.ssid))
    {
        aos_task_new("netmgr_start", living_platform_rawdata_start_netmgr, NULL, 5120);
    }
    else
    {
        printf("INFO: %s no valied ap confing\r\n", __func__);
    }
    return 0;
}

static int check_and_start_awss(void)
{
    int ret, len = 0;
    int awss_type = 0; /* 0 -- unknown, 1 -- dev_ap, 2 -- one key */
    char value_string[8];
    netmgr_ap_config_t my_config = {0};

    memset(value_string, 0, sizeof(value_string));
    len = sizeof(value_string);
    /* check awss type */
    ret = aos_kv_get("awss_type", value_string, &len);
    if (0 == strncmp("dev_ap", value_string, strlen("dev_ap")))
    {
        printf("INFO: %s awss type: device ap\r\n", __func__);
        living_platform_rawdata_do_awss_dev_ap();
        awss_type = 1;
    }
    else if (0 == strncmp("one_key", value_string, strlen("one_key")))
    {
        printf("INFO: %s awss type: one key\r\n", __func__);
        living_platform_rawdata_do_awss();
        living_platform_rawdata_do_awss_active();
        awss_type = 2;
    }
    else
    {
        printf("WARN: %s awss type: unknown\r\n", __func__);
        awss_type = 0;
    }
    /* Clesr awss type */
    ret = aos_kv_del("awss_type");
    return awss_type;
}

static void power_cycle_check(void *p)
{
    int ret = -1;
    int count;

    if (NULL == p)
    {
        printf("ERR: %s data pointer is NULL\r\n", __func__);
        return;
    }
    count = *(int *)p;
    printf("INFO: %s power cycle %d timeout clear\r\n", __func__, count);
    /* Reset cycle count */
    ret = aos_kv_set("power_cycle_count", "1", strlen("1") + 1, 1);
    if (0 != ret)
    {
        printf("ERR: %s aos_kv_set return: %d\r\n", __func__, ret);
        return;
    }
    if (count == 3)
    { /* Three times for device ap awss */
        ret = aos_kv_set("awss_type", "dev_ap", strlen("dev_ap") + 1, 1);
        printf("INFO: %s prepare for device soft ap awss\r\n", __func__);
        aos_schedule_call(living_platform_rawdata_awss_reset, NULL);
    }
    else if (count == 5)
    { /* Five times for one key awss */
        aos_kv_set("awss_type", "one_key", strlen("one_key") + 1, 1);
        printf("INFO: %s prepare for smart config awss\r\n", __func__);
        aos_schedule_call(living_platform_rawdata_awss_reset, NULL);
    }
}

static int power_cycle_awss(void)
{
    int ret, len = 0;
    int awss_type = 0;
    int cycle_count = 0;
    char value_string[8];

    memset(value_string, 0, sizeof(value_string));
    len = sizeof(value_string);

    /* read stored cycle value */
    ret = aos_kv_get("power_cycle_count", value_string, &len);
    if (0 != ret)
    {
        printf("INFO: %s init power cycle count\r\n", __func__);
        snprintf(value_string, sizeof(value_string), "%d", 1); /* Set initial value */
        ret = aos_kv_set("power_cycle_count", value_string, strlen(value_string), 1);
        if (0 != ret)
        {
            printf("ERR: %s, aos_kv_set return: %d\r\n", __func__, ret);
            return ret;
        }
    }
    else
    {
        cycle_count = atoi(value_string);
        printf("INFO: %s power cycle count: %d\r\n", __func__, cycle_count);
        memset(value_string, 0, sizeof(value_string));
        len = sizeof(value_string);
        if (cycle_count == 1)
        { /* AWSS always start after a timeout clear reboot */
            awss_type = check_and_start_awss();
            snprintf(value_string, sizeof(value_string), "%d", 1);
        }
        if (cycle_count > 4)
        { /* Cycle count max value is 5 */
            snprintf(value_string, sizeof(value_string), "%d", 1);
        }
        else
        { /* Increase cycle count */
            snprintf(value_string, sizeof(value_string), "%d", cycle_count + 1);
        }
        if (0 == awss_type)
        { /* Do not store cycle count when doing awss */
            ret = aos_kv_set("power_cycle_count", value_string, strlen(value_string) + 1, 1);
            aos_post_delayed_action(DEFAULT_CYCLE_TIMEOUT, power_cycle_check, (void *)(&cycle_count));
        }
        return ret;
    }
}
#endif

#ifdef MANUFACT_AP_FIND_ENABLE
void manufact_ap_find_process(int result)
{
    // Informed manufact ap found or not.
    // If manufact ap found, lower layer will auto connect the manufact ap
    // IF manufact ap not found, lower layer will enter normal awss state
    if (result == 0)
    {
        LOG("%s ap found.\n", __func__);
    }
    else
    {
        LOG("%s ap not found.\n", __func__);
    }
}

int check_manufacture_ap_info(void)
{
    uint8_t m_manu_flag = 0;
    int m_manu_flag_len = sizeof(m_manu_flag);
    aos_kv_get(MANUAP_CONTROL_KEY, &m_manu_flag, &m_manu_flag_len);
    if (m_manu_flag)
    {
        netmgr_manuap_info_set("TEST_AP", "TEST_PASSWORD", manufact_ap_find_process);
    }

    return 0;
}
#endif

static void show_firmware_version(void)
{
    printf("\n--------Firmware info--------");
    printf("\napp: %s,  board: %s", APP_NAME, PLATFORM);
#ifdef DEBUG
    printf("\nHost: %s", COMPILE_HOST);
#endif
    printf("\nBranch: %s", GIT_BRANCH);
    printf("\nHash: %s", GIT_HASH);
    printf("\nDate: %s %s", __DATE__, __TIME__);
    printf("\nKernel: %s", aos_get_kernel_version());
    printf("\nLinkKit: %s", LINKKIT_VERSION);
    printf("\nAPP ver: %s", aos_get_app_version());

    printf("\nRegion env: %s\n\n", REGION_ENV_STRING);
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

    show_firmware_version();
    load_device_meta_info();

#ifdef DEV_OFFLINE_OTA_ENABLE
    ota_init();
#endif

    netmgr_init();

#ifdef MANUFACT_AP_FIND_ENABLE
    check_manufacture_ap_info();
#endif

#ifdef DEV_ERRCODE_ENABLE
    dev_diagnosis_module_init();
#endif

    aos_register_event_filter(EV_KEY, user_key_process, NULL);
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_register_event_filter(EV_YUNIO, cloud_service_event, NULL);

    IOT_RegisterCallback(ITE_MQTT_CONNECT_SUCC, mqtt_connected_event_handler);

#ifdef CONFIG_AOS_CLI
    living_platform_rawdata_register_cmds(NULL);
#endif

#ifdef POWER_CYCLE_AWSS
    network_start();
    power_cycle_awss();
#else
    aos_task_new("netmgr_start", living_platform_rawdata_start_netmgr, NULL, 5120);
#endif

    //This code just for debug
    len = sizeof(interval);
    if (0 == HAL_Kv_Get(LIVING_PLATFORM_RAWDATA_DUMP_INTERVAL_KV_KEY, interval, &len))
    {
        living_platform_rawdata_duration_interval = atoi(interval);

        if (living_platform_rawdata_duration_interval > 0)
        {
            aos_post_delayed_action(living_platform_rawdata_duration_interval, living_platform_rawdata_duration_work, NULL);
        }
    }

    aos_loop_run();

    return 0;
}
