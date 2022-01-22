/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#define BT_DBG_ENABLED 1

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include "hal/soc/soc.h"
#include "hal_test_entry.h"

#define TAG "wdg"
#define WDG_PORT_NUM 0

/* define dev */
wdg_dev_t wdg;

static int aos_hal_wdt_feed_test()
{
    int ret = -1;
    static int count = 0;

    /* wdg port set */
    wdg.port = WDG_PORT_NUM;

    /* set reload time to 1000ms */
    wdg.config.timeout = 10000; /* 1000ms */

    /* init wdg with the given settings */
    ret = hal_wdg_init(&wdg);
    if (ret != 0) {
        LOGI(TAG, "wdg init error !\n");
        return -1;
    }
    LOGI(TAG, "wdg start feed\n");
    while(1) {
    	/* clear wdg about every 500ms */
        /* finalize wdg */
        if (count < 20) {
            hal_wdg_reload(&wdg);
            LOGI(TAG, "wdg feed %d times\n", count);
        } else {
            LOGI(TAG, "wdg enter system reset!\n");
            break;
        }

        /* sleep 500ms */
        aos_msleep(500);
        count++;
    };

    hal_wdg_finalize(&wdg);
    aos_msleep(2000);
    return 0;
}

static int aos_hal_wdt_reboot()
{
    int ret = -1;
    static int count = 0;

    /* wdg port set */
    wdg.port = WDG_PORT_NUM;

    /* set reload time to 1000ms */
    wdg.config.timeout = 20; /* 1000ms */

    /* init wdg with the given settings */
    ret = hal_wdg_init(&wdg);
    if (ret != 0) {
        LOGI(TAG, "wdg init error !\n");
        return -1;
    }
    while(1) {
        aos_msleep(2000);
    }
    hal_wdg_finalize(&wdg);
    aos_msleep(2000);
    return 0;
}

static int aos_hal_wdt_longtime()
{
    int ret = -1;
    static int count = 0;

    /* wdg port set */
    wdg.port = WDG_PORT_NUM;

    /* set reload time to 1000ms */
    wdg.config.timeout = 30000; /* 1000ms */

    /* init wdg with the given settings */
    ret = hal_wdg_init(&wdg);
    if (ret != 0) {
        LOGI(TAG, "wdg init error !\n");
        return -1;
    }

    while(1) {
        aos_msleep(20000);
    }
    hal_wdg_finalize(&wdg);
    aos_msleep(2000);
    return 0;
}

void test_hal_wdt(char *cmd, int type, char *data)
{
    if (strcmp((const char *)data, "FEED") == 0) {
        aos_hal_wdt_feed_test();
    }else if (strcmp((const char *)data, "LONG_TIME") == 0) {
        aos_hal_wdt_longtime(10000);
    }else if (strcmp((const char *)data, "REBOOT") == 0) {
        aos_hal_wdt_reboot();
    }
}
