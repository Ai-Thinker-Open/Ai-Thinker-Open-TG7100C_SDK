
#include "hal/soc/soc.h"
#include "bl_wdt.h"
#include <stdio.h>

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    uint32_t ms;

    if (NULL == wdg) 
    {
        return -1;
    }

    ms = wdg->config.timeout;

    printf("hal_wdg_init t_ms = %ld\r\n", ms);

    bl_wdt_init((int)ms);

    return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
    bl_wdt_feed();
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    bl_wdt_disable();

    return 0;
}

