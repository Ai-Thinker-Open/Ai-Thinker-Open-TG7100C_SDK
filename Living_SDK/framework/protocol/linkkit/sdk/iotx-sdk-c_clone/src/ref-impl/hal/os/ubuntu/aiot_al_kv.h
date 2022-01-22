/**
 * @file kv_posix_flash.h
 * @brief 
 * @date 2020-07-08
 *
 * @copyright Copyright (C) 2017-2020 Alibaba Group Holding Limited
 *
 * @details
 *
 */

#ifndef __KV_POSIX_FLASH_H__
#define __KV_POSIX_FLASH_H__

#include <stdint.h>
#include <errno.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#define VIRTUAL_FLASH_ERASED_VALUE (0xFF)

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aiot_al_kv_flash_erase(uint32_t offset, uint32_t size);

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aiot_al_kv_flash_write(uint32_t *offset, const void *buf, uint32_t nbytes);

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aiot_al_kv_flash_read(uint32_t *offset, void *buf, uint32_t nbytes);

int aiot_al_is_lk_kv(void);
int aiot_al_lk_kv_get_value_len(void);
int aiot_al_lk_kv_set(const char *key, const void *val, int len);
int aiot_al_lk_kv_get(const char *key, void *val, int *buffer_len);
int aiot_al_lk_kv_del(const char *key);

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef __KV_POSIX_FLASH_H__ */
