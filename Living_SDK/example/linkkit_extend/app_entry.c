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

#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

#include <k_api.h>

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
#include "ota_service.h"
#endif

static char linkkit_started = 0;
static char awss_running    = 0;

void set_iotx_info();
void do_awss_active();

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

    if (!linkkit_started) {
#ifdef CONFIG_PRINT_HEAP
        print_heap();
#endif
        aos_task_new("linkkit", (void (*)(void *))linkkit_main, NULL, 1024 * 10);
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
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            LOG("IOTX_CONN_CLOUD_SUC");
            // operate led to indicate user
            break;
        case IOTX_RESET: // Linkkit reset success (just got reset response from
                         // cloud without any other operation)
            LOG("IOTX_RESET");
            // operate led to indicate user
            break;
        default:
            break;
    }
}

static void awss_close_dev_ap(void *p)
{
    awss_dev_ap_stop();
    LOG("%s exit\n", __func__);
    aos_task_exit(0);
}

void awss_open_dev_ap(void *p)
{
    iotx_event_regist_cb(linkkit_event_monitor);
    LOG("%s\n", __func__);
    if (netmgr_start(false) != 0) {
        aos_msleep(2000);
        awss_dev_ap_start();
    }
    aos_task_exit(0);
}

static void stop_netmgr(void *p)
{
    awss_stop();
    LOG("%s\n", __func__);
    aos_task_exit(0);
}

static void start_netmgr(void *p)
{
    aos_msleep(2000);
    iotx_event_regist_cb(linkkit_event_monitor);
    netmgr_start(true);
    aos_task_exit(0);
}

void do_awss_active()
{
    LOG("do_awss_active %d\n", awss_running);
    awss_running = 1;
    #ifdef WIFI_PROVISION_ENABLED
    extern int awss_config_press();
    awss_config_press();
    #endif
}

void do_awss_dev_ap()
{
    aos_task_new("netmgr_stop", stop_netmgr, NULL, 4096);
    aos_task_new("dap_open", awss_open_dev_ap, NULL, 4096);
}

void do_awss()
{
    aos_task_new("dap_close", awss_close_dev_ap, NULL, 2048);
    aos_task_new("netmgr_start", start_netmgr, NULL, 4096);
}

static void linkkit_reset(void *p)
{
    iotx_sdk_reset_local();
    HAL_Reboot();
}

static void do_awss_reset()
{
#ifdef WIFI_PROVISION_ENABLED
    aos_task_new("reset", (void (*)(void *))iotx_sdk_reset, NULL, 6144);  // stack taken by iTLS is more than taken by TLS.
#endif
    aos_post_delayed_action(2000, linkkit_reset, NULL);
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

#ifdef CONFIG_AOS_CLI
static uint8_t is_devinfo_config = 0;

static void handle_reset_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    LOG("get reset cmd");
    aos_schedule_call(do_awss_reset, NULL);
}

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_active, NULL);
}

static void handle_dev_ap_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_dev_ap, NULL);
}

static void handle_awss_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss, NULL);
}

static void handle_setup_conn_info_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    uint16_t port = 0;

    if (argc < 5) {
        LOG("\nusage: setup_info [pk] [ps] [dn] [ds] [domain] [port]"    \
            "\n  [pk] [ps] [dn] [ds] are mandatory"   \
            "\ndomain:" \
            "\n  if MQTT_DIRECT defined, you shall input MQTT domain or ip address" \
            "\n    example: singapore_online: [pk].iot-as-mqtt.ap-southeast-1.aliyuncs.com" \
            "\n    example: shanghai_on_pre: 100.67.80.75, port = 80" \
            "\n  if MQTT_DIRECT not defined, you shall input HTTP domain or ip address" \
            "\n    example: singapore_online: iot-auth.ap-southeast-1.aliyuncs.com" \
            "\n    example: germany_online: iot-auth.eu-central-1.aliyuncs.com" \
            "\nport:"   \
            "\n  MQTT port number, if this option absence, default port will be used" \
            "\n");
        return;
    }

    if (strlen(argv[1]) > PRODUCT_KEY_LEN) {
        LOG("pk length error");
        return;
    }
    HAL_SetProductKey(argv[1]);

    if (strlen(argv[2]) > PRODUCT_SECRET_LEN) {
        LOG("ps length error");
        return;
    }
    HAL_SetProductSecret(argv[2]);

    if (strlen(argv[3]) > DEVICE_NAME_LEN) {
        LOG("dn length error");
        return;
    }
    HAL_SetDeviceName(argv[3]);

    if (strlen(argv[4]) > DEVICE_SECRET_LEN) {
        LOG("ds length error");
        return;
    }
    HAL_SetDeviceSecret(argv[4]);

    LOG("\npk = %s\nps = %s\ndn = %s\nds = %s", argv[1], argv[2], argv[3], argv[4]);

#ifdef MQTT_DIRECT
    if (argc >=7) {
        port = atoi(argv[6]);
        if (port == 0) {
            LOG("\nport number error");
            return;
        }
    }

    if (argc >= 6) {
        IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)argv[5]);
        LOG("mqtt domain = %s", argv[5]);
        if (port != 0 ) {
            IOT_Ioctl(IOTX_IOCTL_SET_MQTT_PORT, (void *)&port);
            LOG("port = %d", port);
        }
    }
#else
    if (argc >= 6) {
        IOT_Ioctl(IOTX_IOCTL_SET_HTTP_DOMAIN, (void *)argv[5]);
        LOG("https domain = %s", argv[5]);
    }
#endif
    LOG("MBEDTLS_SSL_MAX_CONTENT_LEN = %d", MBEDTLS_SSL_MAX_CONTENT_LEN);

    is_devinfo_config = 1;
}

static struct cli_command setup_info = {
    .name = "setup_info",
    .help = "[pk] [ps] [dn] [ds]",
    .function = handle_setup_conn_info_cmd,
};

static struct cli_command resetcmd = { .name     = "reset",
                                       .help     = "factory reset",
                                       .function = handle_reset_cmd };

static struct cli_command awss_enable_cmd = { .name     = "active_awss",
                                              .help     = "active_awss [start]",
                                              .function = handle_active_cmd };
static struct cli_command awss_dev_ap_cmd = { .name     = "dev_ap",
                                              .help     = "awss_dev_ap [start]",
                                              .function = handle_dev_ap_cmd };
static struct cli_command awss_cmd = { .name     = "awss",
                                       .help     = "awss [start]",
                                       .function = handle_awss_cmd };
#endif

#ifdef CONFIG_PRINT_HEAP
static void duration_work(void *p)
{
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);
}
#endif

static int mqtt_connected_event_handler(void)
{
    LOG("MQTT Construct  OTA start");
#if defined(OTA_ENABLED) && defined(BUILD_AOS)
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};
    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    HAL_GetDeviceSecret(device_secret);
    static ota_service_t ctx = {0};
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

int application_start(int argc, char **argv)
{
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
    aos_set_log_level(AOS_LL_DEBUG);

    netmgr_init();
    aos_register_event_filter(EV_KEY, linkkit_key_process, NULL);
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_register_event_filter(EV_YUNIO, cloud_service_event, NULL);
    IOT_RegisterCallback(ITE_MQTT_CONNECT_SUCC,mqtt_connected_event_handler);

    IOT_SetLogLevel(5);

#ifdef CONFIG_AOS_CLI
    aos_cli_register_command(&setup_info);
    aos_cli_register_command(&resetcmd);
    aos_cli_register_command(&awss_enable_cmd);
    aos_cli_register_command(&awss_dev_ap_cmd);
    aos_cli_register_command(&awss_cmd);

    /* wait until connection info configurated */
    LOG("waiting for setup_info cmds");
    while (!is_devinfo_config) {
        aos_msleep(1000);
    }
#endif

    aos_task_new("netmgr_start", start_netmgr, NULL, 4096);
    aos_loop_run();

    return 0;
}
