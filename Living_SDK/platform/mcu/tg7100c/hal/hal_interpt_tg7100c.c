/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"

/*HIFIVE1 int including local int and global int
for local int now we only care about IRQ_M_TIMER
for global int all go through local int IRQ_M_EXT
local IRQ_M_EXT can extend to PLIC_NUM_INTERRUPTS global interrupts
hal int funs are designed for IRQ_M_EXT
*/



int32_t hal_interpt_init(void)
{

}


int32_t hal_interpt_mask(int32_t vec)
{

    
    return 0;
}


int32_t hal_interpt_umask(int32_t vec)
{

    
    return 0;
}


int32_t hal_interpt_install(int32_t vec, hal_interpt_t handler, void *para,  char *name)
{

    
    return 0;
    
}


