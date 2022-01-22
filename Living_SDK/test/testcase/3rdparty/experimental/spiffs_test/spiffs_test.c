/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <aos/aos.h>
#include <yunit.h>
#include <yts.h>
#include <aos_spiffs.h>

static const char *g_string         = "spiffs test string.";
static const char *g_filepath       = "/spiffs/test.txt";
static const char *g_dirpath        = "/spiffs/testDir";
static const char *g_dirtest_1      = "/spiffs/testDir/test_1.txt";
static const char *g_dirtest_2      = "/spiffs/testDir/test_2.txt";
static const char *g_dirtest_3      = "/spiffs/testDir/test_3.txt";
static const char *g_new_filepath   = "/spiffs/testDir/newname.txt";

static void test_spiffs_case(void)
{
    int ret, fd;
    char readBuffer[32];
    aos_dir_t *dp;

    /* spiffs write test */
    fd = aos_open(g_filepath, O_RDWR | O_CREAT | O_TRUNC);
    YUNIT_ASSERT(fd > 0);
 
    if (fd > 0) {
        ret = aos_write(fd, g_string, strlen(g_string));
        YUNIT_ASSERT(ret > 0);
        ret = aos_sync(fd);
        YUNIT_ASSERT(ret == 0);

        aos_close(fd);
    }

    /* spiffs read test */
    fd = aos_open(g_filepath, O_RDONLY);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0) {
        ret = aos_read(fd, readBuffer, sizeof(readBuffer));
        YUNIT_ASSERT(ret > 0);

        ret = memcmp(readBuffer, g_string, strlen(g_string));
        YUNIT_ASSERT(ret == 0);

        aos_close(fd);      
    }

    /* spiffs readdir test */
    fd = aos_open(g_dirtest_1, O_RDWR | O_CREAT | O_TRUNC);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0)
        aos_close(fd);

    fd = aos_open(g_dirtest_2, O_RDWR | O_CREAT | O_TRUNC);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0)
        aos_close(fd);

    fd = aos_open(g_dirtest_3, O_RDWR | O_CREAT | O_TRUNC);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0)
        aos_close(fd);

    dp = (aos_dir_t *)aos_opendir(g_dirpath);
    YUNIT_ASSERT(dp != NULL);

    if (dp) {
        aos_dirent_t *out_dirent;
        while(1) {
            out_dirent = (aos_dirent_t *)aos_readdir(dp);
            if (out_dirent == NULL)
                break;

            printf("file name is %s\n", out_dirent->d_name);            
        }
    }
    aos_closedir(dp);

    /* spiffs rename test */
    ret = aos_rename(g_filepath, g_new_filepath);
    YUNIT_ASSERT(ret == 0);

    fd = aos_open(g_filepath, O_RDONLY);
    YUNIT_ASSERT(fd < 0);
    if (fd >= 0)
        aos_close(fd);

    fd = aos_open(g_new_filepath, O_RDONLY);
    YUNIT_ASSERT(fd > 0);
    if (fd > 0)
        aos_close(fd);

    /* spiffs unlink test */
    ret = aos_unlink(g_new_filepath);
    YUNIT_ASSERT(ret == 0);

    fd = aos_open(g_new_filepath, O_RDONLY);
    YUNIT_ASSERT(fd < 0);
    if (fd > 0)
        aos_close(fd);
}

static int init(void)
{
    int ret = 0;

    ret = vfs_spiffs_register();
    YUNIT_ASSERT(ret == 0);
    return 0;
}

static int cleanup(void)
{
    int ret = vfs_spiffs_unregister();
    YUNIT_ASSERT(ret == 0);
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_spiffs_testcases[] = {
    { "spiffs_test", test_spiffs_case},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "spiffs", init, cleanup, setup, teardown, aos_spiffs_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_spiffs(void)
{    
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_spiffs);

