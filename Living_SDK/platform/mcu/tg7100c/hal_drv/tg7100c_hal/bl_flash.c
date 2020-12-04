#include <tg7100c_romdriver.h>

#include "bl_flash.h"
#include <blog.h>
#define USER_UNUSED(a) ((void)(a))

static struct {
    uint32_t magic;
    SPI_Flash_Cfg_Type flashCfg;
} boot2_flashCfg; //XXX Dont change the name of varaible, since we refer this boot2_partition_table in linker script

int bl_flash_erase(uint32_t addr, int len)
{
    /*We assume mid zeor is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }
    log_info("bl_flash_erase : addr = %08lx, len = %d\r\n", addr, len);
    RomDriver_XIP_SFlash_Erase_With_Lock(
            &boot2_flashCfg.flashCfg,
            addr,
            len
    );
    return 0;
}

int bl_flash_write(uint32_t addr, uint8_t *src, int len)
{
    /*We assume mid zeor is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }

    log_info("bl_flash_write : addr = %08lx, len = %d\r\n", addr, len);
    RomDriver_XIP_SFlash_Write_With_Lock(
            &boot2_flashCfg.flashCfg,
            addr,
            src,
            len
    );
    return 0;
}

int bl_flash_read(uint32_t addr, uint8_t *dst, int len)
{
    /*We assume mid zeor is illegal*/
    if (0 == boot2_flashCfg.flashCfg.mid) {
        return -1;
    }

    RomDriver_XIP_SFlash_Read_With_Lock(
            &boot2_flashCfg.flashCfg,
            addr,
            dst,
            len
    );
    return 0;
}

static void _dump_flash_config()
{
    extern uint8_t __boot2_flashCfg_src;

    USER_UNUSED(__boot2_flashCfg_src);

    blog_info("======= FlashCfg magiccode @%p, code 0x%08lX =======\r\n",
            &__boot2_flashCfg_src,
            boot2_flashCfg.magic
    );
    blog_info("mid \t\t0x%X\r\n", boot2_flashCfg.flashCfg.mid);
    blog_info("clkDelay \t0x%X\r\n", boot2_flashCfg.flashCfg.clkDelay);
    blog_info("clkInvert \t0x%X\r\n", boot2_flashCfg.flashCfg.clkInvert);
    blog_info("sector size\t%uKBytes\r\n", boot2_flashCfg.flashCfg.sectorSize);
    blog_info("page size\t%uBytes\r\n", boot2_flashCfg.flashCfg.pageSize);
    blog_info("---------------------------------------------------------------\r\n");
}

int bl_flash_config_update(void)
{
    _dump_flash_config();

    return 0;
}

int bl_flash_read_byxip(uint32_t addr, uint8_t *dst, int len)
{
    uint32_t offset;
    uint32_t xipaddr;

    offset = RomDriver_SF_Ctrl_Get_Flash_Image_Offset();

    if ((addr < offset) || (addr >= 0x1000000)) {
        // not support or arg err ?
        return -1;
    }

    xipaddr =  0x23000000 - offset + addr;

    memcpy(dst, (void *)xipaddr, len);

    return 0;
}

#include <aos/aos.h>
#include "iot_export.h"

//设置三元组到flash
void handle_set_linkkey_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc == 5 || argc == 6) {
        aos_kv_set(KV_KEY_PK, argv[1], strlen(argv[1]) + 1, 1);
        aos_kv_set(KV_KEY_DN, argv[2], strlen(argv[2]) + 1, 1);
        aos_kv_set(KV_KEY_DS, argv[3], strlen(argv[3]) + 1, 1);
        aos_kv_set(KV_KEY_PS, argv[4], strlen(argv[4]) + 1, 1);
        if (argc == 6)
            aos_kv_set(KV_KEY_PD, argv[5], strlen(argv[5]) + 1, 1);
        aos_uart_send("OK", strlen("OK"), 0);
    } else {
        aos_uart_send("ERROR", strlen("ERROR"), 0);
        return;
    }
}

//从flash获取三元组
void handle_get_linkkey_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int len = 0;
    char product_key[PRODUCT_KEY_LEN + 1] = { 0 };
    char product_secret[PRODUCT_SECRET_LEN + 1] = { 0 };
    char device_name[DEVICE_NAME_LEN + 1] = { 0 };
    char device_secret[DEVICE_SECRET_LEN + 1] = { 0 };
    char pidStr[9] = { 0 };
    char buf[255] = {0};

    len = PRODUCT_KEY_LEN + 1;
    aos_kv_get(KV_KEY_PK, product_key, &len);
    

    len = PRODUCT_SECRET_LEN + 1;
    aos_kv_get(KV_KEY_PS, product_secret, &len);

    len = DEVICE_NAME_LEN + 1;
    aos_kv_get(KV_KEY_DN, device_name, &len);

    len = DEVICE_SECRET_LEN + 1;
    aos_kv_get(KV_KEY_DS, device_secret, &len);

    // len = sizeof(pidStr);
    // aos_kv_get(KV_KEY_PD, pidStr, &len);

    len = sizeof(pidStr);
    if (aos_kv_get(KV_KEY_PD, pidStr, &len) == 0) {
        sprintf(buf,"+LINKKEYCONFIG=\"%s\",\"%s\",\"%s\",\"%s\",\"%d\"\r\n",product_key,device_name,device_secret,product_secret,atoi(pidStr));
        aos_uart_send(buf,strlen(buf),0);
        return;
    }
    sprintf(buf,"+LINKKEYCONFIG=\"%s\",\"%s\",\"%s\",\"%s\"\r\n",product_key,device_name,device_secret,product_secret);
    aos_uart_send(buf,strlen(buf),0);
}