/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "lega_cm4.h"
#include <hw.h>
#include "board.h"

void hal_reboot(void)
{
    lega_system_reset();
}

void hw_start_hal(void)
{
    printf("start-----------hal\n");
}