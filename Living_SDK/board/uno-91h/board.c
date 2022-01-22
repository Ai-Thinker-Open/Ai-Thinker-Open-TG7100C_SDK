#include "hal/soc/soc.h"
//#include "serial_api.h"
#include <aos/kernel.h>
#include "rda59xx_daemon.h"

#ifndef HAL_PARTITION_META_DATA
#define HAL_PARTITION_META_DATA     HAL_PARTITION_CUSTOM_1
#endif
#ifndef HAL_PARTITION_SYS_DATA
#define HAL_PARTITION_SYS_DATA      HAL_PARTITION_2ND_BOOT
#endif

const hal_logic_partition_t hal_partitions[] =
{
#if 0
    [HAL_PARTITION_BOOTLOADER] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Bootloader",
        .partition_start_addr       = 0x18001000,
        .partition_length           = 0x3000,    //12k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif
    [HAL_PARTITION_APPLICATION] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Application",
        .partition_start_addr       = 0x18004000,
        .partition_length           = 0x91000,//580K bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "OTA Storage",
        .partition_start_addr       = 0x18095000,
        .partition_length           = 0x5D000,//372K bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x180F7000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x180F8000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x180FA000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x180FB000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_META_DATA] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Meta Data",
        .partition_start_addr       = 0x180FC000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_SYS_DATA] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "SYS RF Data",
        .partition_start_addr       = 0x180FE000,
        .partition_length           = 0x1000,
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};
extern uart_dev_t uart_0;
/*
uart_dev_t uart_0 = {
    0,
    {921600, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},
    NULL,
};
*/

int32_t rda_get_macaddr_from_flash(unsigned char *macaddr)
{
    int ret;
    if (NULL == macaddr) {
        printf("%s ERR: null pointer\n", __func__);
        return;
    }
    ret = rda5981_flash_read_mac_addr(macaddr);
    if ((ret!=0) || (is_zero_ether_addr(macaddr))) {
#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
        unsigned int out_len;
        ret = mbedtls_hardware_poll(NULL, macaddr, 6, &out_len);
        if (6 != out_len) {
            printf("%s out_len err:%d\n", __func__, out_len);
        }
#else
        ret = rda_get_random_bytes(macaddr, 6);
#endif
        macaddr[0] &= 0xfe;    /* clear multicast bit */
        macaddr[0] |= 0x02;    /* set local assignment bit (IEEE802) */
        /* write to flash */
        rda5981_flash_write_mac_addr(macaddr);
        printf("WARN: use randon mac\n");
    }
    return ret;
}

void board_init(void)
{
    hal_uart_init(&uart_0);
}

void board_init_later(void)
{
    printf("====board_init_later done===\r\n");
}

