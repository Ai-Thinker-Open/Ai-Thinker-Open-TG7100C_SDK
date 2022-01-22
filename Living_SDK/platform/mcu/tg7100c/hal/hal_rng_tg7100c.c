
#include "hal/soc/soc.h"
#include "bl_sec.h"
#include <stdio.h>

int32_t hal_random_num_read(random_dev_t random, void *buf, int32_t bytes)
{
    uint32_t tmp;
    uint32_t *val;
    int i, nwords;

	if(buf == NULL || bytes < 1)
	{
		return -1;
	}
    
    val = (uint32_t *)buf;

    nwords = bytes / 4;

    for (i = 0;i < nwords;i++) {
        val[i] = bl_rand();
    }
    val += nwords;

    tmp = bl_rand();
    
    if (bytes % 4 == 1) {
        *((uint8_t *)val) = (uint8_t)tmp;
    }
    if (bytes % 4 == 2) {
        *((uint8_t *)val) = (uint8_t)tmp;
        *((uint8_t *)val + 1) = (uint8_t)(tmp >> 8);
    }
    if (bytes % 4 == 3) {
        *((uint8_t *)val) = (uint8_t)tmp;
        *((uint8_t *)val + 1) = (uint8_t)(tmp >> 16);
        *((uint8_t *)val + 2) = (uint8_t)(tmp >> 24);
    }
   
    return 0;
}


