/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef __VENDOR_H__
#define __VENDOR_H__

#include <aos/aos.h>

typedef enum{
    OFF = 0,
    ON
}LEDState;



#define POWER_OFF   0
#define POWER_ON    1
#define LAST_STATE  2

#define KV_KEY_SWITCH_STATE "OUTLET_SWITCH_STATE"

#define REBOOT_STATE LAST_STATE



#define KV_KEY_PK "product_key"
#define KV_KEY_PS "product_secret"
#define KV_KEY_DN "device_name"
#define KV_KEY_DS "device_secret"
#define KV_KEY_PD "product_id"

#define MAX_KEY_LEN (6)


/**
 * @brief set device meta info.
 *
 * @param [in] None.
 * @param [in] None.
 *
 * @return 0:success, -1:failed.
 * @see None.
 */
int set_device_meta_info(void);

int vendor_get_product_key(char *product_key, int *len);
int vendor_get_product_secret(char *product_secret, int *len);
int vendor_get_device_name(char *device_name, int *len);
int vendor_get_device_secret(char *device_secret, int *len);
int vendor_get_product_id(uint32_t *pid);

void vendor_product_init(void);
void key_detect_event_task(void *arg);
void product_set_led(bool state);

#endif
