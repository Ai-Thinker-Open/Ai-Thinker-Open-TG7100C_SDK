/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>

unsigned int aos_log_level = AOS_LL_V_DEBUG | AOS_LL_V_INFO | AOS_LL_V_WARN | AOS_LL_V_ERROR | AOS_LL_V_FATAL;

aos_mutex_t log_mutex;

#ifndef csp_printf
__attribute__((weak)) int csp_printf(const char *fmt, ...)
{
    va_list args;
    int ret;

#ifdef AOS_DISABLE_ALL_LOGS
    if (0 == log_get_enable_aos_log_flag()) return;
#endif

    ret = aos_mutex_lock(&log_mutex, AOS_WAIT_FOREVER);
    if (ret == 0)
    {
        va_start(args, fmt);
        ret = vprintf(fmt, args);
        va_end(args);

        fflush(stdout);
    }

    aos_mutex_unlock(&log_mutex);

    return ret;
}
#endif

#if defined(LOG_SIMPLE)
void log_print_simple(unsigned int log_level, const char *fmt, ...)
{
    va_list args;
    int ret;
    char level_prt;

#ifdef AOS_DISABLE_ALL_LOGS
    if (0 == log_get_enable_aos_log_flag()) return;
#endif

    if ((aos_log_level & log_level) == 0)
    {
        return;
    }

    switch (log_level)
    {
    case AOS_LL_V_ALL:
        level_prt = 'A';
        break;
    case AOS_LL_V_FATAL:
        level_prt = 'F';
        break;
    case AOS_LL_V_ERROR:
        level_prt = 'E';
        break;
    case AOS_LL_V_WARN:
        level_prt = 'W';
        break;
    case AOS_LL_V_INFO:
        level_prt = 'I';
        break;
    case AOS_LL_V_DEBUG:
        level_prt = 'D';
        break;
    default:
        level_prt = ' ';
    }

    ret = aos_mutex_lock(&log_mutex, AOS_WAIT_FOREVER);
    if (ret != 0)
    {
        return;
    }

    printf("[%06d]<%c> ", (unsigned)aos_now_ms(), level_prt);
    va_start(args, fmt);
    ret = vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
    printf("\r\n");

    aos_mutex_unlock(&log_mutex);
}
#endif

void aos_set_log_level(aos_log_level_t log_level)
{
    unsigned int value = 0;

    switch (log_level)
    {
    case AOS_LL_NONE:
        value |= AOS_LL_V_NONE;
        break;
    case AOS_LL_DEBUG:
        value |= AOS_LL_V_DEBUG;
    case AOS_LL_INFO:
        value |= AOS_LL_V_INFO;
    case AOS_LL_WARN:
        value |= AOS_LL_V_WARN;
    case AOS_LL_ERROR:
        value |= AOS_LL_V_ERROR;
    case AOS_LL_FATAL:
        value |= AOS_LL_V_FATAL;
        break;
    default:
        break;
    }

    aos_log_level = value;
}

static void log_cmd(char *buf, int len, int argc, char **argv)
{
    const char *lvls[] = {
        [AOS_LL_NONE] = "N",
        [AOS_LL_FATAL] = "F",
        [AOS_LL_ERROR] = "E",
        [AOS_LL_WARN] = "W",
        [AOS_LL_INFO] = "I",
        [AOS_LL_DEBUG] = "D",
    };

    if (argc < 2)
    {
        aos_cli_printf("cur level: %02x\r\n", aos_get_log_level());
        return;
    }

    int i;
    for (i = 0; i < sizeof(lvls) / sizeof(lvls[0]); i++)
    {
        if (strncmp(lvls[i], argv[1], strlen(lvls[i]) + 1) != 0)
            continue;

        aos_set_log_level((aos_log_level_t)i);
        aos_cli_printf("set level success\r\n");
        return;
    }
    aos_cli_printf("set level fail\r\n");
}

struct cli_command log_cli_cmd[] = {
    {"lvl", "Set loglevel. lvl [N/F/E/W/I/D]", log_cmd}};

/* log init with cli */
void log_cli_init(void)
{
    aos_log_level = AOS_LL_V_DEBUG | AOS_LL_V_INFO | AOS_LL_V_WARN | AOS_LL_V_ERROR | AOS_LL_V_FATAL;
    aos_cli_register_commands(&log_cli_cmd[0], sizeof(log_cli_cmd) / sizeof(struct cli_command));
    aos_mutex_new(&log_mutex);
}

/* log init without cli */
void log_no_cli_init(void)
{
    aos_mutex_new(&log_mutex);
}

#ifdef AOS_DISABLE_ALL_LOGS
static char enable_aos_log_flag = 0;
extern int cli_service_start(void);

static int aos_uart_getchar(char *inbuf, int timeout)
{
    if (aos_uart_recv(inbuf, 1, NULL, timeout) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

char log_get_enable_aos_log_flag(void)
{
    return enable_aos_log_flag;
}

int log_check_uart_input(int timeout)
{
    int ch = 0;

    if (aos_uart_getchar((char*)&ch, timeout) == 1)
    {
        if ((0 != ch) && (isprint((int)ch) || iscntrl((int)ch)))
        {
            enable_aos_log_flag = 1;
            printf("%d\r\n", ch);
#ifdef CONFIG_AOS_CLI
            cli_service_start();
#endif
        }
    }

    return 0;
}
#endif
