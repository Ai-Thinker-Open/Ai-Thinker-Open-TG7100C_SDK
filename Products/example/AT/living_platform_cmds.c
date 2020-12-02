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
#include "cJSON.h"

#include <k_api.h>

#include "at_main.h"
#include "app_entry.h"
#include "living_platform_cmds.h"
#include "combo_net.h"
#include "vendor.h"
#include "net_state_manger.h"
#include <hal/soc/gpio.h>


typedef struct {
    uint8_t       port;    /* gpio port */
    gpio_config_t config;  /* gpio config */
} GPIO_CFG;

extern aos_queue_t *g_property_report_queue_id;
extern aos_queue_t *g_cmd_msg_queue_id;

uint32_t meta_addr = 0x1FC100;      //存储三元组地址

void uart_push_data(char *msg)
{
    my_aos_uart_send(msg, strlen(msg), 0);
}


static void handle_set_linkkey_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc == 5 || argc == 6) {
        aos_kv_set(KV_KEY_PK, argv[1], strlen(argv[1]) + 1, 1);
        aos_kv_set(KV_KEY_DN, argv[2], strlen(argv[2]) + 1, 1);
        aos_kv_set(KV_KEY_DS, argv[3], strlen(argv[3]) + 1, 1);
        aos_kv_set(KV_KEY_PS, argv[4], strlen(argv[4]) + 1, 1);
        if (argc == 6)
            aos_kv_set(KV_KEY_PD, argv[5], strlen(argv[5]) + 1, 1);
        uart_push_data("OK\r\n");
    } else {
        uart_push_data("ERROR\r\n");
        return;
    }
    
    // if(argc < 6)
    // {
    //     uart_push_data("ERROR");
    // }else{
    //     char meta[200] = {0};
    //     //拼接五元组
    //     sprintf(meta, "[%s %s %s %s %s]", argv[1], argv[2], argv[3], argv[4], argv[5]);

    //     bl_flash_erase(meta_addr, 200);
    //     bl_flash_write(meta_addr, meta,  sizeof(meta));
    //     uart_push_data("OK");
    // }

}

static void handle_get_linkkey_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int len = 0;
    char product_key[PRODUCT_KEY_LEN + 1] = { 0 };
    char product_secret[PRODUCT_SECRET_LEN + 1] = { 0 };
    char device_name[DEVICE_NAME_LEN + 1] = { 0 };
    char device_secret[DEVICE_SECRET_LEN + 1] = { 0 };
    char pidStr[9] = { 0 };
    char buf[255] = {0};

    len = PRODUCT_KEY_LEN + 1;
    aos_kv_get(KV_KEY_PK, product_key, &len);
    

    len = PRODUCT_SECRET_LEN + 1;
    aos_kv_get(KV_KEY_PS, product_secret, &len);

    len = DEVICE_NAME_LEN + 1;
    aos_kv_get(KV_KEY_DN, device_name, &len);

    len = DEVICE_SECRET_LEN + 1;
    aos_kv_get(KV_KEY_DS, device_secret, &len);

    // len = sizeof(pidStr);
    // aos_kv_get(KV_KEY_PD, pidStr, &len);

    aos_cli_printf("Product Key=%s.\r\n", product_key);
    aos_cli_printf("Device Name=%s.\r\n", device_name);
    aos_cli_printf("Device Secret=%s.\r\n", device_secret);
    aos_cli_printf("Product Secret=%s.\r\n", product_secret);


    len = sizeof(pidStr);
    if (aos_kv_get(KV_KEY_PD, pidStr, &len) == 0) {
        sprintf(buf,"+LINKKEYCONFIG=\"%s\",\"%s\",\"%s\",\"%s\",\"%d\"\r\n",product_key,device_name,device_secret,product_secret,atoi(pidStr));
        uart_push_data(buf);
        return;
    }
    sprintf(buf,"+LINKKEYCONFIG=\"%s\",\"%s\",\"%s\",\"%s\"\r\n",product_key,device_name,device_secret,product_secret);
    uart_push_data(buf);



    // char product_key[PRODUCT_KEY_LEN + 1] = { 0 };
    // char product_secret[PRODUCT_SECRET_LEN + 1] = { 0 };
    // char device_name[DEVICE_NAME_LEN + 1] = { 0 };
    // char device_secret[DEVICE_SECRET_LEN + 1] = { 0 };
    // char product_ID[9] = { 0 };
    // char buf[255] = {0};
    // char meta[200] = {0};
    // bl_flash_read(meta_addr, meta, 200);
    // int i = 0;
    // char* token;
    // meta[strlen(meta) - 1] = '\0';
    // token = strtok(meta+1, " ");
    // while(token != NULL) {
    //     if(i == 0){
    //         memcpy(product_key, token, strlen(token));
    //         printf("product_key = %s\n\n", product_key);
    //     }
    //     else if(i == 1){
    //         memcpy(device_name, token, strlen(token));
    //         printf("device_name = %s\n\n", device_name);
    //     }
    //     else if(i == 2){
    //         memcpy(device_secret, token, strlen(token));
    //         printf("device_secret = %s\n\n", device_secret);
    //     }
    //     else if(i == 3){
    //         memcpy(product_secret, token, strlen(token));
    //         printf("product_secret = %s\n\n", product_secret);
    //     }
    //     else if(i == 4){
    //         char *pb =  strstr(token,"]");
    //         int len = pb - token;
    //         for(i=0; i<len; i++){
    //             product_ID[i] = token[i];
    //         }
    //         printf("product_ID = %s\n",product_ID);
    //     }
    //     i++;
    //     token = strtok(NULL, " ");
    // }
    // sprintf(buf,"+LINKKEYCONFIG=\"%s\",\"%s\",\"%s\",\"%s\",\"%d\"\r\n",product_key,device_name,device_secret,product_secret,atoi(product_ID));
    // uart_push_data(buf);

}


static void handle_at_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    uart_push_data("OK\r\n");
}

static void handle_veriosn_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    char buf[100] = "0";
    sprintf(buf,"at version:%s\r\nsdk version:%s\r\nfirmware version:%s\r\n", AT_VERSION,SDK_VERSION,aos_get_app_version());
    uart_push_data(buf);
}

static void handle_get_mac_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    uint8_t mac[6] = {0};
    bl_flash_efuse_read_mac(mac);
    // bl_efuse_read_mac_factory(mac);
    char buf[40] = {0};
    sprintf(buf, "+CIPSTAMAC_CUR: %02X:%02X:%02X:%02X:%02X:%02X\r\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    uart_push_data(buf);
    uart_push_data("OK\r\n");
}

static void handle_reboot_cmd(char *buf, int len, int argc, char **argv)
{
    uart_push_data("OK\r\n");
    aos_msleep(5);
    hal_reboot();
}

static void handle_restore_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    uart_push_data("OK\r\n");

    aos_schedule_call((aos_call_t)do_awss_reset, NULL);
}

// static void timer_func_awss_timeout(void *arg1, void *arg2)
// {
//     LOG("awss timeout, stop awss");
//     set_net_state(AWSS_NOT_START);
//     awss_dev_ap_stop();
//     awss_stop();
//     aos_timer_stop(&awss_timeout_timer);
//     aos_timer_free(&awss_timeout_timer);
// }

static void handle_bleap_connect_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    check_awss_timeout();
    netmgr_ap_config_t ap_config;
    memset(&ap_config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&ap_config);
    if(strlen(ap_config.ssid) <= 0){
        set_net_state(UNCONFIGED);  
        
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


    
        uart_push_data("OK\r\n");
        return;
    }else{
        set_net_state(GOT_AP_SSID);
        uart_push_data("OK\r\n");
        return;
    }
}

//数据格式  AT+SENDJSON PROPERTY {"PowerSwitch":1}
static void handle_sendjson_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    if(strcmp("PROPERTY", argv[1]) == 0){       //上报产品属性
        if(0 != aos_queue_send(g_property_report_queue_id, argv[2], strlen(argv[2]))){
            LOG("###############ERROR: report_light_property aos_queue_send failed! #################\r\n");
            // cJSON_Delete(root);
            return;
        }
    }else if(strcmp("EVENT", argv[1]) == 0){    //上报产品事件
        // printf("%s-%s\n",argv[2],argv[3]);
        if(-1 != IOT_Linkkit_TriggerEvent(user_example_ctx->master_devid, argv[2], strlen(argv[3]), argv[3], strlen(argv[2])))
        {
           uart_push_data("OK\r\n"); 
           return;
        }
        else
        {
            uart_push_data("post event error\r\n"); 
            return;
        }
        
    } else {
        uart_push_data("parm error\r\n");
    }
    return;
}

static void handle_recvjson_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    char msg[512] = {0};
    unsigned int rcvLen;
    if(aos_queue_recv(g_cmd_msg_queue_id,10, msg, &rcvLen) == 0){
        uart_push_data((char *)msg);
    }else{
        uart_push_data("ERROR\r\n");
    }
}

static void handle_connectedcheck_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int connect_status = get_net_state();
    if(connect_status == CONNECT_CLOUD_SUCCESS){
        uart_push_data("+CONNECTEDCHECK:1\r\nOK");
    }else{
        uart_push_data("+CONNECTEDCHECK:0\r\nERROR");
    }
}

static void handle_zerocongif_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss, NULL);
    aos_msleep(100);
    aos_schedule_call((aos_call_t)do_awss_active, NULL);
}

static void handle_led_test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (!strcmp(argv[1], "1"))
    {
        product_set_led(ON);
        uart_push_data("OK\r\n");
    }
    else if (!strcmp(argv[1], "0"))
    {
        product_set_led(OFF);
        uart_push_data("OK\r\n");
    }
    else
    {
        uart_push_data("ERROR\r\n");
    }
}

static void set_gpio_dir_handler(char *pwbuf, int blen, int argc, char **argv)
{
    printf("port: %s\tmodel:%s\n",argv[1],argv[2]);
}


static const struct cli_command at_cmd_list[] = {
    // { "AT+LINKKEYCONFIG",            "set linkkit keys. linkkey [devid] [<Product Key> <Device Name> <Device Secret> <Product Secret>]", handle_set_linkkey_cmd},
    // { "AT+LINKKEYCONFIG?",           "get linkkit keys. linkkey [devid] [<Product Key> <Device Name> <Device Secret> <Product Secret>]", handle_get_linkkey_cmd},    
    
    { "AT",                     "Null cmd, always returns OK",                  handle_at_cmd },
    { "AT+GMR",                 "veriosn info",                                 handle_veriosn_cmd },       //获取版本号
    { "AT+RST",                 "reboot system",                                handle_reboot_cmd },        //重启
    { "AT+CIPSTAMAC_DEF?",      "get mac address",                              handle_get_mac_cmd },       //获得MAC地址
    { "AT+RESTORE",             "factory reset",                                handle_restore_cmd },       //恢复出厂设置
    { "AT+ALIBTSMARTCONFIG",    "Bluetooth auxiliary distribution network",     handle_bleap_connect_cmd }, //蓝牙辅助配网
    { "AT+SENDJSON",            "send json data to server",                     handle_sendjson_cmd },      //上行数据
    { "AT+RECVJSON",            "recv cloud json data",                         handle_recvjson_cmd },      //接收下行数据
    { "AT+CONNECTEDCHECK?",     "query cloud connection status",                handle_connectedcheck_cmd}, //查询连接云端状态
    // { "AT+ZEROCONFIG",          "zerconfig distribution network",               handle_zerocongif_cmd},
    // {"AT+SYSGPIODIR",		    "set the gpio dir mode",                        set_gpio_dir_handler},
	// {"AT+SYSGPIOWRITE",			set_gpio_handler,		"set the gpio"},
	// {"AT+SYSGPIOREAD",			read_gpio_handler,		"read the gpio"},
    { "AT+LEDTEST",             "LED test cmd",                                 handle_led_test_cmd},
};


int at_register_cmds(void)
{
    int cmd_number = sizeof(at_cmd_list)/sizeof(struct cli_command);
    // printf("cmd number = %d\n",cmd_number);
    for(int i=0; i<cmd_number; i++){
        if(aos_cli_register_command( &at_cmd_list[i] ) != 0){
            printf("register error\n");
            return -1;
        }
    }
    
    return 0;
}