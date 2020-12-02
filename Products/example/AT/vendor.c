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
#include "aos/kv.h"
#include "vendor.h"
#include "net_state_manger.h"
#include "iot_import_awss.h"
#include "at_main.h"
#include "vendor.h"
#include <hal/soc/gpio.h>
#if defined(HF_LPT230) || defined(HF_LPT130)
#include "hfilop/hfilop.h"
#endif

#if defined (TG7100CEVB)
char *p_product_key = NULL;
char *p_product_secret = NULL;
char *p_device_name = NULL;
char *p_device_secret = NULL;
char *productidStr = NULL;
#endif

#define KEY_GPIO        8
#define R_GPIO          17
#define G_GPIO          20
#define B_GPIO          14
#define C_GPIO          5
#define W_GPIO          22


static gpio_dev_t io_r_led;
static gpio_dev_t io_g_led;
static gpio_dev_t io_b_led;
static gpio_dev_t io_c_led;
static gpio_dev_t io_w_led;
static gpio_dev_t io_key;


static aos_timer_t led_test_timer;

int led_count = 0;

static void open_led_task(void *parm)
{
    if(led_count == 0){
        led_count = 1;
        hal_gpio_output_high(&io_r_led);
        hal_gpio_output_low(&io_g_led);
        hal_gpio_output_low(&io_b_led);
        hal_gpio_output_low(&io_c_led);
        hal_gpio_output_low(&io_w_led);
        aos_msleep(1000);
    }
    if(led_count == 1){
        led_count = 2;
        hal_gpio_output_high(&io_g_led);
        hal_gpio_output_low(&io_r_led);
        hal_gpio_output_low(&io_b_led);
        hal_gpio_output_low(&io_c_led);
        hal_gpio_output_low(&io_w_led);
        aos_msleep(1000);
    }
    if(led_count = 2){
        led_count = 3;
        hal_gpio_output_high(&io_b_led);
        hal_gpio_output_low(&io_g_led);
        hal_gpio_output_low(&io_r_led);
        hal_gpio_output_low(&io_c_led);
        hal_gpio_output_low(&io_w_led);
        aos_msleep(1000);
    }
    if(led_count == 3)
    {
        led_count = 4;
        hal_gpio_output_high(&io_c_led);
        hal_gpio_output_low(&io_g_led);
        hal_gpio_output_low(&io_b_led);
        hal_gpio_output_low(&io_r_led);
        hal_gpio_output_low(&io_w_led);
        aos_msleep(1000);
    }
    if(led_count == 4)
    {
        led_count = 0;
        hal_gpio_output_high(&io_w_led);
        hal_gpio_output_low(&io_g_led);
        hal_gpio_output_low(&io_b_led);
        hal_gpio_output_low(&io_c_led);
        hal_gpio_output_low(&io_r_led);
        aos_msleep(1000);
    }
}

static int led_state = 2;
void product_set_led(bool state)
{
    if (led_state == (int)state) {
        return;
    }
    if (state) {
        aos_timer_new_ext(&led_test_timer, open_led_task, NULL, 1000, 1, 1);
    } else if(state == OFF){
        aos_timer_stop(&led_test_timer);
        aos_timer_free(&led_test_timer);
        hal_gpio_output_low(&io_r_led);
        hal_gpio_output_low(&io_g_led);
        hal_gpio_output_low(&io_b_led);
        hal_gpio_output_low(&io_c_led);
        hal_gpio_output_low(&io_w_led);
    }
    led_state = (int)state;
}
int product_get_led_statue(void)
{
    return led_state;
}

void vendor_product_init(void)
{
    io_r_led.port = R_GPIO;
    io_r_led.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&io_r_led);

    io_g_led.port = G_GPIO;
    io_g_led.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&io_g_led);

    io_b_led.port = B_GPIO;
    io_b_led.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&io_b_led);

    io_c_led.port = C_GPIO;
    io_c_led.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&io_c_led);

    io_w_led.port = W_GPIO;
    io_w_led.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&io_w_led);

    io_key.port = KEY_GPIO;
    io_key.config = INPUT_PULL_UP;
    hal_gpio_init(&io_key);
    product_set_led(OFF);
}



int vendor_get_product_key(char *product_key, int *len)
{
    char *pk = NULL;
    int ret = -1;
    int pk_len = *len;

    ret = aos_kv_get(KV_KEY_PK, product_key, &pk_len);
#if defined(HF_LPT230) || defined(HF_LPT130)
    if ((ret != 0)&&((pk = hfilop_layer_get_product_key()) != NULL)) {
        pk_len = strlen(pk);
        memcpy(product_key, pk, pk_len);
        ret = 0;
    }
#else
    /*
        todo: get pk...
    */
#endif
    if (ret == 0) {
        *len = pk_len;
    }
#if (defined (TG7100CEVB))
    if(p_product_key != NULL && strlen(p_product_key) > 0){
        pk_len = strlen(p_product_key);
        memcpy(product_key, p_product_key, pk_len);
        *len = pk_len;
        ret = 0;
    }
#endif
    return ret;
}

int vendor_get_product_secret(char *product_secret, int *len)
{
    char *ps = NULL;
    int ret = -1;
    int ps_len = *len;

    ret = aos_kv_get(KV_KEY_PS, product_secret, &ps_len);
#if defined(HF_LPT230) || defined(HF_LPT130)
    if ((ret != 0)&&((ps = hfilop_layer_get_product_secret()) != NULL)) {
        ps_len = strlen(ps);
        memcpy(product_secret, ps, ps_len);
        ret = 0;
    }
#else
    /*
        todo: get ps...
    */
#endif
    if (ret == 0) {
        *len = ps_len;
    }
#if (defined (TG7100CEVB))
    if(p_product_secret != NULL && strlen(p_product_secret) > 0){
        ps_len = strlen(p_product_secret);
        memcpy(product_secret, p_product_secret, ps_len);
        *len = ps_len;
        ret = 0;
    }
#endif
    // LOG("poduct secret: %s\n",product_secret);
    return ret;
}

int vendor_get_device_name(char *device_name, int *len)
{
    char *dn = NULL;
    int ret = -1;
    int dn_len = *len;

    ret = aos_kv_get(KV_KEY_DN, device_name, &dn_len);
#if defined(HF_LPT230) || defined(HF_LPT130)
    if ((ret != 0)&&((dn = hfilop_layer_get_device_name()) != NULL)) {
        dn_len = strlen(dn);
        memcpy(device_name, dn, dn_len);
        ret = 0;
    }
#else
    /*
        todo: get dn...
    */
#endif
    if (ret == 0) {
        *len = dn_len;
    }
#if (defined (TG7100CEVB))
    if(p_device_name != NULL && strlen(p_device_name) > 0){
        dn_len = strlen(p_device_name);
        memcpy(device_name, p_device_name, dn_len);
        *len = dn_len;
        ret = 0;
    }
#endif
    // LOG("device name: %s\n",device_name);
    return ret;
}

int vendor_get_device_secret(char *device_secret, int *len)
{
    char *ds = NULL;
    int ret = -1;
    int ds_len = *len;

    ret = aos_kv_get(KV_KEY_DS, device_secret, &ds_len);

#if defined(HF_LPT230) || defined(HF_LPT130)
    if ((ret != 0)&&((ds = hfilop_layer_get_device_secret()) != NULL)) {
        ds_len = strlen(ds);
        memcpy(device_secret, ds, ds_len);
        ret = 0;
    }
#else
    /*
        todo: get ds...
    */
#endif
    if (ret == 0) {
        *len = ds_len;
    }
#if (defined (TG7100CEVB))
    if(p_device_secret != NULL && strlen(p_device_secret) > 0){
        ds_len = strlen(p_device_secret);
        memcpy(device_secret, p_device_secret, ds_len);
        *len = ds_len;
        ret = 0;
    }
#endif
    return ret;
}

int vendor_get_product_id(uint32_t *pid)
{
    int ret = -1;
    char pidStr[9] = { 0 };
    int len = sizeof(pidStr);
    ret = aos_kv_get(KV_KEY_PD, pidStr, &len);
    if (ret == 0 && len < sizeof(pidStr)) {
        *pid = atoi(pidStr);
    } else {
        ret = -1;
    }
#if (defined (TG7100CEVB))
    if(productidStr != NULL && strlen(productidStr) > 6){
        *pid = atoi(productidStr);
        // LOG("pid[%d]", *pid);
        return 0;
    }
#endif
    return ret;
}

int set_device_meta_info(void)
{
    int len = 0;
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char product_secret[PRODUCT_SECRET_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};

    memset(product_key, 0, sizeof(product_key));
    memset(product_secret, 0, sizeof(product_secret));
    memset(device_name, 0, sizeof(device_name));
    memset(device_secret, 0, sizeof(device_secret));

    len = PRODUCT_KEY_LEN + 1;
    vendor_get_product_key(product_key, &len);

    len = PRODUCT_SECRET_LEN + 1;
    vendor_get_product_secret(product_secret, &len);

    len = DEVICE_NAME_LEN + 1;
    vendor_get_device_name(device_name, &len);

    len = DEVICE_SECRET_LEN + 1;
    vendor_get_device_secret(device_secret, &len);

    if ((strlen(product_key) > 0) && (strlen(product_secret) > 0) \
            && (strlen(device_name) > 0) && (strlen(device_secret) > 0)) {
        HAL_SetProductKey(product_key);
        HAL_SetProductSecret(product_secret);
        HAL_SetDeviceName(device_name);
        HAL_SetDeviceSecret(device_secret);
        LOG("pk[%s]", product_key);
        LOG("dn[%s]", device_name);
        return 0;
    } else {
#if (defined (TG7100CEVB))
        /* check media valid, and update p */
        int res = ali_factory_media_get(
                    &p_product_key,
                    &p_product_secret,
                    &p_device_name,
                    &p_device_secret,
                    &productidStr);
        if (0 != res) {
            printf("ali_factory_media_get res = %d\r\n", res);
            return -1;
        } else {
            HAL_SetProductKey(p_product_key);
            HAL_SetProductSecret(p_product_secret);
            HAL_SetDeviceName(p_device_name);
            HAL_SetDeviceSecret(p_device_secret);
            LOG("pk[%s]", p_product_key);
            LOG("dn[%s]", p_device_name);
            // LOG("pid[%s]", productidStr);
            return 0;
        }
#endif
        printf("no valid device meta data");
        return -1;
    }
}


bool product_get_key(void)
{
    uint32_t level;
    hal_gpio_input_get(&io_key, &level);
    return level;
}





#define AWSS_REBOOT_TIMEOUT     (4 * 1000)
#define AWSS_RESET_TIMEOUT      (6 * 1000)
#define KEY_PRESSED_VALID_TIME  100
#define KEY_DETECT_INTERVAL     50
#define AWSS_REBOOT_CNT         AWSS_REBOOT_TIMEOUT / KEY_DETECT_INTERVAL   //80
#define AWSS_RESET_CNT          AWSS_RESET_TIMEOUT / KEY_DETECT_INTERVAL    //120
#define KEY_PRESSED_CNT         KEY_PRESSED_VALID_TIME / KEY_DETECT_INTERVAL//2
void key_detect_event_task(void *arg)
{
    int nCount = 0, awss_mode = 0;
    int timeout = (AWSS_REBOOT_CNT < AWSS_RESET_TIMEOUT)? AWSS_REBOOT_CNT : AWSS_RESET_TIMEOUT;
    printf("############################\r\n");
    while (1) {
        if (!product_get_key()) {
            nCount++;
        } else {
            if (nCount >= KEY_PRESSED_CNT && nCount < timeout) {
                uart_push_data("boot\r\n");
            }
            nCount = 0;
        }
        aos_msleep(KEY_DETECT_INTERVAL);
    }
    aos_task_exit(0);
}
