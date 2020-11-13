#include "hal/soc/soc.h"
//#include "serial_api.h"
#include <aos/kernel.h>
#include "hfilop/hfilop.h"
#include "rda59xx_daemon.h"
#include "rda59xx_wifi_include.h"
#include "ota_hal_plat.h"
/* Logic partition on flash devices */
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
#if 1
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "OTA Storage",
        .partition_start_addr       = 0x18095000,
        .partition_length           = 0x5D000,//372K bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif
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
    [HAL_PARTITION_SYS_DATA] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "SYS RF Data",
        .partition_start_addr       = 0x180FC000,
        .partition_length           = 0x1000,
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_HFILOP] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "HFILOP",
        .partition_start_addr       = 0x180FD000,
        .partition_length           = 0x2000, //8k bytes
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

typedef int (*wifi_scan_callback_t)(ap_list_adv_t *);
static int transform_rssi(int rssi_dbm)
{
    int ret;
    ret = (rssi_dbm+95)*2;

    if (ret < 70)
        ret = ret -(15 - ret/5);

    if(ret < 0)
        ret = 0;
    else if(ret >100)
        ret = 100;

    return ret;
}

int hfilop_wifi_scan(wifi_scan_callback_t cb)
{
    rda59xx_scan_info scan_info;
    memset(&scan_info, 0, sizeof(rda59xx_scan_info));
    scan_info.scan_mode = 1;
    scan_info.scan_time = 3;
    rda59xx_scan(&scan_info);

    int ap_num = 0, i;
    if(cb != NULL)
    {
        ap_list_adv_t ap_info;
        rda59xx_scan_result *ap_records;
        ap_num = rda59xx_get_scan_num();
        if (ap_num > 50)
            ap_num = 50;

        ap_records = aos_malloc(ap_num * sizeof(*ap_records));
        if (!ap_records)
            return 0;

        rda59xx_get_scan_result(ap_records, ap_num);

        for (i = 0; i < ap_num; i++)
        {
            rda59xx_scan_result *r = ap_records + i;
            memset(&ap_info, 0, sizeof(ap_info));
            memcpy(ap_info.bssid, r->BSSID, sizeof(ap_info.bssid));
            memcpy(ap_info.ssid, r->SSID, r->SSID_len);
            ap_info.ap_power = transform_rssi(r->RSSI);
            ap_info.channel = r->channel;
            ap_info.security = r->secure_type;

            cb(&ap_info);
        }
        if (ap_records)
            aos_free(ap_records);
    }
    return ap_num;
}

int32_t rda_get_macaddr_from_flash(unsigned char *macaddr)
{
    memcpy((char *)macaddr, hfilop_layer_get_mac(), 6);
    return 0;
}

char* rda59xx_sta_netif_get_hostname(void)
{
    char* host_name = hfilop_layer_get_hostname();
    printf("Vendor host name: %.*s\n", 25, host_name);
    return host_name;
}

int board_wifi_init_hook(hal_wifi_module_t *m)
{
    hfilop_config_init();
    extern ota_hal_module_t ota_hal_module;
    ota_hal_register_module(&ota_hal_module);

    hfilop_ota_auto_upgrade(NULL, NULL);

    extern int hfilop_mac_key_is_valid(void);
    if(!hfilop_mac_key_is_valid())
    {
        hfilop_uart_task_start(NULL, NULL);
        while(1) aos_msleep(1000);
    }
}

void board_init(void)
{
    hal_uart_init(&uart_0);
}

void board_init_later(void)
{
    hfilop_init_rf_type(MODULE_TYPE);
    hfilop_assis_task_start();
    hfilop_check_ota_state();
    printf("====board_init_later done===\r\n");
}
