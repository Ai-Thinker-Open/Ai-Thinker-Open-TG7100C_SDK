#include <tg7100c_glb.h>
#include <tg7100c_xip_sflash.h>
#include <tg7100c_sf_cfg.h>
#include <tg7100c_sf_cfg_ext.h>
#include <tg7100c_romdriver.h>
#include <tg7100c_xip_sflash_ext.h>
#include "bl_flash.h"
#include <blog.h>

int bl_irq_save(void)
{
  uint32_t oldstat;

  /* Read mstatus & clear machine interrupt enable (MIE) in mstatus */
  __asm volatile("csrrc %0, mstatus, %1" : "=r"(oldstat) : "r"(8));
  return oldstat;
}

void bl_irq_restore(int flags)
{
  __asm volatile("csrw mstatus, %0"
               : /* no output */
               : "r"(flags));
}

#define GLOBAL_IRQ_DIABLE()  if (1) { \
  int ____global_prev_mie____ = bl_irq_save();

#define GLOBAL_IRQ_RESTORE() \
  bl_irq_restore(____global_prev_mie____ ); \
}

#define BL_FLASH_CFG_MAGIC "FCFG"

static SPI_Flash_Cfg_Type g_flash_cfg = {
    .resetCreadCmd=0xff,
    .resetCreadCmdSize=3,
    .mid=0xc8,

    .deBurstWrapCmd=0x77,
    .deBurstWrapCmdDmyClk=0x3,
    .deBurstWrapDataMode=SF_CTRL_DATA_4_LINES,
    .deBurstWrapData=0xF0,

    /*reg*/
    .writeEnableCmd=0x06,
    .wrEnableIndex=0x00,
    .wrEnableBit=0x01,
    .wrEnableReadRegLen=0x01,

    .qeIndex=1,
    .qeBit=0x01,
    .qeWriteRegLen=0x02,
    .qeReadRegLen=0x1,

    .busyIndex=0,
    .busyBit=0x00,
    .busyReadRegLen=0x1,
    .releasePowerDown=0xab,

    .readRegCmd[0]=0x05,
    .readRegCmd[1]=0x35,
    .writeRegCmd[0]=0x01,
    .writeRegCmd[1]=0x01,

    .fastReadQioCmd=0xeb,
    .frQioDmyClk=16/8,
    .cReadSupport=1,
    .cReadMode=0xA0,

    .burstWrapCmd=0x77,
    .burstWrapCmdDmyClk=0x3,
    .burstWrapDataMode=SF_CTRL_DATA_4_LINES,
    .burstWrapData=0x40,
     /*erase*/
    .chipEraseCmd=0xc7,
    .sectorEraseCmd=0x20,
    .blk32EraseCmd=0x52,
    .blk64EraseCmd=0xd8,
    /*write*/
    .pageProgramCmd=0x02,
    .qpageProgramCmd=0x32,
    .qppAddrMode=SF_CTRL_ADDR_1_LINE,

    .ioMode=SF_CTRL_QIO_MODE,
    .clkDelay=0,
    .clkInvert=0x3,

    .resetEnCmd=0x66,
    .resetCmd=0x99,
    .cRExit=0xff,
    .wrEnableWriteRegLen=0x00,

    /*id*/
    .jedecIdCmd=0x9f,
    .jedecIdCmdDmyClk=0,
    .qpiJedecIdCmd=0x9f,
    .qpiJedecIdCmdDmyClk=0x00,
    .sectorSize=4,
    .pageSize=256,

    /*read*/
    .fastReadCmd=0x0b,
    .frDmyClk=8/8,
    .qpiFastReadCmd =0x0b,
    .qpiFrDmyClk=8/8,
    .fastReadDoCmd=0x3b,
    .frDoDmyClk=8/8,
    .fastReadDioCmd=0xbb,
    .frDioDmyClk=0,
    .fastReadQoCmd=0x6b,
    .frQoDmyClk=8/8,

    .qpiFastReadQioCmd=0xeb,
    .qpiFrQioDmyClk=16/8,
    .qpiPageProgramCmd=0x02,
    .writeVregEnableCmd=0x50,

    /* qpi mode */
    .enterQpi=0x38,
    .exitQpi=0xff,

     /*AC*/
    .timeEsector=300,
    .timeE32k=1200,
    .timeE64k=1200,
    .timePagePgm=5,
    .timeCe=20*1000,
    .pdDelay=20,
    .qeData=0,
};

void *ATTR_TCM_SECTION arch_memcpy(void *dst, const void *src, uint32_t n)
{
    const uint8_t *p = src;
    uint8_t *q = dst;

    while (n--) {
        *q++ = *p++;
    }

    return dst;
}

/**
 * @brief flash_get_cfg
 *
 * @return BL_Err_Type
 */
BL_Err_Type flash_get_cfg(uint8_t **cfg_addr, uint32_t *len)
{
    *cfg_addr = (uint8_t *)&g_flash_cfg;
    *len = sizeof(SPI_Flash_Cfg_Type);

    return SUCCESS;
}

/**
 * @brief flash_set_qspi_enable
 *
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_set_qspi_enable(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        SFlash_Qspi_Enable(p_flash_cfg);
    }

    return SUCCESS;
}

/**
 * @brief flash_set_l1c_wrap
 *
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_set_l1c_wrap(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if (((p_flash_cfg->ioMode >> 4) & 0x01) == 1) {
        L1C_Set_Wrap(DISABLE);
    } else {
        L1C_Set_Wrap(ENABLE);
        if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
            SFlash_SetBurstWrap(p_flash_cfg);
        }
    }

    return SUCCESS;
}

/**
 * @brief flash_config_init
 *
 * @return BL_Err_Type
 */
static BL_Err_Type ATTR_TCM_SECTION flash_config_init(SPI_Flash_Cfg_Type *p_flash_cfg, uint8_t *jedec_id)
{
    BL_Err_Type ret = ERROR;
    uint8_t isAesEnable = 0;
    uint32_t jid = 0;
    uint32_t offset = 0;

    GLOBAL_IRQ_DIABLE();
    XIP_SFlash_Opt_Enter(&isAesEnable);
    XIP_SFlash_State_Save(p_flash_cfg, &offset);
    SFlash_GetJedecId(p_flash_cfg, (uint8_t *)&jid);
    arch_memcpy(jedec_id, (uint8_t *)&jid, 3);
    jid &= 0xFFFFFF;
    ret = SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(jid, p_flash_cfg);
    if (ret == SUCCESS) {
        p_flash_cfg->mid = (jid & 0xff);
    }

    /* Set flash controler from p_flash_cfg */
    flash_set_qspi_enable(p_flash_cfg);
    flash_set_l1c_wrap(p_flash_cfg);
    XIP_SFlash_State_Restore_Ext(p_flash_cfg, offset);
    XIP_SFlash_Opt_Exit(isAesEnable);
    GLOBAL_IRQ_RESTORE();

    return ret;
}

BL_Err_Type flash_get_bootheader_cfg(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    uint8_t buf[sizeof(SPI_Flash_Cfg_Type)+8];
    uint32_t crc,*pCrc;
    
    XIP_SFlash_Read_Need_Lock_Ext(p_flash_cfg, 8, buf, sizeof(SPI_Flash_Cfg_Type)+8);
    if(TG7100C_MemCmp(buf,BL_FLASH_CFG_MAGIC,4)==0){
        crc=BFLB_Soft_CRC32((uint8_t *)buf+4,sizeof(SPI_Flash_Cfg_Type));
        pCrc=(uint32_t *)(buf+4+sizeof(SPI_Flash_Cfg_Type));
        if(*pCrc==crc){
            TG7100C_MemCpy_Fast(p_flash_cfg,(uint8_t *)buf+4,sizeof(SPI_Flash_Cfg_Type));
            return SUCCESS;
        }
    }
    
    return ERROR;
}

/**
 * @brief multi flash adapter
 *
 * @return BL_Err_Type
 */
int ATTR_TCM_SECTION flash_init(void)
{
    int ret = 1;
    uint8_t clkDelay = 1;
    uint8_t clkInvert = 1;
    uint32_t jedec_id = 0;

    flash_get_bootheader_cfg(&g_flash_cfg);
    if (g_flash_cfg.mid != 0xff) {
        return SUCCESS;
    }
    clkDelay = g_flash_cfg.clkDelay;
    clkInvert = g_flash_cfg.clkInvert;
    g_flash_cfg.ioMode = g_flash_cfg.ioMode & 0x0f;

    ret = flash_config_init(&g_flash_cfg, (uint8_t *)&jedec_id);
#if 0
    MSG("flash ID = %08x\r\n", jedec_id);
    bflb_platform_dump((uint8_t *)&g_flash_cfg, sizeof(g_flash_cfg));
    if (ret != SUCCESS) {
        MSG("flash config init fail!\r\n");
    }
#endif
    g_flash_cfg.clkDelay = clkDelay;
    g_flash_cfg.clkInvert = clkInvert;

    return ret;
}

#define USER_UNUSED(a) ((void)(a))
#define USER_OPSFLASH_ROMAPIWITHLOCK   (1)

#if 0
static struct {
    uint32_t magic;
    SPI_Flash_Cfg_Type flashCfg;
} boot2_flashCfg; //XXX Dont change the name of varaible, since we refer this boot2_partition_table in linker script
#endif

int ATTR_TCM_SECTION bl_flash_erase(uint32_t addr, int len)
{
    /*We assume mid zeor is illegal*/
    if (0 == g_flash_cfg.mid) {
        return -1;
    }
    log_info("bl_flash_erase : addr = %08lx, len = %d\r\n", addr, len);

#if USER_OPSFLASH_ROMAPIWITHLOCK
    GLOBAL_IRQ_DIABLE();
    XIP_SFlash_Clear_Status_Register_Need_Lock(&g_flash_cfg);
    RomDriver_XIP_SFlash_Erase_Need_Lock(
            &g_flash_cfg,
            addr,
            addr + len - 1
    );
    GLOBAL_IRQ_RESTORE();
#else
    RomDriver_XIP_SFlash_Erase_With_Lock(
            &g_flash_cfg,
            addr,
            len
    );
#endif
    return 0;
}

int ATTR_TCM_SECTION bl_flash_write(uint32_t addr, uint8_t *src, int len)
{
    /*We assume mid zeor is illegal*/
    if (0 == g_flash_cfg.mid) {
        return -1;
    }

    log_info("bl_flash_write : addr = %08lx, len = %d\r\n", addr, len);
#if USER_OPSFLASH_ROMAPIWITHLOCK
    GLOBAL_IRQ_DIABLE();
    XIP_SFlash_Clear_Status_Register_Need_Lock(&g_flash_cfg);
    RomDriver_XIP_SFlash_Write_Need_Lock(
            &g_flash_cfg,
            addr,
            src,
            len
    );
    GLOBAL_IRQ_RESTORE();
#else
    RomDriver_XIP_SFlash_Write_With_Lock(
            &g_flash_cfg,
            addr,
            src,
            len
    );
#endif
    return 0;
}

int ATTR_TCM_SECTION bl_flash_read(uint32_t addr, uint8_t *dst, int len)
{
    /*We assume mid zeor is illegal*/
    if (0 == g_flash_cfg.mid) {
        return -1;
    }

#if USER_OPSFLASH_ROMAPIWITHLOCK
    GLOBAL_IRQ_DIABLE();
    RomDriver_XIP_SFlash_Read_Need_Lock(
            &g_flash_cfg,
            addr,
            dst,
            len
    );
    GLOBAL_IRQ_RESTORE();
#else
    RomDriver_XIP_SFlash_Read_With_Lock(
            &g_flash_cfg,
            addr,
            dst,
            len
    );
#endif
    return 0;
}

static void _dump_flash_config()
{
    extern uint8_t __boot2_flashCfg_src;

    USER_UNUSED(__boot2_flashCfg_src);
    
    blog_info("======= FlashCfg magiccode @%p=======\r\n", &__boot2_flashCfg_src);
    blog_info("mid \t\t0x%X\r\n", g_flash_cfg.mid);
    blog_info("clkDelay \t0x%X\r\n", g_flash_cfg.clkDelay);
    blog_info("clkInvert \t0x%X\r\n", g_flash_cfg.clkInvert);
    blog_info("sector size\t%uKBytes\r\n", g_flash_cfg.sectorSize);
    blog_info("page size\t%uBytes\r\n", g_flash_cfg.pageSize);
    blog_info("---------------------------------------------------------------\r\n");
}

int bl_flash_config_update(void)
{
    _dump_flash_config();

    return 0;
}

void* bl_flash_get_flashCfg(void)
{
    return &g_flash_cfg;
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
    aos_uart_send("OK", strlen("OK"), 0);
}
