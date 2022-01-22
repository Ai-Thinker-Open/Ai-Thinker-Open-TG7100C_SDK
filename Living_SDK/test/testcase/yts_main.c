/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#if defined(__GNUC__)
#include <sys/wait.h>
#endif

#include <aos/kernel.h>

#include "yunit.h"
#include "yts.h"

static int yts_argc;
static char **yts_argv;

extern void add_test(void);

int yts_get_args(const char ***argv)
{
    *argv = (const char **)yts_argv;
    return yts_argc;
}

void yts_run(int argc, char **argv)
{
    int ret = 0;

    yts_argc = argc;
    yts_argv = argv;

    aos_msleep(2 * 1000);

    yunit_test_init();

    add_test();

    if (argc > 1)
    {
        int i;
        for (i = 1; i < argc; i++)
        {
            yunit_test_suite_t *test_suite = yunit_get_test_suite(argv[i]);
            if (test_suite != NULL)
            {
                ret = yunit_run_test_suite(test_suite, 1);
                printf("suite:%s completed with %d\n", argv[i], ret);

                continue;
            }

            const char *suite_case = argv[i];
            char *test = strrchr(suite_case, ':');
            if (test != NULL)
            {
                *test++ = '\0';

                test_suite = yunit_get_test_suite(suite_case);
                yunit_test_case_t *test_case = yunit_get_test_case(test_suite, test);
                if (test_case != NULL)
                {
                    ret = yunit_run_test_case(test_suite, test_case);
                    printf("suite:%s completed with %d\n", argv[i], ret);
                }
                else
                {
                    printf("suite:%s not found\n", argv[i]);
                }
            }
        }
    }
    else
    {
        ret = yunit_test_run();
        printf("\nTests completed with return value %d\n", ret);
    }

    yunit_test_print_result();

    yunit_test_deinit();
}

int yts_run_suite_by_name(const char *suite_name, int test_case_times)
{
    int ret = -1;
    yunit_test_suite_t *test_suite = NULL;

    yunit_test_init();

    if (!strcmp(suite_name, "basic"))
    {
        test_basic();
    }
    else
    {
        add_test();

        test_suite = yunit_get_test_suite(suite_name);
        if (test_suite != NULL)
        {
            ret = yunit_run_test_suite(test_suite, test_case_times);
        }
        else
        {
            HAL_Printf("Test suite:%s not found\r\n", suite_name);
            return ret;
        }

        yunit_test_print_result();
    }

    yunit_test_deinit();

    return ret;
}

void ct_yts_run_all(void)
{
    int ret = 0;

    yunit_test_init();

    test_basic(); //Run basic

    add_test();

    ret = yunit_test_run(); //Run case which beyond of basic
    printf("\nTests completed with return value %d\n", ret);

    yunit_test_print_result();

    yunit_test_deinit();
}
