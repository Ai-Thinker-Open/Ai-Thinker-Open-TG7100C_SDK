#include <tg7100c_romdriver.h>
#include <tg7100c_glb.h>

#include <stdio.h>
#include <stdbool.h>
#include "bl_sys.h"

volatile bool sys_log_all_enable = true;

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
    GLB_SW_POR_Reset();

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

    return 0;
}
