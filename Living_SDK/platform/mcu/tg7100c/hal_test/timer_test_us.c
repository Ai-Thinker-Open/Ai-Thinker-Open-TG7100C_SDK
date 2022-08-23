#include <stdio.h>
#include <hosal_timer.h>
#include <bl_gpio.h>
#include <blog.h>

hosal_timer_dev_t timer;

void timer_callback(void *arg)
{
    static int i = 0;
    if (i % 2) {
        bl_gpio_output_set(4,0);
    } else {
        bl_gpio_output_set(4,1);
    }
    i++;
}

void demo_hosal_timer_start(void)
{
    timer.port = 0;
    timer.config.period = 100; /* 100us */
    timer.config.reload_mode = TIMER_RELOAD_PERIODIC;
    timer.config.cb =timer_callback;
    timer.config.arg = NULL;
    bl_gpio_enable_output(4, 1, 0);
    hosal_timer_init(&timer);
    hosal_timer_start(&timer);
}

void demo_hosal_timer_stop(void)
{
    hosal_timer_stop(&timer);
    hosal_timer_finalize(&timer);
}

void test_hal_timer_us(char *cmd, int type, char *data)
{
    if (strcmp((const char *)data, "start") == 0) {
        demo_hosal_timer_start();
    }else if (strcmp((const char *)data, "stop") == 0) {
        demo_hosal_timer_stop();
    }else {
        printf("error\r\n");
    }
}


