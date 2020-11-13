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

