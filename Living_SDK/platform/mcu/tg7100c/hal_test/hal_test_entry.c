/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

extern void test_hal_rtc(char *cmd, int type, char *data);
extern void test_hal_timer(char *cmd, int type, char *data);
extern void test_hal_timer_us(char *cmd, int type, char *data);
extern void test_hal_eflash(char *cmd, int type, char *data);
extern void test_hal_spi_master(char *cmd, int type, char *data);
extern void test_hal_spi_slave(char *cmd, int type, char *data);
extern void test_hal_pwm(char *cmd, int type, char *data);
extern void test_hal_pwm_b(char *cmd, int type, char *data);
extern void test_hal_wdt(char *cmd, int type, char *data);
extern void test_hal_adc(char *cmd, int type, char *data);
extern void test_hal_iic_master(char *cmd, int type, char *data);
extern void test_hal_iic_slave(char *cmd, int type, char *data);
extern void test_hal_gpio(char *cmd, int type, char *data);
extern void test_hal_rng(char *cmd, int type, char *data);
extern void test_hal_uart_slaver(char *cmd, int type, char *data);
extern void test_hal_uart_master(char *cmd, int type, char *data);

static void cmd_test_hal_rtc(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }
    test_hal_rtc(NULL, NULL, argv[1]);
}
static void cmd_test_hal_timer(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        printf("param error\r\n");
        return;
    }    
    printf("argv[1]:%s\r\n", argv[1]);
    test_hal_timer(NULL, 0, argv[1]);
}

static void cmd_test_hal_timer_us(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        printf("param error\r\n");
        return;
    }    
    printf("argv[1]:%s\r\n", argv[1]);
    test_hal_timer_us(NULL, 0, argv[1]);
}

static void cmd_test_hal_eflash(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }
    test_hal_eflash(NULL, NULL, argv[1]);
}
static void cmd_test_hal_spi_master(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }
    test_hal_spi_master(NULL, NULL, argv[1]);
}
static void cmd_test_hal_spi_slave(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }
    test_hal_spi_slave(NULL, NULL, argv[1]);
}

static void cmd_test_hal_pwm(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        printf("param error\r\n");
        return;
    }    
    printf("argv[1]:%s\r\n", argv[1]);
    test_hal_pwm(NULL, 0, argv[1]);
}

static void cmd_test_hal_pwm_b(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void cmd_test_hal_wdt(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        printf("param error\r\n");
        return;
    }    
    printf("argv[1]:%s\r\n", argv[1]);
    test_hal_wdt(NULL, 0, argv[1]);
}

static void cmd_test_hal_adc(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }
    test_hal_adc(NULL, NULL, argv[1]);
}
static void cmd_test_hal_iic_master(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        printf("param error\r\n");
        return;
    }    
    printf("argv[1]:%s\r\n", argv[1]);
    test_hal_iic_master(NULL, 0, argv[1]);
}

static void cmd_test_hal_iic_slave(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void cmd_test_hal_gpio(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        printf("param error\r\n");
        return;
    }    
    test_hal_gpio(NULL, 0, argv[1]);
}

static void cmd_test_hal_rng(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        printf("param error\r\n");
        return;
    }    
    test_hal_rng(NULL, 0, argv[1]);
}

static void cmd_test_hal_uart_slaver(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }
    test_hal_uart_slaver(NULL, NULL, argv[1]);
}
static void cmd_test_hal_uart_master(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }
    test_hal_uart_master(NULL, NULL, argv[1]);
}

static const struct cli_command g_hal_test_cmd[] = {
    {"test_rtc", "test_rtc", cmd_test_hal_rtc},
    {"test_timer", "", cmd_test_hal_timer},
    {"test_timer_us", "test timer us", cmd_test_hal_timer_us},
    {"test_eflash", "test_eflash", cmd_test_hal_eflash},
    {"test_spi_master", "test_spi_master", cmd_test_hal_spi_master},
    {"test_spi_slave", "test_spi_slave", cmd_test_hal_spi_slave},
    {"test_pwm", "", cmd_test_hal_pwm},
//    {"test_pwm_b", "", cmd_test_hal_pwm_b},
    {"test_wdt", "", cmd_test_hal_wdt},
    {"test_adc", "test_adc", cmd_test_hal_adc},
    {"test_iic_master", "", cmd_test_hal_iic_master},
//    {"test_iic_slave", "", cmd_test_hal_iic_slave},
    {"test_gpio", "", cmd_test_hal_gpio},
    {"test_rng", "", cmd_test_hal_rng},
    {"test_uart_slaver", "test_uart_slaver", cmd_test_hal_uart_slaver},
    {"test_uart_master", "test_uart_master", cmd_test_hal_uart_master},
};

void hal_test_entry(void)
{
    aos_cli_register_commands(g_hal_test_cmd, sizeof(g_hal_test_cmd)/sizeof(g_hal_test_cmd[0]));
}

