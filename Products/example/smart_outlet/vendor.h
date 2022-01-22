/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef __VENDOR_H__
#define __VENDOR_H__

#include <aos/aos.h>

typedef enum {
    OFF = 0,
    ON
} eSwitchState;

#define POWER_OFF   0
#define POWER_ON    1
#define LAST_STATE  2

#define KV_KEY_SWITCH_STATE "OUTLET_SWITCH_STATE"

#define REBOOT_STATE LAST_STATE
/**
 * @brief product init.
 *
 * @param [in] None.
 *
 * @return None.
 * @see None.
 */
void vendor_product_init(void);
void product_set_switch(bool state);
bool product_get_switch(void);
void product_toggle_led(void);
void product_set_led(bool state);
bool product_get_key(void);

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

/**
 * @brief vendor operation after receiving device_bind event from Cloud.
 *
 * @param [in] None.
 *
 * @return None.
 * @see None.
 */
void vendor_device_bind(void);

/**
 * @brief vendor operation after receiving device_unbind event from Cloud.
 *
 * @param [in] None.
 *
 * @return None.
 * @see None.
 */
void vendor_device_unbind(void);
/**
 * @brief vendor operation after receiving device_reset event from Cloud.
 *
 * @param [in] None.
 *
 * @return None.
 * @see None.
 */
void vendor_device_reset(void);

int vendor_get_product_key(char *product_key, int *len);
int vendor_get_product_secret(char *product_secret, int *len);
int vendor_get_device_name(char *device_name, int *len);
int vendor_get_device_secret(char *device_secret, int *len);
int vendor_get_product_id(uint32_t *pid);

#endif
