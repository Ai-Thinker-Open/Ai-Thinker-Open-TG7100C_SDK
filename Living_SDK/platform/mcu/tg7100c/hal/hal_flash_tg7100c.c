#include <string.h>
#include "bl_flash.h"
#include "hal/soc/soc.h"
#include "aos/kernel.h"
#include "board.h"
#include <bl_mtd.h>
#include <bl_boot2.h>
#include "hal/soc/flash.h"
#include "utils_log.h"

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition);

static bl_mtd_handle_t gp_handle[] = {
    [HAL_PARTITION_BOOTLOADER]  = NULL,     // Bootloader
    [HAL_PARTITION_PARAMETER_1] = NULL,   // PARAMETER1
    [HAL_PARTITION_PARAMETER_2] = NULL,   // PARAMETER2
    [HAL_PARTITION_APPLICATION] = NULL,       // Application
    [HAL_PARTITION_OTA_TEMP]    = NULL,      // OTA
    [HAL_PARTITION_PARAMETER_3] = NULL,   // PARAMETER3
    [HAL_PARTITION_PARAMETER_4] = NULL,   // PARAMETER4
};

static const char *g_hal_partitions[] = {
    [HAL_PARTITION_BOOTLOADER]  = "Boot",     // Bootloader
    [HAL_PARTITION_PARAMETER_1] = "PARAM1",   // PARAMETER1
    [HAL_PARTITION_PARAMETER_2] = "PARAM2",   // PARAMETER2
    [HAL_PARTITION_APPLICATION] = "FW",       // Application
    [HAL_PARTITION_OTA_TEMP]    = "OTA",      // OTA
    [HAL_PARTITION_PARAMETER_3] = "PARAM3",   // PARAMETER3
    [HAL_PARTITION_PARAMETER_4] = "PARAM4",   // PARAMETER4
};

/* Logic partition on flash devices for 2M bytes 1024*1024 */
static const hal_logic_partition_t g_tg7100c_hal_partitions_2M[] =
{
	[HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAM1",
        .partition_start_addr       = 0x1FB000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAM2",
        .partition_start_addr       = 0x1FC000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "FW",
	    .partition_start_addr       = 0x10000,
	    .partition_length           = 0x111000, 
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA",
        .partition_start_addr      = 0x121000,
        .partition_length          = 0xA0000, 
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAM3",
        .partition_start_addr       = 0x1FE000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAM4",
        .partition_start_addr       = 0x1FF000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

static int __flash_offset(hal_partition_t in_partition, uint32_t *off_set, uint32_t size)
{
    hal_logic_partition_t partition;
    int ret;
    
    if (!off_set) {
        return -1;
    }

    if ((ret = hal_flash_info_get(in_partition, &partition) != 0)) {
        return ret;
    }

    if (partition.partition_length < *off_set + size) {
        return -1;
    }
    return 0;
}

int hal_flash_init(hal_partition_t part)
{
    hal_partition_t handle_part = part;

    int flag = BL_MTD_OPEN_FLAG_NONE;
    if (part > HAL_PARTITION_PARAMETER_4) {
        return -1;
    }

    if (part == HAL_PARTITION_OTA_TEMP) {
        part = HAL_PARTITION_APPLICATION;
        flag = BL_MTD_OPEN_FLAG_BACKUP;
    }

    return bl_mtd_open(g_hal_partitions[part], &(gp_handle[handle_part]), flag);
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    int ret = __flash_offset(in_partition, &off_set, size);
    if (ret != 0) {
        return ret;
    }
    if (NULL == gp_handle[in_partition]) {
        hal_flash_init(in_partition);
    }
    return bl_mtd_erase(gp_handle[in_partition], off_set, size);
}

int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf, uint32_t in_buf_len)
{
    int ret = __flash_offset(in_partition, off_set, in_buf_len);
    if (ret != 0) {
        return ret;
    }

    ret = hal_flash_erase(in_partition, *off_set, in_buf_len);
    if (ret != 0) {
        return ret;
    }
    return hal_flash_write(in_partition, off_set, in_buf, in_buf_len);
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len)
{
    uint32_t addr = *off_set;

    static uint32_t head_flag = 512;
    char *wbuf = (char *)in_buf;
    uint32_t wlen = in_buf_len;

    uint32_t unused_len = 0;
 
    int ret = __flash_offset(in_partition, off_set, in_buf_len);
    if (ret != 0) {
        return ret;
    }
    if (in_partition > HAL_PARTITION_PARAMETER_4) {
        printf("%s arg err.", __func__);
        return -1;
    }
    if (in_buf == NULL) {
        return -1;
    }

    if (NULL == gp_handle[in_partition]) {
        hal_flash_init(in_partition);
    }

    if (off_set) {
        printf("part = %s, *off_set = %ld, in_buf = %p, in_buf_len = %ld\r\n",
                g_hal_partitions[in_partition], *off_set, (char *)in_buf, in_buf_len);
    } else {
        printf("part = %s, off_set = NULL, in_buf = %p, in_buf_len = %ld\r\n",
                g_hal_partitions[in_partition], (char *)in_buf, in_buf_len);
    }

    bl_mtd_write(gp_handle[in_partition], addr, wlen, wbuf);

    if (off_set) {
        *off_set += in_buf_len;
    }

    printf("%s end\r\n", __func__);

    return ret;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len)
{
    uint32_t addr = *off_set;
 
    int ret = __flash_offset(in_partition, off_set, in_buf_len);
    if (ret != 0) {
        return ret;
    }
    if (NULL == gp_handle[in_partition]) {
        hal_flash_init(in_partition);
    }
    if ((ret = bl_mtd_read(gp_handle[in_partition], addr, in_buf_len, out_buf)) == 0) {
        *off_set += in_buf_len;
    }
    return ret;
}

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    if (partition == NULL) {
        return -1;
    }
    if ((int)in_partition > HAL_PARTITION_PARAMETER_4) {
        return NULL;
    }
    *partition = (hal_logic_partition_t)g_tg7100c_hal_partitions_2M[in_partition];
    return 0;
}

hal_logic_partition_t *hal_flash_get_info(hal_partition_t in_partition)
{
    if ((int)in_partition > HAL_PARTITION_PARAMETER_4) {
        return NULL;
    }
    return (hal_logic_partition_t *)&g_tg7100c_hal_partitions_2M[in_partition];
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}


