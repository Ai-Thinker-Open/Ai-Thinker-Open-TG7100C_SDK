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

#include <k_api.h>

#include "living_platform_main.h"
#include "app_entry.h"
#include "living_platform_cmds.h"
#include "living_platform_ut.h"

static char awss_running = 0;

#ifdef AWSS_BATCH_DEVAP_ENABLE
#define DEV_AP_ZCONFIG_TIMEOUT_MS 120000 // (ms)
extern void awss_set_config_press(uint8_t press);
extern uint8_t awss_get_config_press(void);
extern void zconfig_80211_frame_filter_set(uint8_t filter, uint8_t fix_channel);

static aos_timer_t dev_ap_zconfig_timeout_timer;
static uint8_t g_dev_ap_zconfig_timer = 0; // this timer create once and can restart
static uint8_t g_dev_ap_zconfig_run = 0;

static void timer_func_devap_zconfig_timeout(void *arg1, void *arg2)
{
    living_platform_info("%s run\n", __func__);

    if (awss_get_config_press())
    {
        // still in zero wifi provision stage, should stop and switch to dev ap
        living_platform_do_awss_dev_ap();
    }
    else
    {
        // zero wifi provision finished
    }

    awss_set_config_press(0);
    zconfig_80211_frame_filter_set(0xFF, 0xFF);
    g_dev_ap_zconfig_run = 0;
    aos_timer_stop(&dev_ap_zconfig_timeout_timer);
}

static void awss_dev_ap_switch_to_zeroconfig(void *p)
{
    living_platform_info("%s run\n", __func__);
    // Stop dev ap wifi provision
    awss_dev_ap_stop();
    // Start and enable zero wifi provision
    iotx_event_regist_cb(linkkit_event_monitor);
    awss_set_config_press(1);

    // Start timer to count duration time of zero provision timeout
    if (!g_dev_ap_zconfig_timer)
    {
        aos_timer_new(&dev_ap_zconfig_timeout_timer, timer_func_devap_zconfig_timeout, NULL, DEV_AP_ZCONFIG_TIMEOUT_MS, 0);
        g_dev_ap_zconfig_timer = 1;
    }
    aos_timer_start(&dev_ap_zconfig_timeout_timer);

    // This will hold thread, when awss is going
    netmgr_start(true);

    living_platform_info("%s exit\n", __func__);
    aos_task_exit(0);
}

int awss_dev_ap_modeswitch_cb(uint8_t awss_new_mode, uint8_t new_mode_timeout, uint8_t fix_channel)
{
    if ((awss_new_mode == 0) && !g_dev_ap_zconfig_run)
    {
        g_dev_ap_zconfig_run = 1;
        // Only receive zero provision packets
        zconfig_80211_frame_filter_set(0x00, fix_channel);
        living_platform_info("switch to awssmode %d, mode_timeout %d, chan %d\n", 0x00, new_mode_timeout, fix_channel);
        // switch to zero config
        aos_task_new("devap_to_zeroconfig", awss_dev_ap_switch_to_zeroconfig, NULL, 2048);
    }
}
#endif

static void awss_close_dev_ap(void *p)
{
    awss_dev_ap_stop();
    living_platform_info("%s exit\n", __func__);
    aos_task_exit(0);
}

static void awss_open_dev_ap(void *p)
{
    iotx_event_regist_cb(linkkit_event_monitor);
    living_platform_info("%s\n", __func__);
    if (netmgr_start(false) != 0)
    {
        aos_msleep(2000);
#ifdef AWSS_BATCH_DEVAP_ENABLE
        awss_dev_ap_reg_modeswit_cb(awss_dev_ap_modeswitch_cb);
#endif
        awss_dev_ap_start();
    }
    aos_task_exit(0);
}

static void stop_netmgr(void *p)
{
    awss_stop();
    living_platform_info("%s\n", __func__);
    aos_task_exit(0);
}

void living_platform_start_netmgr(void *p)
{
    iotx_event_regist_cb(linkkit_event_monitor);
    netmgr_start(true);
    aos_task_exit(0);
}

void living_platform_do_awss_active(void)
{
    living_platform_info("living_platform_do_awss_active %d\n", awss_running);
    awss_running = 1;
#ifdef WIFI_PROVISION_ENABLED
    extern int awss_config_press();
    awss_config_press();
#endif
}

void living_platform_do_awss_dev_ap(void)
{
    aos_task_new("netmgr_stop", stop_netmgr, NULL, 4096);
    aos_task_new("dap_open", awss_open_dev_ap, NULL, 4096);
}

void living_platform_do_awss(void)
{
    aos_task_new("dap_close", awss_close_dev_ap, NULL, 2048);
    aos_task_new("netmgr_start", living_platform_start_netmgr, NULL, 4096);
}

static void linkkit_reset(void *p)
{
    iotx_sdk_reset_local();
    HAL_Reboot();
}

void living_platform_awss_reset(void)
{
#ifdef WIFI_PROVISION_ENABLED
    aos_task_new("reset", (void (*)(void *))iotx_sdk_reset, NULL, 4096); // stack taken by iTLS is more than taken by TLS.
#endif
    aos_post_delayed_action(2000, linkkit_reset, NULL);
}

#ifdef CONFIG_AOS_CLI
static void handle_reset_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(living_platform_awss_reset, NULL);
}

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(living_platform_do_awss_active, NULL);
}

static void handle_dev_ap_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(living_platform_do_awss_dev_ap, NULL);
}

static void handle_awss_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(living_platform_do_awss, NULL);
}

#if defined(OFFLINE_LOG_UT_TEST)
static void handle_offline_log_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int ret = 0;

    if (argc < 2)
    {
        aos_cli_printf("params err,oll [save/upload/erase] or [read [0-1]] or [write [0-3] \"test offline log\"]\r\n");
        return;
    }

    aos_cli_printf("cmd:%s\r\n", argv[1]);

    if (!strcmp("save", argv[1]) && argc == 2)
    {
        diagnosis_offline_log_save_all();
    }
    else if (!strcmp("upload", argv[1]) && argc == 4)
    {
        int port = atoi(argv[3]);

        diagnosis_offline_log_upload(argv[2], port);
    }
    else if (!strcmp("write", argv[1]) && argc == 4)
    {
        int loglevel = atoi(argv[2]);

        if (loglevel >= 0 && loglevel <= 3)
        {
            diagnosis_offline_log(loglevel, "%s\\n", argv[3]);
        }
    }
    else if (!strcmp("read", argv[1]) && argc == 3)
    {
        int log_mode = atoi(argv[2]);

        diagnosis_offline_log_read_all(log_mode);
    }
    else if (!strcmp("erase", argv[1]) && argc == 2)
    {
        diagnosis_offline_log_erase_flash_desc();
    }
    else
    {
        aos_cli_printf("cmd:%s is not support\r\n", argv[1]);
    }

    if (ret != 0)
    {
        aos_cli_printf("handle oll cmd:%s failed:%d\r\n", argv[1], ret);
    }

    return -1;
}

static struct cli_command offline_log_cmd = {.name = "oll",
                                             .help = "oll [save]/[erase] or [upload host port] or [read [0-1]] or [write [0-3] \"test offline log\"]",
                                             .function = handle_offline_log_cmd};

#endif

static void handle_linkkey_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int index = 0;
    char key_buf[MAX_KEY_LEN];

    if (argc == 1)
    {
        int len = 0;
        char product_key[PRODUCT_KEY_LEN + 1] = {0};
        char product_secret[PRODUCT_SECRET_LEN + 1] = {0};
        char device_name[DEVICE_NAME_LEN + 1] = {0};
        char device_secret[DEVICE_SECRET_LEN + 1] = {0};

        len = PRODUCT_KEY_LEN + 1;
        memset(key_buf, 0, MAX_KEY_LEN);
        memset(product_key, 0, sizeof(product_key));
        HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PK, index);
        aos_kv_get(key_buf, product_key, &len);

        len = PRODUCT_SECRET_LEN + 1;
        memset(key_buf, 0, MAX_KEY_LEN);
        memset(product_secret, 0, sizeof(product_secret));
        HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PS, index);
        aos_kv_get(key_buf, product_secret, &len);

        len = DEVICE_NAME_LEN + 1;
        memset(key_buf, 0, MAX_KEY_LEN);
        memset(device_name, 0, sizeof(device_name));
        HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DN, index);
        aos_kv_get(key_buf, device_name, &len);

        len = DEVICE_SECRET_LEN + 1;
        memset(key_buf, 0, MAX_KEY_LEN);
        memset(device_secret, 0, sizeof(device_secret));
        HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DS, index);
        aos_kv_get(key_buf, device_secret, &len);

        aos_cli_printf("PK=%s.\r\n", product_key);
        aos_cli_printf("PS=%s.\r\n", product_secret);
        aos_cli_printf("DN=%s.\r\n", device_name);
        aos_cli_printf("DS=%s.\r\n", device_secret);
    }
    else if (argc == 5)
    {
        int devid = 0;

        memset(key_buf, 0, MAX_KEY_LEN);
        HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PK, devid);
        aos_kv_set(key_buf, argv[1], strlen(argv[1]) + 1, 1);

        memset(key_buf, 0, MAX_KEY_LEN);
        HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DN, devid);
        aos_kv_set(key_buf, argv[2], strlen(argv[2]) + 1, 1);

        memset(key_buf, 0, MAX_KEY_LEN);
        HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DS, devid);
        aos_kv_set(key_buf, argv[3], strlen(argv[3]) + 1, 1);

        memset(key_buf, 0, MAX_KEY_LEN);
        HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PS, devid);
        aos_kv_set(key_buf, argv[4], strlen(argv[4]) + 1, 1);

        aos_cli_printf("Done\r\n");
    }
    else if (argc == 6)
    {
        int devid = atoi(argv[1]);

        if (devid >= 0 && devid <= MAX_DEVICES_META_NUM)
        {
            memset(key_buf, 0, MAX_KEY_LEN);
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PK, devid);
            aos_kv_set(key_buf, argv[2], strlen(argv[2]) + 1, 1);

            memset(key_buf, 0, MAX_KEY_LEN);
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DN, devid);
            aos_kv_set(key_buf, argv[3], strlen(argv[3]) + 1, 1);

            memset(key_buf, 0, MAX_KEY_LEN);
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DS, devid);
            aos_kv_set(key_buf, argv[4], strlen(argv[4]) + 1, 1);

            memset(key_buf, 0, MAX_KEY_LEN);
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PS, devid);
            aos_kv_set(key_buf, argv[5], strlen(argv[5]) + 1, 1);

            aos_cli_printf("Done\r\n");
        }
        else
        {
            aos_cli_printf("Err:devid range[0-%d]", MAX_DEVICES_META_NUM - 1);
        }
    }
    else
    {
        aos_cli_printf("Params Err\r\n");
    }

    return;
}

#ifdef DEV_ERRCODE_ENABLE
#define DEV_DIAG_CLI_SET_ERRCODE "set_err"
#define DEV_DIAG_CLI_ERRCODE_MANUAL_MSG "errcode was manually set"
#define DEV_DIAG_CLI_GET_ERRCODE "get_err"
#define DEV_DIAG_CLI_DEL_ERRCODE "del_err"

static void handle_dev_serv_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    uint16_t err_code = 0;
    char err_msg[DEV_ERRCODE_MSG_MAX_LEN] = {0};
    uint8_t ret = 0;
    if (argc == 1)
    {
        aos_cli_printf("device diagnosis service test, please input more parameters.\r\n");
    }
    else if (argc > 1)
    {
        if ((argc == 3) && (!strcmp(argv[1], DEV_DIAG_CLI_SET_ERRCODE)))
        {
            err_code = (uint16_t)strtoul(argv[2], NULL, 0);
            ret = dev_errcode_kv_set(err_code, DEV_DIAG_CLI_ERRCODE_MANUAL_MSG);
            if (ret == 0)
            {
                aos_cli_printf("manually set err_code=%d\r\n", err_code);
            }
            else
            {
                aos_cli_printf("manually set err_code failed!\r\n");
            }
        }
        else if ((argc == 2) && (!strcmp(argv[1], DEV_DIAG_CLI_GET_ERRCODE)))
        {
            ret = dev_errcode_kv_get(&err_code, err_msg);
            if (ret == 0)
            {
                aos_cli_printf("get err_code=%d, err_msg=%s\r\n", err_code, err_msg);
            }
            else
            {
                aos_cli_printf("get err_code fail or no err_code found!\r\n");
            }
        }
        else if ((argc == 2) && (!strcmp(argv[1], DEV_DIAG_CLI_DEL_ERRCODE)))
        {
            dev_errcode_kv_del();
            aos_cli_printf("del err_code\r\n");
        }
    }
    else
    {
        aos_cli_printf("Error: %d\r\n", __LINE__);
        return;
    }
}
#endif

#ifdef MANUFACT_AP_FIND_ENABLE
#define MANUAP_CLI_OPEN "open"
#define MANUAP_CLI_CLOSE "close"

static void handle_manuap_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int ret = 0;
    uint8_t manuap_flag = 0;
    int manu_flag_len = sizeof(manuap_flag);
    if (argc == 1)
    {
        ret = aos_kv_get(MANUAP_CONTROL_KEY, &manuap_flag, &manu_flag_len);
        if (ret == 0)
        {
            if (manuap_flag)
            {
                aos_cli_printf("manuap state is open\r\n");
            }
            else
            {
                aos_cli_printf("manuap state is close\r\n");
            }
        }
        else
        {
            aos_cli_printf("no manuap state stored\r\n");
        }
    }
    else if (argc == 2)
    {
        if (!strcmp(argv[1], MANUAP_CLI_OPEN))
        {
            manuap_flag = 1;
            ret = aos_kv_set(MANUAP_CONTROL_KEY, &manuap_flag, sizeof(manuap_flag), 1);
            if (ret == 0)
            {
                aos_cli_printf("manuap opened\r\n");
            }
        }
        else if (!strcmp(argv[1], MANUAP_CLI_CLOSE))
        {
            manuap_flag = 0;
            ret = aos_kv_set(MANUAP_CONTROL_KEY, &manuap_flag, sizeof(manuap_flag), 1);
            if (ret == 0)
            {
                aos_cli_printf("manuap closed\r\n");
            }
        }
        else
        {
            aos_cli_printf("param input err\r\n");
        }
    }
    else
    {
        aos_cli_printf("param input err\r\n");
        return;
    }
}
#endif

#if defined(TSL_POST_UT_TEST_ENABLE)
static void handle_tsl_post_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (4 != argc) {
        printf("input error\r\n");
    } else {
        living_platform_ut_tsl_unify_post(argv[1], &argv[2]);
    }
}
#endif

#if defined(SYS_TIMER_UT_TEST_ENABLE)
static void handle_systimer_test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (3 != argc) {
        printf("input error\r\n");
    } else {
        living_platform_ut_systimer_test(argv[1], &argv[2]);
    }
}
#endif

static struct cli_command resetcmd = {.name = "reset",
                                      .help = "factory reset",
                                      .function = handle_reset_cmd};

static struct cli_command awss_enable_cmd = {.name = "active_awss",
                                             .help = "active_awss [start]",
                                             .function = handle_active_cmd};
static struct cli_command awss_dev_ap_cmd = {.name = "dev_ap",
                                             .help = "awss_dev_ap [start]",
                                             .function = handle_dev_ap_cmd};
static struct cli_command awss_cmd = {.name = "awss",
                                      .help = "awss [start]",
                                      .function = handle_awss_cmd};
static struct cli_command linkkeycmd = {.name = "linkkey",
                                        .help = "set/get linkkit keys. linkkey [devid] [<Product Key> <Device Name> <Device Secret> <Product Secret>]",
                                        .function = handle_linkkey_cmd};

#ifdef DEV_ERRCODE_ENABLE
static struct cli_command dev_service_cmd = {.name = "dev_serv",
                                             .help = "device diagnosis service tests. like errcode, and log report",
                                             .function = handle_dev_serv_cmd};
#endif
#ifdef MANUFACT_AP_FIND_ENABLE
static struct cli_command manuap_cmd = {.name = "manuap",
                                        .help = "manuap [open] or manuap [close]",
                                        .function = handle_manuap_cmd};
#endif

#if defined(TSL_POST_UT_TEST_ENABLE)
static struct cli_command tsl_post_cmd = {.name = "tsl_post",
                                        .help = "tsl_post <type: 0 - num, 1 - string> <name> <value>",
                                        .function = handle_tsl_post_cmd};
#endif

#if defined(SYS_TIMER_UT_TEST_ENABLE)
    static struct cli_command systimer_test_cmd = {.name = "systimer_test",
                                        .help = "<start/stop> <seconds>",
                                        .function = handle_systimer_test_cmd};
#endif

int living_platform_register_cmds(void)
{
    aos_cli_register_command(&resetcmd);
    aos_cli_register_command(&awss_enable_cmd);
    aos_cli_register_command(&awss_dev_ap_cmd);
    aos_cli_register_command(&awss_cmd);
    aos_cli_register_command(&linkkeycmd);
#if defined(TSL_POST_UT_TEST_ENABLE)
    aos_cli_register_command(&tsl_post_cmd);
#endif
#if defined(SYS_TIMER_UT_TEST_ENABLE)
    aos_cli_register_command(&systimer_test_cmd);
#endif
#ifdef MANUFACT_AP_FIND_ENABLE
    aos_cli_register_command(&manuap_cmd);
#endif
    return 0;
}
#endif
