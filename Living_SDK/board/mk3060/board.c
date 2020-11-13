/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include <aos/kernel.h>
#include <aos/aos.h>

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
	[HAL_PARTITION_BOOTLOADER] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Bootloader",
	    .partition_start_addr       = 0x0,
	    .partition_length           = 0x10000,    //64k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	},
	[HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x10000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    { //This partition just for kv,config mk3060 kv flash size to 4K
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x11000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "Application",
	    .partition_start_addr       = 0x13000,
	    .partition_length           = 0xED000, //948k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "OTA Storage",
        .partition_start_addr      = 0x100000,
        .partition_length          = 0xED000, //948k bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x1ED000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0xD000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

void qc_test(void);

#define KEY_STATUS 1
#define KEY_ELINK  2
#define KEY_BOOT   7

static uint64_t   elink_time = 0;
static gpio_dev_t gpio_key_boot, gpio_key_status;

static void key_poll_func(void *arg)
{
    uint32_t level;
    uint64_t diff;

    hal_gpio_input_get(&gpio_key_boot, &level);

    if (level == 0) {
        aos_post_delayed_action(10, key_poll_func, NULL);
    } else {
        diff = aos_now_ms() - elink_time;
        if (diff > 6000) { /*long long press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LLTCLICK);
        } else if (diff > 2000) { /* long press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_LTCLICK);
        } else if (diff > 40) { /* short press */
            elink_time = 0;
            aos_post_event(EV_KEY, CODE_BOOT, VALUE_KEY_CLICK);
        } else {
            aos_post_delayed_action(10, key_poll_func, NULL);
        }
    }
}

static void key_proc_work(void *arg)
{
    aos_schedule_call(key_poll_func, NULL);
}

static void handle_elink_key(void *arg)
{
    uint32_t gpio_value;

    hal_gpio_input_get(&gpio_key_boot, &gpio_value);
    if (gpio_value == 0 && elink_time == 0) {
        elink_time = aos_now_ms();
        aos_loop_schedule_work(0, key_proc_work, NULL, NULL, NULL);
    }
}

/* For QC test */
static void board_qc_check(void)
{
    uint32_t gpio_value = 1;

    gpio_key_boot.port = KEY_BOOT;
    gpio_key_boot.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_key_boot);
    hal_gpio_input_get(&gpio_key_boot, &gpio_value);
    
    if (gpio_value != 0) {
        return;
    }

    gpio_value = 1;
    gpio_key_status.port = KEY_STATUS;
    gpio_key_status.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_key_status);
    hal_gpio_input_get(&gpio_key_status, &gpio_value);
    if (gpio_value != 0) {
        return;
    }

    // QC:
    printf("Enter QC mode\r\n");
    qc_test();
    return;
}

void board_init(void)
{
    gpio_key_boot.port = KEY_BOOT;

    board_qc_check();
    hal_gpio_clear_irq(&gpio_key_boot);
    hal_gpio_enable_irq(&gpio_key_boot, IRQ_TRIGGER_FALLING_EDGE, handle_elink_key, NULL);
}
