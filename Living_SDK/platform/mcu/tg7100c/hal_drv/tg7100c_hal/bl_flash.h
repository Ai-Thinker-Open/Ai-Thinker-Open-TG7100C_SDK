#ifndef __BL_FLASH_H__
#define __BL_FLASH_H__

#include <stdint.h>

//三元组KEY值
#define KV_KEY_PK "product_key"
#define KV_KEY_PS "product_secret"
#define KV_KEY_DN "device_name"
#define KV_KEY_DS "device_secret"
#define KV_KEY_PD "product_id"
#define MAX_KEY_LEN (6)

int bl_flash_erase(uint32_t addr, int len);
int bl_flash_write(uint32_t addr, uint8_t *src, int len);
int bl_flash_read(uint32_t addr, uint8_t *dst, int len);
int bl_flash_config_update(void);

int bl_flash_read_byxip(uint32_t addr, uint8_t *dst, int len);

//设置三元组到flash
void handle_set_linkkey_cmd(char *pwbuf, int blen, int argc, char **argv);
//从flash获取三元组
void handle_get_linkkey_cmd(char *pwbuf, int blen, int argc, char **argv);
#endif
