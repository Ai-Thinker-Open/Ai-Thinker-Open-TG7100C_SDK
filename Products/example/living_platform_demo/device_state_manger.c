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
#include "app_entry.h"
#include "device_state_manger.h"
#include "factory.h"
#include "living_platform_demo_main.h"

#define AWSS_TIMEOUT_MS              (10 * 60 * 1000)   //开启AP超时时间
#define CONNECT_AP_FAILED_TIMEOUT_MS (2 * 60 * 1000)    //连接AP超时时间
static uint8_t awss_run_state = 0;                  /* 1-awss running, 0-awss is idle */

static aos_timer_t awss_timeout_timer;
static aos_timer_t connect_ap_failed_timeout_timer = {
    .hdl = NULL,
};
static aos_timer_t clear_awss_flag_timeout_timer;

static void app_start_wifi_awss(void);

static int device_net_state = UNKNOW_STATE;
int get_net_state(void)
{
    return device_net_state;
}

void set_net_state(int state)
{
    device_net_state = state;
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
#else
#warning "Unsupported awss mode!!!"
#endif
    }
    awss_run_state = 0;
}

void device_start_awss(void)
{
    if (awss_run_state == 0) {
#if defined (AWSS_ONESHOT_MODE)
        awss_config_press();
        do_awss();
#elif defined (AWSS_DEV_AP_MODE)
        do_awss_dev_ap();
#elif defined (AWSS_BT_MODE)
        // for combo device, ble_awss and smart_config awss mode can exist simultaneously
        do_ble_awss();
        awss_config_press();
        do_awss();
#else
#warning "Unsupported awss mode!!!"
#endif
    }
    awss_run_state = 1;
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




void indicate_net_state_task(void *arg)
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
                    nCount = 0;
                }
                break;

            case UNCONFIGED:
                nCount++;
                if (nCount >= 2) {
                    nCount = 0;
                }
                break;

            case AWSS_NOT_START:
                if (pre_state != cur_state) {
                    LOG("[net_state]awss timeout");
                }
                break;

            case GOT_AP_SSID:
            case CONNECT_CLOUD_FAILED:
                nCount++;
                if (nCount >= 8) {
                    nCount = 0;
                }
                break;

            case CONNECT_AP_FAILED_TIMEOUT:
                if (pre_state != cur_state) {
                    LOG("[net_state]connect AP failed timeout");
                }
                break;

            case CONNECT_AP_FAILED:
                nCount++;
                if (nCount >= 5) {
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
                    LOG("[net_state]connect cloud success");
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
                }
                break;

            case FACTORY_FAILED_1:
                nCount++;
                if (nCount >= 5) {
                    nCount = 0;
                }
                break;

            case FACTORY_FAILED_2:
                nCount++;
                if (nCount >= 2) {
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



static void app_start_wifi_awss(void)
{
    aos_timer_new_ext(&awss_timeout_timer, timer_func_awss_timeout, NULL, AWSS_TIMEOUT_MS, 0, 1);

    device_start_awss();
}

extern void start_netmgr(void *p);
void check_factory_mode(void)
{
    int ret = 0;
    netmgr_ap_config_t ap_config;
    memset(&ap_config, 0, sizeof(netmgr_ap_config_t));

    netmgr_get_ap_config(&ap_config);
    if (strlen(ap_config.ssid) <= 0) {
        ret = scan_factory_ap();
        if (0 != ret) {     //不存在产测路由，进入配网模式
            set_net_state(UNCONFIGED);
            LOG("not enter factory mode, start awss");
            app_start_wifi_awss();
        }
    }
    else    //重连路由
    {
        set_net_state(GOT_AP_SSID);
        LOG("start connect");
        aos_task_new("netmgr_start", start_netmgr, NULL, 5120);
    }
}
