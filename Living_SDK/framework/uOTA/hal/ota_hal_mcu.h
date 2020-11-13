/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef MCU_HAL_OTA_H
#define MCU_HAL_OTA_H

#ifdef __cplusplus
extern "C" {
#endif

int ota_mcu_init(void *something);
int ota_mcu_write(int* off, char* in_buf ,int in_buf_len);
int ota_mcu_boot(void *something);


#ifdef __cplusplus
}
#endif
#endif /* MCU_HAL_OTA_H */

