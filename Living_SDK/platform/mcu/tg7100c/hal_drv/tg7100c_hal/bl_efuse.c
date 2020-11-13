#include "tg7100c_glb.h"
#include <tg7100c_ef_ctrl.h>
#include "bl_efuse.h"

int bl_efuse_read_mac(uint8_t mac[6])
{
    EF_Ctrl_Read_MAC_Address(mac);
    return 0;
}

int bl_efuse_read_mac_factory(uint8_t mac[6])
{
    if (0 == mfg_media_read_macaddr(mac, 1)) {
        return 0;
    }
    return -1;
}


int bl_efuse_read_capcode(uint8_t *capcode)
{
    if (0 == mfg_media_read_xtal_capcode(capcode, 1)) {
        return 0;
    }
    return -1;
}

int bl_efuse_read_pwroft(int8_t poweroffset[14])
{
    if (0 == mfg_media_read_poweroffset(poweroffset, 1)) {
        return 0;
    }
    return -1;
}

