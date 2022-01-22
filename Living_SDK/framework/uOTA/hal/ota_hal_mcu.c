#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>

#include "aos/kernel.h"
#include "aos/kv.h"

#include "ota_hal_plat.h"
#include "ota_hal_os.h"
#include "ota_log.h"
#include "ota_verify.h"
#include "ota_hal_mcu.h"

#define OTA_CRC16  "ota_file_crc16"
static unsigned int wdata_len = 0;
static ota_crc16_ctx ctx = {0};

static unsigned short ota_get_crc16(void)
{
    int len = 2;
    unsigned short crc16=0;
    aos_kv_get(OTA_CRC16, &crc16, &len);
    return crc16;
}

static void  ota_set_crc16(unsigned short crc16)
{
    aos_kv_set(OTA_CRC16, &crc16, 2, 1);
}

int ota_mcu_init(void *something)
{
    int ret = 0;
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    param->off_bp = ota_get_break_point();
    if (param->off_bp >= param->len) {
        param->off_bp = 0;
    }
    wdata_len = param->off_bp;

    if (param->off_bp == 0) {
        ota_crc16_init(&ctx);
    } else {
        ctx.crc = ota_get_crc16();
    }
    OTA_LOG_I("ota mcu init off:0x%08x len:%d crc:0x%04x\n", param->off_bp, param->len, ctx.crc);
    return ret;
}

int ota_mcu_write(int* off, char* in_buf ,int in_buf_len)
{
    int ret = 0;
    if (off != NULL) {
        OTA_LOG_I("recv MCU data off:%d", *off);
    }

    /* for only test */
    wdata_len = wdata_len + in_buf_len; 
    OTA_LOG_I("recv MCU data wdata_len:%d, in_buf_len:%d", wdata_len, in_buf_len);

    ota_crc16_update(&ctx, in_buf, in_buf_len);
    return ret;
}

int ota_mcu_boot(void *something)
{
    int ret = 0;
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    if (param == NULL) {
        ret = OTA_REBOOT_FAIL;
        return ret;
    }
    if (param->res_type == OTA_FINISH) {
        ota_crc16_final(&ctx, &param->crc);
        ota_set_break_point(0);
        ota_msleep(1500);
        ota_reboot();
    } else if(param->res_type == OTA_BREAKPOINT) {
        OTA_LOG_I("save bp crc:0x%04x", ctx.crc);
        ota_set_crc16(ctx.crc);
    }
    return ret;
}

