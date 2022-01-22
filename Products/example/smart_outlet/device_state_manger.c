/*
 *copyright (C) 2015-2018 Alibaba Group Holding Limited
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
#include "vendor.h"
#include "app_entry.h"
#include "device_state_manger.h"
#include "factory.h"
#include "smart_outlet.h"
#ifdef EN_COMBO_NET
#include "combo_net.h"
#endif

#define AWSS_TIMEOUT_MS              (10 * 60 * 1000)
#define CLEAR_AWSS_FLAG_TIMEOUT_MS   (30 * 1000)
#define CONNECT_AP_FAILED_TIMEOUT_MS (2 * 60 * 1000)

static aos_timer_t awss_timeout_timer;
static aos_timer_t connect_ap_failed_timeout_timer = {
    .hdl = NULL,
};
static aos_timer_t clear_awss_flag_timeout_timer;
static uint8_t awss_run_state = 0;                  /* 1-awss running, 0-awss is idle */

static void app_start_wifi_awss(uint8_t discoverable_awss_mode);

static int device_net_state = UNKNOW_STATE;
int get_net_state(void)
{
    return device_net_state;
}

void set_net_state(int state)
{
    device_net_state = state;
}

/* discoverable_awss_mode: enable discoverable provision mode flag, dev-ap or ble-awss 
    0 - all awss mode
    1 - only discoverable provision mode */
void device_start_awss(uint8_t discoverable_awss_mode)
{
    if (awss_run_state == 0) {
#if defined (AWSS_ONESHOT_MODE)
        awss_config_press();
        do_awss();
#elif defined (AWSS_DEV_AP_MODE)
        do_awss_dev_ap();
#elif defined (AWSS_BT_MODE)
        // for combo device, ble_awss and smart_config awss mode can exist simultaneously
        if (discoverable_awss_mode) {
            /* should re-open discoverable provision mode when has ap-info */
            combo_set_ap_state(COMBO_AP_DISCONNECTED);
            do_ble_awss();
        } else {
            do_ble_awss();
            awss_config_press();
            do_awss();
        }
#else
#warning "Unsupported awss mode!!!"
#endif
    }

    awss_run_state = 1;
}

void device_stop_awss(void)
{
    if (awss_run_state != 0) {
#if defined (AWSS_ONESHOT_MODE)
        awss_stop();
#elif defined (AWSS_DEV_AP_MODE)
        awss_dev_ap_stop(); 
#elif defined (AWSS_BT_MODE)
        // for combo device, ble_awss and smart_config awss mode can exist simultaneously
        awss_stop();
        combo_set_awss_state(0);
#ifdef AWSS_SUPPORT_DEV_AP
        awss_dev_ap_stop(); 
#endif
#else
#warning "Unsupported awss mode!!!"
#endif
    }

    awss_run_state = 0;
}

static void timer_func_awss_timeout(void *arg1, void *arg2)
{
    LOG("awss timeout, stop awss");
    set_net_state(AWSS_NOT_START);
    device_stop_awss();
    aos_timer_stop(&awss_timeout_timer);
    aos_timer_free(&awss_timeout_timer);
}

static void timer_func_connect_ap_failed_timeout(void *arg1, void *arg2)
{
    LOG("connect ap failed timeout");
    set_net_state(CONNECT_AP_FAILED_TIMEOUT);
    aos_timer_stop(&connect_ap_failed_timeout_timer);
}

static unsigned char awss_flag = 0;
int init_awss_flag(void)
{
    unsigned char value;
    int ret, len = sizeof(value);

    ret = aos_kv_get("awss_flag", &value, &len);
    if (ret == 0 && len > 0) {
        awss_flag = value;
    }
    return 0;
}

#define AWSS_REBOOT_TIMEOUT     (4 * 1000)
#define AWSS_RESET_TIMEOUT      (6 * 1000)
#define KEY_PRESSED_VALID_TIME  100
#define KEY_DETECT_INTERVAL     50
#define AWSS_REBOOT_CNT         AWSS_REBOOT_TIMEOUT / KEY_DETECT_INTERVAL
#define AWSS_RESET_CNT          AWSS_RESET_TIMEOUT / KEY_DETECT_INTERVAL
#define KEY_PRESSED_CNT         KEY_PRESSED_VALID_TIME / KEY_DETECT_INTERVAL

void key_detect_event_task(void *arg)
{
    int nCount = 0, awss_mode = 0;
    int timeout = (AWSS_REBOOT_CNT < AWSS_RESET_TIMEOUT)? AWSS_REBOOT_CNT : AWSS_RESET_TIMEOUT;

    while (1) {
        if (!product_get_key()) {
            nCount++;
            LOG("nCount :%d", nCount);
        } else {
            if (nCount >= KEY_PRESSED_CNT && nCount < timeout) {
                if (product_get_switch() == ON) {
                    product_set_switch(OFF);
                    user_post_powerstate(OFF);
                } else {
                    product_set_switch(ON);
                    user_post_powerstate(ON);
                }
            }
            if ((awss_flag == 0) && (nCount >= AWSS_REBOOT_CNT)) {
                LOG("do awss reboot");
                do_awss_reboot();
                break;
            } else if ((awss_flag == 1) && (nCount > AWSS_RESET_CNT)) {
                LOG("do awss reset");
                do_awss_reset();
                break;
            }
            nCount = 0;
        }
        if ((awss_flag == 0) && (nCount >= AWSS_REBOOT_CNT && awss_mode == 0)) {
            set_net_state(RECONFIGED);
            awss_mode = 1;
        } else if ((awss_flag == 1) && (nCount > AWSS_RESET_CNT && awss_mode == 0)) {
            set_net_state(UNCONFIGED);
            awss_mode = 1;
        }
        aos_msleep(KEY_DETECT_INTERVAL);
    }
    aos_task_exit(0);
}

static void indicate_net_state_task(void *arg)
{
    uint32_t nCount = 0;
    uint32_t duration = 0;
    int pre_state = UNKNOW_STATE;
    int cur_state = UNKNOW_STATE;
    int switch_stat = 0;

    while (1) {
        pre_state = cur_state;
        cur_state = get_net_state();
        switch (cur_state) {
            case RECONFIGED:
                nCount++;
                if (nCount >= 8) {
                    product_toggle_led();
                    nCount = 0;
                }
                break;

            case UNCONFIGED:
                nCount++;
                if (nCount >= 2) {
                    product_toggle_led();
                    nCount = 0;
                }
                break;

            case AWSS_NOT_START:
                if (pre_state != cur_state) {
                    switch_stat = (int)product_get_switch();
                    LOG("[net_state]awss timeout, set led %d", switch_stat);
                    product_set_led(switch_stat);
                }
                break;

            case GOT_AP_SSID:
            case CONNECT_CLOUD_FAILED:
                nCount++;
                if (nCount >= 8) {
                    product_toggle_led();
                    nCount = 0;
                }
                break;

            case CONNECT_AP_FAILED_TIMEOUT:
                if (pre_state != cur_state) {
                    switch_stat = (int)product_get_switch();
                    LOG("[net_state]connect AP failed timeout, set led %d", switch_stat);
                    product_set_led(switch_stat);
                }
                break;

            case CONNECT_AP_FAILED:
                nCount++;
                if (nCount >= 5) {
                    product_toggle_led();
                    nCount = 0;
                }
                if (pre_state != cur_state) {
                    LOG("[net_state]connect AP failed");
                    if (NULL == connect_ap_failed_timeout_timer.hdl) {
                        aos_timer_new_ext(&connect_ap_failed_timeout_timer,
                            timer_func_connect_ap_failed_timeout, NULL, CONNECT_AP_FAILED_TIMEOUT_MS, 0, 1);
                    } else {
                        /* timer already created, just restart */
                        aos_timer_stop(&connect_ap_failed_timeout_timer);
                        aos_timer_start(&connect_ap_failed_timeout_timer);
                    }
                    device_stop_awss();
                }
                break;

            case CONNECT_CLOUD_SUCCESS:
                if (pre_state != cur_state) {
                    aos_timer_stop(&awss_timeout_timer);

                    switch_stat = (int)product_get_switch();
                    LOG("[net_state]connect cloud success, set led %d", switch_stat);
                    product_set_led(switch_stat);
                }
                break;

            case APP_BIND_SUCCESS:
                if (pre_state != cur_state) {
                    set_net_state(CONNECT_CLOUD_SUCCESS);
                }
                break;

            case FACTORY_BEGIN:
                LOG("[net_state]factory begin");
                set_net_state(FACTORY_SUCCESS);
                break;

            case FACTORY_SUCCESS:
                if (pre_state != cur_state) {
                    LOG("[net_state]factory success, set led OFF");
                    product_set_led(OFF);
                }
                break;

            case FACTORY_FAILED_1:
                nCount++;
                if (nCount >= 5) {
                    product_toggle_led();
                    nCount = 0;
                }
                break;

            case FACTORY_FAILED_2:
                nCount++;
                if (nCount >= 2) {
                    product_toggle_led();
                    nCount = 0;
                }
                break;

            default:
                break;
        }
        aos_msleep(100);
    }

    LOG("exit quick_light mode");
    aos_task_exit(0);
}

static void timer_func_clear_awss_flag_timeout(void *arg1, void *arg2)
{
    LOG("clear awss flag");
    awss_flag = 0;
    aos_kv_del("awss_flag");
    aos_timer_stop(&clear_awss_flag_timeout_timer);
    aos_timer_free(&clear_awss_flag_timeout_timer);
}

static void app_start_wifi_awss(uint8_t discoverable_awss_mode)
{
    aos_timer_new_ext(&awss_timeout_timer, timer_func_awss_timeout, NULL, AWSS_TIMEOUT_MS, 0, 1);
    aos_timer_new_ext(&clear_awss_flag_timeout_timer, timer_func_clear_awss_flag_timeout, NULL, CLEAR_AWSS_FLAG_TIMEOUT_MS, 0, 1);
    device_start_awss(discoverable_awss_mode);
}

extern void start_netmgr(void *p);
void check_factory_mode(void)
{
    int ret = 0;
    netmgr_ap_config_t ap_config;
    memset(&ap_config, 0, sizeof(netmgr_ap_config_t));

#if (defined (TG7100CEVB))
    aos_task_new("indicate net state", indicate_net_state_task, NULL, 1024 + 2 * 1024);
#else
    aos_task_new("indicate net state", indicate_net_state_task, NULL, 1024);
#endif

    netmgr_get_ap_config(&ap_config);
    if (strlen(ap_config.ssid) <= 0) {
        LOG("scan factory ap, set led ON");
        product_set_led(ON);

        ret = scan_factory_ap();
        if (0 != ret) {
            set_net_state(UNCONFIGED);
            LOG("not enter factory mode, start awss");
            app_start_wifi_awss(0);
        }
    } else {
        if (awss_flag == 1) {
            LOG("start awss with netconfig exist");
            set_net_state(RECONFIGED);
            aos_kv_del("awss_flag");
            app_start_wifi_awss(awss_flag);
        } else {
            set_net_state(GOT_AP_SSID);
            LOG("start connect");
            aos_task_new("netmgr_start", start_netmgr, NULL, 5120);
        }
    }
}
