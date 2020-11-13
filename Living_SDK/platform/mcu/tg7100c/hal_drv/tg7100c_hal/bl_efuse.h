#ifndef __BL_EFUSE_H__
#define __BL_EFUSE_H__
#include <stdint.h>
int bl_efuse_read_mac(uint8_t mac[6]);
int bl_efuse_read_mac_factory(uint8_t mac[6]);
int bl_efuse_read_capcode(uint8_t *capcode);
int bl_efuse_read_pwroft(int8_t poweroffset[14]);
#endif
