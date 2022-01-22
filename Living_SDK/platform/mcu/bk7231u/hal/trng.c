#include "hal/soc/soc.h"
#include "typedef.h"
#include "drv_model_pub.h"
#include "irda_pub.h"

int32_t hal_random_num_read(random_dev_t random, void *buf, int32_t bytes)
{
    uint32_t i;
    uint32_t tmp;

	if(buf == NULL || bytes < 1)
	{
		return -1;
	}

	for(i = 0; i < bytes; i+=sizeof(uint32_t)) 
	{
		sddev_control(IRDA_DEV_NAME, TRNG_CMD_GET,(void *)&tmp);
		memcpy((void *)(buf+i), (void *)&tmp, sizeof(uint32_t) > (bytes - i) ? bytes - i : sizeof(uint32_t));
	}

	return 0;
}
