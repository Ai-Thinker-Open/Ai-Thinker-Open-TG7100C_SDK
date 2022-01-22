/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "iot_export.h"
#include "utils_httpc.h"

#include "http.h"
#include "yunit.h"

static char request_buf[MAX_BUF_LEN];
static char response_buf[MAX_BUF_LEN];

static void *http_handle = NULL;

static int http_do_request(iotx_device_info_t *pdev, void *handle)
{
    int ret = -1;
    char *path = NULL;
    iotx_http_message_param_t msg;

    if (NULL == pdev || NULL == handle)
    {
        return -1;
    }

    path = (char *)HAL_Malloc(IOTX_URI_MAX_LEN + 1);
    if (path == NULL)
    {
        YUNIT_ASSERT_MSG((path != NULL), "no mem")
        return FAIL_RETURN;
    }

    HAL_Snprintf(request_buf, MAX_BUF_LEN, "{\"name\":\"hello world\"}");
    memset(response_buf, 0x00, MAX_BUF_LEN);
    HAL_Snprintf(path, IOTX_URI_MAX_LEN, "/topic/%s/%s/data",
                 pdev->product_key, pdev->device_name);

    msg.request_payload = request_buf;
    msg.response_payload = response_buf;
    msg.timeout_ms = DEFAULT_TIMEOUT_MS;
    msg.request_payload_len = strlen(msg.request_payload) + 1;
    msg.response_payload_len = MAX_BUF_LEN;
    msg.topic_path = path;

    ret = IOT_HTTP_SendMessage(handle, &msg);
    if (path)
    {
        HAL_Free(path);
    }

    YUNIT_ASSERT(ret == 0);

    return 0;
}

static int init(void)
{
    iotx_device_info_t device_info;
    iotx_http_param_t http_param;

    memset(&http_param, 0, sizeof(http_param));

    iotx_device_info_get(&device_info);

    http_param.device_info = &device_info;
    http_param.timeout_ms = DEFAULT_TIMEOUT_MS;

    http_handle = IOT_HTTP_Init(&http_param);
    YUNIT_ASSERT(http_handle != NULL);

    return 0;
}

static int cleanup(void)
{
    IOT_HTTP_Disconnect(http_handle);
    IOT_HTTP_DeInit(&http_handle);

    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}

static void http_auth_case(void)
{
    int ret = -1;

    ret = IOT_HTTP_DeviceNameAuth(http_handle);
    YUNIT_ASSERT(ret == 0);
}

static void http_request_case(void)
{
    int ret = -1;
    iotx_device_info_t device_info;

    memset(&device_info, 0, sizeof(iotx_device_info_t));

    iotx_device_info_get(&device_info);

    ret = http_do_request(&device_info, http_handle);

    YUNIT_ASSERT(ret == 0);
}

static yunit_test_case_t http_testcases[] = {
    {"auth", http_auth_case},
    {"request", http_request_case},
    YUNIT_TEST_CASE_NULL};

static yunit_test_suite_t suites[] = {
    {"http", init, cleanup, setup, teardown, http_testcases},
    YUNIT_TEST_SUITE_NULL};

void test_http(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_http);
