#ifndef __BL_SEC_H__
#define __BL_SEC_H__
#include <stddef.h>
#include <stdint.h>
int bl_sec_init(void);
int bl_sec_test(void);
int bl_pka_test(void);
int bl_sec_aes_init(void);
int bl_sec_aes_enc(uint8_t *key, int keysize, uint8_t *input, uint8_t *output);
int bl_sec_aes_test(void);
uint32_t bl_sec_get_random_word(void);

/*SHA Engine API*/
int bl_sec_sha_test(void);
#endif
