#include "hal/soc/soc.h"
#include <aos/kernel.h>
#include "soc_defs.h"
#include <stdio.h>

/* Logic partition on flash devices */
hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX];

void board_init(void)
{
    uint32_t main_partition_size = REG32(MAGIC_HEADER_BASE+OFFSET_PARTITION_MAIN_SIZE);
    uint32_t flash_total_size = REG32(MAGIC_HEADER_BASE+OFFSET_FLASH_TOTAL_SIZE);
    uint32_t up_partition_start_addr = 0;
    uint32_t up_partition_length = 0;
    /////HAL_PARTITION_APPLICATION
    hal_partitions[HAL_PARTITION_APPLICATION].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_description      = "Application";
    hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr       = (up_partition_start_addr = 0x0);
    hal_partitions[HAL_PARTITION_APPLICATION].partition_length           = (up_partition_length = main_partition_size);//650
    hal_partitions[HAL_PARTITION_APPLICATION].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
    printf("HAL_PARTITION_APPLICATION start 0x%lx, len %lu\n",
    hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr,
    hal_partitions[HAL_PARTITION_APPLICATION].partition_length);
    
    /////HAL_PARTITION_OTA_TEMP
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_owner               = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_description         = "OTA Storage";
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_start_addr          = (up_partition_start_addr = (up_partition_start_addr+up_partition_length));//0+650k
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_length              = (up_partition_length = main_partition_size-0x1000);//=650k-4k=646k
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_options             = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
    printf("HAL_PARTITION_OTA_TEMP start 0x%lx, len %lu\n",
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_start_addr,
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_length);

    /////HAL_PARTITION_PARAMETER_1
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_owner               = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_description         = "PARAMETER1";
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr          = (up_partition_start_addr = (up_partition_start_addr+up_partition_length));//0+650k+646k
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length              = (up_partition_length = 0x1000);//4k
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_options             = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
    printf("HAL_PARTITION_PARAMETER_1 start 0x%lx, len %lu\n",
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr,
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length);

    /////HAL_PARTITION_PARAMETER_2
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_owner               = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_description         = "PARAMETER2";
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_start_addr          = (up_partition_start_addr = (up_partition_start_addr+up_partition_length));//0+650k+646k+4k
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_length              = (up_partition_length = 0x2000);
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_options             = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
    printf("HAL_PARTITION_PARAMETER_2 start 0x%lx, len %lu\n",
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_start_addr,
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_length);

    #ifdef DEV_OFFLINE_LOG_ENABLE
    /////HAL_PARTITION_PARAMETER_3
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_owner               = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_description         = "PARAMETER3";
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_start_addr          = (up_partition_start_addr = (up_partition_start_addr+up_partition_length));//0+650k+646k+4k+4k
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_length              = (up_partition_length = 0x1000); //4k
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_options             = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
    printf("HAL_PARTITION_PARAMETER_3 start 0x%lx, len %lu\n",
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_start_addr,
    hal_partitions[HAL_PARTITION_PARAMETER_3].partition_length);
    #endif
}
