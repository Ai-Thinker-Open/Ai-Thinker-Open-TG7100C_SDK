#include "tg7100c_glb.h"
#include <tg7100c_ef_ctrl.h>
#include "tg7100c_mfg_media.h"
#include "bl_efuse.h"

int bl_efuse_read_mac(uint8_t mac[6])
{
    EF_Ctrl_Read_MAC_Address(mac);
    return 0;
}

int bl_efuse_read_mac_factory(uint8_t mac[6])
{
    if (0 == mfg_media_read_macaddr_with_lock(mac, 1)) {
        return 0;
    }
    return -1;
}


int bl_efuse_read_capcode(uint8_t *capcode)
{
    if (0 == mfg_media_read_xtal_capcode_need_lock(capcode, 1)) {
        return 0;
    }
    return -1;
}

int bl_efuse_read_pwroft(int8_t poweroffset[14])
{
    if (0 == mfg_media_read_poweroffset_need_lock(poweroffset, 1)) {
        return 0;
    }
    return -1;
}

int bl_efuse_ctrl_program_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Program_Direct_R0(index, data, len);

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);

    return 0;
}

int bl_efuse_ctrl_read_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Read_Direct_R0(index, data, len);

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);

    return 0;
}

int bl_efuse_read_mac_opt(uint8_t slot, uint8_t mac[6], uint8_t reload)
{
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Read_MAC_Address_Opt(slot, mac, reload);

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);

    return 0;
}
