/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>

const char  *aos_get_app_version(void)
{
    return (const char *)SYSINFO_APP_VERSION;
}
