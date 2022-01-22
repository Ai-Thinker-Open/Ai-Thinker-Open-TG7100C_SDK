#include <tg7100c_romdriver.h>
#include <tg7100c_glb.h>
#include <bl_flash.h>
#include <stdio.h>
#include <stdbool.h>
#include "bl_sys.h"

typedef void (*bl602_romdrv_reset_por) (void);
volatile bool sys_log_all_enable = true;

#define REASON_WDT        (0x77646F67) // watchdog reboot wdog
#define REASON_SOFTWARE   (0x736F6674) // software        soft
#define REASON_POWEROFF   (0x0) // software        soft

#define RST_REASON (*((volatile uint32_t *)0x40010000)) // use 4 Bytes

static BL_RST_REASON_E s_rst_reason = BL_RST_POWER_OFF;

static char *RST_REASON_ARRAY[] = {
    "BL_RST_POWER_OFF",
    "BL_RST_HARDWARE_WATCHDOG",
    "BL_RST_FATAL_EXCEPTION",
    "BL_RST_SOFTWARE_WATCHDOG",
    "BL_RST_SOFTWARE"
};

BL_RST_REASON_E bl_sys_rstinfo_get(void)
{
    BL_RST_REASON_E ret = s_rst_reason;

    s_rst_reason = REASON_POWEROFF;

    return ret;
}

int bl_sys_rstinfo_set(BL_RST_REASON_E val)
{
    if (val == BL_RST_SOFTWARE_WATCHDOG) {
        RST_REASON = REASON_WDT;
    } else if (val == BL_RST_SOFTWARE) {
        RST_REASON = REASON_SOFTWARE;
    }

    return 0;
}

void bl_sys_rstinfo_init(void)
{
    if (RST_REASON == REASON_WDT) {
        s_rst_reason = BL_RST_SOFTWARE_WATCHDOG;
    } else if (RST_REASON == REASON_SOFTWARE) {
        s_rst_reason = BL_RST_SOFTWARE;
    } else {
        s_rst_reason = BL_RST_POWER_OFF;
    }

    bl_sys_rstinfo_set(BL_RST_SOFTWARE_WATCHDOG);
}

int bl_sys_rstinfo_getsting(char *info)
{
    memcpy(info, (char *)RST_REASON_ARRAY[s_rst_reason], strlen(RST_REASON_ARRAY[s_rst_reason]));
    *(info + strlen(RST_REASON_ARRAY[s_rst_reason])) = '\0';
    return 0;
}

int bl_sys_logall_enable(void)
{
    sys_log_all_enable = true;
    return 0;
}

int bl_sys_logall_disable(void)
{
    sys_log_all_enable = false;
    return 0;
}

int bl_sys_reset_por(void)
{
    asm volatile("csrci mstatus, 8");
    asm volatile("nop;nop;nop;nop");
    asm volatile("nop;nop;nop;nop");

    ((bl602_romdrv_reset_por)(*((uint32_t *)(0x210108c4))))();

    /* never here */
    return 0;
}

void bl_sys_reset_system(void)
{
    GLB_SW_System_Reset();
}

int bl_sys_isxipaddr(uint32_t addr)
{
    if (((addr & 0xFF000000) == 0x23000000) || ((addr & 0xFF000000) == 0x43000000)) {
        return 1;
    }
    return 0;
}

int bl_sys_em_config(void)
{
    extern uint8_t __LD_CONFIG_EM_SEL;
    uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;

    switch (em_size) {
        case 0 * 1024:
        {
            GLB_Set_EM_Sel(GLB_EM_0KB);
        }
        break;
        case 8 * 1024:
        {
            GLB_Set_EM_Sel(GLB_EM_8KB);
        }
        break;
        case 16 * 1024:
        {
            GLB_Set_EM_Sel(GLB_EM_16KB);
        }
        break;
        default:
        {
            /*nothing here*/
        }
    }

    return 0;
}

int bl_sys_early_init(void)
{
    /* read flash config*/
    //extern int flash_init(void);
    flash_init();
    
    /* we ensure that the vdd core voltage is normal(1.2V) and the chip will work normally */
    uint8_t Ldo11VoutSelValue;
    extern BL_Err_Type EF_Ctrl_Read_Ldo11VoutSel_Opt(uint8_t *Ldo11VoutSelValue);
         
    if(0 == EF_Ctrl_Read_Ldo11VoutSel_Opt(&Ldo11VoutSelValue)){
        HBN_Set_Ldo11_Soc_Vout((HBN_LDO_LEVEL_Type)Ldo11VoutSelValue);
    }

    extern BL_Err_Type HBN_Aon_Pad_IeSmt_Cfg(uint8_t padCfg);
    HBN_Aon_Pad_IeSmt_Cfg(1);

#if 0// freertos
    extern void freertos_risc_v_trap_handler(void); //freertos_riscv_ram/portable/GCC/RISC-V/portASM.S
    write_csr(mtvec, &freertos_risc_v_trap_handler);
#else
    extern void trap_entry();
    write_csr(mtvec, (int)(trap_entry) | 0x02); /* CLIC direct mode */
#endif

    /*debuger may NOT ready don't print anything*/
    return 0;
}

int bl_sys_init(void)
{
    bl_sys_em_config();
    bl_sys_rstinfo_get(); 
    bl_sys_rstinfo_init();
    return 0;
}
