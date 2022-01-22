/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#define AT_BACK_OK() printf("[TEST][HAL][OK]...\r\n")
#define AT_BACK_ERR() printf("[TEST][HAL][ERROR] %s %d\r\n", __FUNCTION__, __LINE__)
#define AT_BACK_READY() printf("[TEST]...")
