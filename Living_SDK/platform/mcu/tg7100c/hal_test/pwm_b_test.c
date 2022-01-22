/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <aos/hal/gpio.h>
#include <ulog/ulog.h>
#include <yoc/atserver.h>
#include <pin_name.h>
#include <time.h>
#include <sys/time.h>

#define GPIO_BUTTON_IO_1 PA13
#define GPIO_BUTTON_IO_2 PA23

gpio_dev_t button1,button2;
struct timeval start_1;
struct timeval end_1;
int stop_flag = 0;
int duty_time_1=0;
int duty_time_2=0;
int sum_time = 0;

void falling_entry(void *arg)
{
    gettimeofday(&start_1, NULL);
    duty_time_2 = 1000000 * (end_1.tv_sec - start_1.tv_sec) + (start_1.tv_usec - end_1.tv_usec);

	printf("enter button1 fun\n");
}

void rising_entry(void *arg)
{
    int i = 0;
    gettimeofday(&end_1, NULL);
    
    duty_time_1 = 1000000 * (end_1.tv_sec - start_1.tv_sec) + (end_1.tv_usec - start_1.tv_usec);
	sum_time = duty_time_2 + duty_time_1;

    /*for (i=0; i<100; i++) {
        if (duty_time_1<97000||duty_time_1>103000) {
            AT_BACK_ERR();
            return;
        }
    }*/
    printf("enter button2 fun=%d\n",duty_time_1);
    if (sum_time < 0) {
        return;
    }
    for (i=0; i<100; i++) {
        if (sum_time>200300||sum_time<196000) {
            AT_BACK_ERR();
            return;
        }
    }
    AT_BACK_OK();
    stop_flag=1;
}

void hal_pwm_b_test(void)
{
    int ret = 0;
    int i = 20;
    /* input pin config */
	button1.port = GPIO_BUTTON_IO_1;
    button2.port = GPIO_BUTTON_IO_2;

	/* set as interrupt mode */
	button1.config = IRQ_MODE;
    button2.config = IRQ_MODE;
    
	/* configure GPIO with the given settings */
	ret = hal_gpio_init(&button1);
	if (ret != 0) {
	    printf("gpio init error !\n");
        AT_BACK_ERR();
	}
    ret = hal_gpio_init(&button2);
	if (ret != 0) {
	    printf("gpio init error !\n");
        AT_BACK_ERR();
	}
    button1.config = INPUT_PULL_UP;
    ret = hal_gpio_init(&button2);
	if (ret != 0) {
	    printf("gpio init error !\n");
        AT_BACK_ERR();
	}

    /*button2.config = INPUT_PULL_UP;
    ret = hal_gpio_init(&button2);
	if (ret != 0) {
	    printf("gpio init error !\n");
        AT_BACK_ERR();
	}*/
	/* gpio interrupt config */
    ret = hal_gpio_enable_irq(&button1, IRQ_TRIGGER_RISING_EDGE, 
                              rising_entry, NULL);
    if (ret != 0) {
        printf("gpio irq enable error !\n");
	}
    ret = hal_gpio_enable_irq(&button2, IRQ_TRIGGER_FALLING_EDGE, 
                              falling_entry, NULL);
    if (ret != 0) {
        printf("gpio irq enable error !\n");
	}
    while(1) {
        if (stop_flag==1) {
            break;
        }
    }
    hal_gpio_finalize(&button1);
    hal_gpio_finalize(&button2);
}

void test_hal_pwm_b(char *cmd, int type, char *data)
{
    drv_pinmux_config(GPIO_BUTTON_IO_1,PIN_FUNC_GPIO);
    drv_pinmux_config(GPIO_BUTTON_IO_2,PIN_FUNC_GPIO);
    // aos_msleep(5000);
    hal_pwm_b_test();
}