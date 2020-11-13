/*
Date    : 2018/07/26
Author  : Snow Yang
Mail    : yangsw@mxchip.com
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <hal/ota.h>
#include <aos/aos.h>
#include <hal/soc/soc.h>
#include <CheckSumUtils.h>

#define LOG(fmt, ...)

#define KV_HAL_OTA_CRC16  "hal_ota_get_crc16"

typedef struct ota_hal_module_s {
    int (*init)(void *something);
    int (*write)(int *off_set, char *in_buf , int in_buf_len);
    int (*read)(int *off_set, char *out_buf , int out_buf_len);
    int (*boot)(void *something);
    int (*rollback)(void *something);
    const char *(*version)(unsigned char dev_type);
}ota_hal_module_t;

typedef struct
{
    uint32_t ota_len;
    uint32_t ota_crc;
} ota_reboot_info_t;

static ota_reboot_info_t ota_info;
static uint16_t hal_ota_get_crc16(void);
static void  hal_ota_save_crc16(uint16_t crc16);

static int boot_table_update( boot_table_t * boot_table )
{
  int ret = 0;
  uint32_t offset;

  boot_table_t *boot_table_dump = malloc(sizeof(boot_table_t));
  hal_flash_erase(HAL_PARTITION_PARAMETER_1, 0x0, hal_flash_get_info( HAL_PARTITION_PARAMETER_1 )->partition_length);
  offset = 0x0;
  hal_flash_write(HAL_PARTITION_PARAMETER_1, &offset, boot_table, sizeof(boot_table_t));
  offset = 0x0;
  hal_flash_read(HAL_PARTITION_PARAMETER_1, &offset, boot_table_dump, sizeof(boot_table_t));
  if(memcmp(boot_table, boot_table_dump, sizeof(boot_table_t)) != 0)
  {
    ret = -1;
  }

  free(boot_table_dump);
  return ret;
}

int hal_ota_switch_to_new_fw(uint8_t parti, int ota_data_len, uint16_t ota_data_crc )
{
    int ret = 0;
    boot_table_t* boot_table = malloc(sizeof(boot_table_t));
    memset(boot_table, 0, sizeof(boot_table_t));
    boot_table->length = ota_data_len;
    boot_table->start_address = hal_flash_get_info( HAL_PARTITION_OTA_TEMP )->partition_start_addr;
    boot_table->type = 'A';
    boot_table->upgrade_type = 'U';
    boot_table->crc = ota_data_crc;
    if(boot_table_update(boot_table) != 0)
    {
        ret = -1;
    }
    else
    {
        /* reboot */
        hal_reboot();
    }

    free(boot_table);
    return ret;
}

static  CRC16_Context contex;

unsigned int _off_set = 0;

typedef struct  {
    unsigned int dst_adr;
    unsigned int src_adr;
    unsigned int len;
    unsigned short crc;
    unsigned int  upg_flag;
    unsigned char boot_count;
    unsigned int  splict_size;
    int off_bp;   /*Break point offset*/
    unsigned char  res_type; /*result type: OTA_FINISH, OTA_BREAKPOINT*/
    unsigned short param_crc; /*Parameter crc*/
} __attribute__((packed)) ota_boot_param_t;

static int ota_init(void *something)
{
    hal_logic_partition_t *partition_info;

    ota_boot_param_t *param = (ota_boot_param_t *)something;
    _off_set = param->off_bp;
    ota_info.ota_len=_off_set;
    LOG("ota_init(), offset=%d", param->off_bp);
    if(_off_set==0)
    {
        wdg_dev_t sys_wdg;
        sys_wdg.config.timeout = 20000;
        hal_wdg_init(&sys_wdg);
        partition_info = hal_flash_get_info( HAL_PARTITION_OTA_TEMP );
        hal_wdg_reload(NULL);
        hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0 ,partition_info->partition_length);
        hal_wdg_reload(NULL);
        CRC16_Init( &contex );
        LOG("ota_init(), return");
    }

    return 0;
}

static int ota_write(volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
    LOG("ota_write(), offset=0x%x, length=0x%x", *off_set, in_buf_len);
    if(ota_info.ota_len == 0) {
        _off_set = 0;
        CRC16_Init( &contex );
        memset(&ota_info, 0 , sizeof ota_info);
    }
    CRC16_Update( &contex, in_buf, in_buf_len);
    hal_wdg_reload(NULL);
    int ret = hal_flash_write(HAL_PARTITION_OTA_TEMP, &_off_set, in_buf, in_buf_len);
    hal_wdg_reload(NULL);
    ota_info.ota_len += in_buf_len;
    return ret;
}

static int ota_read(volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    hal_flash_read(HAL_PARTITION_OTA_TEMP, off_set, out_buf, out_buf_len);
    return 0;
}

static int ota_set_boot(void *something)
{
    ota_boot_param_t *param = (ota_boot_param_t *)something;

    LOG("ota_set_boot(), type=%d", param->res_type);

    if (param==NULL){
        return -1;
    }

    if (param->res_type==OTA_FINISH)
    {
        CRC16_Final( &contex, &ota_info.ota_crc );
        LOG("ota_set_boot(), switch to new firmware");
        hal_ota_switch_to_new_fw(0, ota_info.ota_len, ota_info.ota_crc);
        memset(&ota_info, 0 , sizeof ota_info);
    }

    return 0;
}

static const char *ota_get_version(unsigned char dev_type)
{
    if(dev_type) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    } else {
        return aos_get_app_version();
    }
}

ota_hal_module_t ota_hal_module = {
    .init     = ota_init,
    .write    = ota_write,
    .read     = ota_read,
    .boot     = ota_set_boot,
    .rollback = NULL,
    .version  = ota_get_version
};


static uint16_t hal_ota_get_crc16(void)
{
    int len = 2;
    uint16_t crc16=0;
    aos_kv_get(KV_HAL_OTA_CRC16, &crc16, &len);
    return crc16;
}

static void  hal_ota_save_crc16(uint16_t crc16)
{
    aos_kv_set(KV_HAL_OTA_CRC16, &crc16, 2, 1);
}
