/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __GENIE_SAL_OTA_H__
#define __GENIE_SAL_OTA_H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

enum
{
    GENIE_STORAGE_INDEX_START = 0,
    GFI_MESH_PARA,
    GFI_MESH_TRITUPLE,
    GFI_MESH_SADDR,
    GFI_MESH_SUB,
    GFI_MESH_RESET_CNT,
    GFI_MESH_POWERUP,
    GFI_MESH_HB,
    GFI_MESH_DEVKEY,
    GFI_MESH_NETKEY,
    GFI_MESH_APPKEY,
    GFI_OTA_INDICAT,
    GFI_OTA_IMAGE_ID,
    GFI_USERDATA_START,
    GFI_MESH_GROUP_ADDR,
#ifdef CONFIG_BT_MESH_CTRL_RELAY
    GFI_MESH_CTRL_RELAY,
#endif
    GFI_MESH_APPKEY1,
};

typedef enum
{
    GENIE_STORAGE_SUCCESS = 0,
    GENIE_STORAGE_INIT_FAIL,
    GENIE_STORAGE_MALLOC_FAIL,
    GENIE_STORAGE_EARSE_FAIL,
    GENIE_STORAGE_DATA_INVALID,
    GENIE_STORAGE_ERASE_FAIL,
    GENIE_STORAGE_READ_FAIL,
    GENIE_STORAGE_WRITE_FAIL,
    GENIE_STORAGE_DELETE_FAIL,
    GENIE_STORAGE_INVALID_INDEX,
    GENIE_STORAGE_SIZE_ERROR,
    GENIE_STORAGE_CHECK_CRC_FAIL
} genie_storage_status_e;

typedef struct genie_sal_ota_ctx_s
{
    uint8_t image_type;
    uint32_t image_size;
    uint16_t image_crc16;
    uint8_t ota_ready;
} genie_sal_ota_ctx_t;

/**
 * @brief finish ota and reboot the device.
 */
void genie_sal_ota_reboot(void);

/**
 * @brief check the dfu image.
 * @param[in] the image type.
 * @param[out] the crc of image.
 * @return the result of checksum.
 */
unsigned char genie_sal_ota_check_checksum(short image_id, unsigned short *crc16_output);

/**
 * @brief write dfu data.
 * @param[in] the image type.
 * @param[in] the offset of flash.
 * @param[in] the length of data.
 * @param[in] the writting data.
 * @return the current runing partition.
 */
int genie_sal_ota_update_image(short signature, int offset, int length, int *p_void);

uint32_t genie_sal_ota_get_max_partition_size(void);

#ifdef CONFIG_GENIE_OTA_PINGPONG
/**
 * @brief get the current runing partition.
 * @return the current runing partition.
 */
uint8_t genie_sal_ota_get_program_image(void);

/**
 * @brief switch the running partition, without reboot.
 * @param[in] the partition which switch to.
 * @return the runing partition when next boot.
 */
uint8_t genie_sal_ota_change_image_id(uint8_t target_id);
#endif

int genie_sal_ota_erase(void);

/*This api mainly for light product,if light is on we shouldn't reboot by ota*/
bool genie_sal_ota_is_allow_reboot(void);

int32_t genie_sal_ota_update_boot_params(genie_sal_ota_ctx_t sal_ota_ctx);

#endif
