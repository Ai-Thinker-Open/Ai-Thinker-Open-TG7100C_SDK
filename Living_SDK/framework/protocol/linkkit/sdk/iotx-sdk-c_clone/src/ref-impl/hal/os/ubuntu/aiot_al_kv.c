/**
 * @file kv_posix_flash.c
 * @brief this file aim to simulate flash use posic file
 * @date 2020-07-08
 * 
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

#include "aiot_al_kv.h"

#define AIOT_KV_FILE_NAME "aiot_kv.bin"

#define kv_err(...)          \
    do                       \
    {                        \
        printf(__VA_ARGS__); \
        printf("\r\n");      \
    } while (0)

static int create_kv_file(char *file)
{
    int fd;

    fd = open(file, O_CREAT | O_RDWR, 0644);
    if (fd < 0)
    {
        return -1;
    }

    if (fsync(fd) < 0)
    {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static int kv_posix_flash_init(void)
{
    if (access(AIOT_KV_FILE_NAME, F_OK) < 0)
    {
        /* create KV file when not exist */
        if (create_kv_file(AIOT_KV_FILE_NAME) < 0)
        {
            return -1;
        }
    }

    return 0;
}

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aiot_al_kv_flash_erase(uint32_t off_set, uint32_t size)
{
    int ret;
    int fd = -1;

    kv_posix_flash_init();
    fd = open(AIOT_KV_FILE_NAME, O_WRONLY);
    if (fd < 0)
    {
        return -1;
    }

    ret = lseek(fd, off_set, SEEK_SET);
    if (ret < 0)
    {
        kv_err("lseek err");
        close(fd);
        return -1;
    }

    while (size--)
    {
        char data = VIRTUAL_FLASH_ERASED_VALUE;
        if (write(fd, &data, 1) != 1)
        {
            kv_err("kv erase failed");
            close(fd);
            return -1;
        }
    }

    fsync(fd);
    close(fd);

    return 0;
}

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aiot_al_kv_flash_write(uint32_t *off_set, const void *in_buf, uint32_t in_buf_len)
{
    struct stat st;
    int ret;
    int fd = -1;

    kv_posix_flash_init();
    fd = open(AIOT_KV_FILE_NAME, O_WRONLY);
    if (fd < 0)
    {
        return -1;
    }

    if (fstat(fd, &st) < 0)
    {
        kv_err("fstat err");
        close(fd);
        return -1;
    }

    if (st.st_size < *off_set + in_buf_len)
    {
        kv_err("overstep st.st_size = %d location =%d cur loc=%d",
               (int)st.st_size,
               (int)*off_set,
               (int)(*off_set + in_buf_len));
        close(fd);
        return -1;
    }

    ret = lseek(fd, *off_set, SEEK_SET);
    if (ret < 0)
    {
        kv_err("lseek err");
        close(fd);
        return -1;
    }

    if (write(fd, in_buf, in_buf_len) != in_buf_len)
    {
        kv_err("kv write failed");
        close(fd);
        return -1;
    }

    fsync(fd);
    close(fd);

    return 0;
}

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aiot_al_kv_flash_read(uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    struct stat st;
    int ret = 0;
    int offset;
    int fd = -1;

    if (out_buf == NULL || out_buf_len == 0 || off_set == NULL)
    {
        kv_err("paras err");
        return -1;
    }

    kv_posix_flash_init();
    offset = *off_set;
    fd = open(AIOT_KV_FILE_NAME, O_RDONLY);

    if (fd < 0)
    {
        kv_err("open err");
        return -1;
    }

    if (fstat(fd, &st) < 0)
    {
        kv_err("fstat err");
        close(fd);
        return -1;
    }

    if (st.st_size < offset + out_buf_len)
    {
        kv_err("aiot kv read overstep");
        close(fd);
        return -1;
    }

    ret = lseek(fd, offset, SEEK_SET);
    if (ret < 0)
    {
        kv_err("lseek err");
        close(fd);
        return -1;
    }

    if (read(fd, out_buf, out_buf_len) != out_buf_len)
    {
        kv_err("read err");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
