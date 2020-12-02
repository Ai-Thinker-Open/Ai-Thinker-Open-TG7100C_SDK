/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __DEVICE_STATE_MANGER_H__
#define __DEVICE_STATE_MANGER_H__

#include <aos/aos.h>

typedef enum {
    RECONFIGED = 0,            //reconfig with netconfig exist
    UNCONFIGED,                //awss start
    AWSS_NOT_START,            //standby mode(not start softAP)
    GOT_AP_SSID,               //connect AP successfully
    CONNECT_CLOUD_SUCCESS,     //connect cloud successfully
    CONNECT_CLOUD_FAILED,      //connect cloud failded
    CONNECT_AP_FAILED,         //connect ap failed
    CONNECT_AP_FAILED_TIMEOUT, //connect ap failed timeout
    APP_BIND_SUCCESS,          //bind sucessfully, wait cmd from APP
    FACTORY_BEGIN,             //factory test begin
    FACTORY_SUCCESS,           //factory test successfully
    FACTORY_FAILED_1,          //factory test failed for rssi < -60dbm
    FACTORY_FAILED_2,          //factory test failed
    UNKNOW_STATE
    //local:连上路由但未连上服务器  cloud:连上服务器    offline:接连中或掉线
} eNetState;

int get_net_state(void);
void set_net_state(int state);
void indicate_net_state_task(void *arg);
void check_awss_timeout(void);

#endif


