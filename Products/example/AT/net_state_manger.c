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
#include "net_state_manger.h"
#include "at_main.h"

#define AWSS_TIMEOUT_MS              (10 * 60 * 1000)   //开启AP超时时间
#define CONNECT_AP_FAILED_TIMEOUT_MS (2 * 60 * 1000)    //连接AP超时时间


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
                if(nCount >= 8)
                    nCount = 0;
                /*user handler event*/
                break;

            case UNCONFIGED:
                nCount++;
                if(nCount >= 2)
                    nCount = 0;
                /*user handler event*/
                break;

            case AWSS_NOT_START:
                if(pre_state != cur_state)
                    LOG("[net_state] AWSS_NOT_START");
                /*user handler event*/
                break;

            case GOT_AP_SSID:
            case CONNECT_CLOUD_FAILED:
                nCount++;
                if(nCount >= 8)
                    nCount = 0;
                /*user handler event*/
                break;

            case CONNECT_AP_FAILED_TIMEOUT:
                if (pre_state != cur_state)
                    LOG("[net_state] CONNECT_AP_FAILED_TIMEOUT");
                /*user handler event*/
                break;

            case CONNECT_AP_FAILED:
                nCount++;
                if(nCount >= 5)
                    nCount = 0;
                if(pre_state != cur_state){
                    LOG("[net_state]connect AP failed");
                    if (NULL == connect_ap_failed_timeout_timer.hdl) {
                        aos_timer_new_ext(&connect_ap_failed_timeout_timer,
                            timer_func_connect_ap_failed_timeout, NULL, CONNECT_AP_FAILED_TIMEOUT_MS, 0, 1);
                    } else {
                        /* timer already created, just restart */
                        aos_timer_stop(&connect_ap_failed_timeout_timer);
                        aos_timer_start(&connect_ap_failed_timeout_timer);
                    }
                    awss_dev_ap_stop();
                    awss_stop();
                }
                break;

            case CONNECT_CLOUD_SUCCESS:  
                if(pre_state != cur_state){
                    LOG("[net_state] CONNECT_CLOUD_SUCCESS");
                    aos_timer_stop(&awss_timeout_timer);    
                }         
                /*user handler event*/
                break;

            case APP_BIND_SUCCESS:
                LOG("[net_state] APP_BIND_SUCCESS");
                if(pre_state != cur_state){
                    set_net_state(CONNECT_CLOUD_SUCCESS);
                }
                break;

            case FACTORY_BEGIN:
                LOG("[net_state] FACTORY_BEGIN");
                /*user handler event*/ 
                break;

            case FACTORY_SUCCESS:
                LOG("[net_state] FACTORY_SUCCESS");
                set_net_state(FACTORY_SUCCESS);
                /*user handler event*/
                break;

            case FACTORY_FAILED_1:
                nCount++;
                if(nCount >= 5){
                    nCount = 0;
                }
                /*user handler event*/
                break;

            case FACTORY_FAILED_2:
                nCount++;
                if(nCount >= 2){
                    nCount = 0;
                }
                /*user handler event*/
                break;

            default:
                break;
        }
        aos_msleep(100);
    }

    LOG("exit quick_light mode");
    aos_task_exit(0);
}

static void timer_func_awss_timeout(void *arg1, void *arg2)
{
    LOG("awss timeout, stop awss");
    set_net_state(AWSS_NOT_START);
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
    aos_timer_stop(&awss_timeout_timer);
    aos_timer_free(&awss_timeout_timer);
}

void check_awss_timeout(void)
{
    aos_timer_new_ext(&awss_timeout_timer, timer_func_awss_timeout, NULL, AWSS_TIMEOUT_MS, 0, 1);
}