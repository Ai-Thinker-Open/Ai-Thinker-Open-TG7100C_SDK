#include "bzopt.h"
#include "genie_sal_ota.h"
#include "hal/soc/flash.h"
#include "iot_import.h"
#include <utils_sha256.h>

#ifdef TG7100CEVB
#include <hal_boot2.h>
#endif

#define OTA_IMAGE_RESERVE_SIZE (0)

static unsigned short image_crc16 = 0;

void genie_sal_ota_reboot(void)
{
    HAL_Reboot();
}

int genie_sal_ota_erase(void)
{
    int ret;
    hal_logic_partition_t *partition_info = NULL;
    uint32_t offset = OTA_IMAGE_RESERVE_SIZE;
    uint32_t length = 0;
    uint8_t cmp_buf[32] = {0xFF};
    uint8_t wr_buf[32] = {0};

    ///get OTA temporary partition information
    partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);

    if (partition_info == NULL)
    {
        BREEZE_ERR("ota read partition fail");
        return -1;
    }
    length = partition_info->partition_length - offset;

    memset(cmp_buf, 0xFF, sizeof(cmp_buf));
    ret = hal_flash_read(HAL_PARTITION_OTA_TEMP, &offset, (void *)wr_buf, sizeof(wr_buf));
    if (ret < 0)
    {
        BREEZE_ERR("ota read flash error");
        return -1;
    }
    if (memcmp(wr_buf, cmp_buf, sizeof(wr_buf)) == 0)
    {
        return 0;
    }

    BREEZE_INFO("OTA dirty\r\n");

    offset = OTA_IMAGE_RESERVE_SIZE;

    /* For bootloader upgrade, we will reserve two sectors, then save the image */
    ret = hal_flash_erase(HAL_PARTITION_OTA_TEMP, offset, length);
    if (ret < 0)
    {
        BREEZE_ERR("ota erase flash error\r\n");
        return -1;
    }

    return 0;
}

uint32_t genie_sal_ota_get_max_partition_size(void)
{
    hal_logic_partition_t *partition_info = NULL;

    partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);
    if (partition_info == NULL)
    {
        BREEZE_ERR("ota read partition fail");
        return 0;
    }

    if (partition_info->partition_length > OTA_IMAGE_RESERVE_SIZE)
    {
        return partition_info->partition_length - OTA_IMAGE_RESERVE_SIZE;
    }

    return 0;
}

uint16_t util_crc16_ccitt(uint8_t const *p_data, uint32_t size, uint16_t const *p_crc)
{
    uint16_t crc = (p_crc == NULL) ? 0xFFFF : *p_crc;
    uint8_t b = 0;

    if (p_crc == NULL)
    {
        b = 0;
    }

    for (uint32_t i = 0; i < size; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            b = ((p_data[i] << j) & 0x80) ^ ((crc & 0x8000) >> 8);
            crc <<= 1;
            if (b != 0)
            {
                crc ^= 0x1021;
            }
        }
    }

    return crc;
}

int genie_sal_ota_update_image(short signature, int offset, int length, int *buf)
{
    int ret = 0;
    uint32_t wr_idx = offset;
    uint8_t *wr_buf = (uint8_t *)buf;
    hal_logic_partition_t *partition_info = NULL;

    if (offset == 0)
    {
        image_crc16 = util_crc16_ccitt(wr_buf, length, NULL);
    }
    else
    {
        image_crc16 = util_crc16_ccitt(wr_buf, length, &image_crc16);
    }

    partition_info = hal_flash_get_info(HAL_PARTITION_OTA_TEMP);

    if (partition_info == NULL ||
        partition_info->partition_length < (offset + length))
    {
        BREEZE_ERR("OTA update fail");
        return -1;
    }

    wr_idx += OTA_IMAGE_RESERVE_SIZE;
    ret = hal_flash_write(HAL_PARTITION_OTA_TEMP, &wr_idx, (void *)wr_buf, length);
    if (ret < 0)
    {
        BREEZE_ERR("ota write flash error");
        return -1;
    }

    return 0;
}

unsigned char genie_sal_ota_check_checksum(short image_id, unsigned short *crc16_output)
{
    *crc16_output = image_crc16;
    return 1;
}

int32_t genie_sal_ota_update_boot_params(genie_sal_ota_ctx_t sal_ota_ctx)
{
    if (sal_ota_ctx.ota_ready == 1)
    {
#ifdef TG7100CEVB
        uint8_t activeID;
        HALPartition_Entry_Config ptEntry;

        // int offset = 0;
        // char *r_buf;

        activeID = hal_boot2_get_active_partition();

        printf("tg7100c ota active id:%d\r\n", activeID);

        if (hal_boot2_get_active_entries(BOOT2_PARTITION_TYPE_FW, &ptEntry))
        {
            BREEZE_ERR("tg7100c ota get active entries fail");
            return -1;
        }

        ptEntry.len = sal_ota_ctx.image_size - 32;

        #define CHECK_IMG_BUF_SIZE   512
        uint8_t sha_check[32] = {0};
        uint8_t dst_sha[32] = {0};
        uint32_t bin_size, read_size;
        iot_sha256_context sha256_ctx;
        int offset = 0;
        char *r_buf = NULL;

        bin_size = sal_ota_ctx.image_size - 32;
        r_buf = HAL_Malloc(CHECK_IMG_BUF_SIZE);
        if (r_buf == NULL) {
            goto ERR;
        }
        utils_sha256_init(&sha256_ctx);
        utils_sha256_starts(&sha256_ctx);

        offset = 0;
        while (offset < bin_size) {
            (bin_size - offset >= CHECK_IMG_BUF_SIZE) ? (read_size = CHECK_IMG_BUF_SIZE):(read_size = bin_size - offset);
            if (hal_flash_read(HAL_PARTITION_OTA_TEMP, &offset, r_buf, read_size) < 0) {
                HAL_Free(r_buf);
                goto ERR;
            }
            utils_sha256_update(&sha256_ctx, (const uint8_t *)r_buf, read_size);
        }

        utils_sha256_finish(&sha256_ctx, sha_check);
        utils_sha256_free(&sha256_ctx);
        HAL_Free(r_buf);

        hal_flash_read(HAL_PARTITION_OTA_TEMP, &offset, (char *)dst_sha, 32);

        if (memcmp(sha_check, (const void *)dst_sha, 32) != 0) {
            printf("upgrade over err:sha256\r\n");
            goto ERR;
        }

        printf("tg7100c ota image size:%lu\r\n", ptEntry.len);
        hal_boot2_update_ptable(&ptEntry);
#endif
    }

    return 0;

ERR:
    BREEZE_ERR("ble ota error");
    return -2;
}
