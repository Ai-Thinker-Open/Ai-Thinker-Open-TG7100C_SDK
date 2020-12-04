#ifndef __BL_EFUSE_H__
#define __BL_EFUSE_H__
#include <stdint.h>
int bl_efuse_read_mac(uint8_t mac[6]);
int bl_efuse_read_mac_factory(uint8_t mac[6]);
int bl_efuse_read_capcode(uint8_t *capcode);
int bl_efuse_read_pwroft(int8_t poweroffset[14]);

//先检查flash中的MAC是否有效，有效则使用flash中的MAC,若无效则使用EFUSE中的MAC
char* bl_flash_efuse_read_mac(uint8_t mac[6]);

#endif
