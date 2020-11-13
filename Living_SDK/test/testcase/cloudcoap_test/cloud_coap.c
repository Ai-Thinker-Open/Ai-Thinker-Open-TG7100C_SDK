/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "iot_import.h"
#include "iot_export.h"

#include "cloud_coap.h"
#include "yunit.h"

static iotx_coap_context_t *p_coap_ctx = NULL;

static void iotx_response_handler(void *arg, void *p_response)
{
    int len = 0;
    int ret = -1;
    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;

    ret = IOT_CoAP_GetMessageCode(p_response, &resp_code);
    YUNIT_ASSERT(ret == 0);
    ret = IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    YUNIT_ASSERT(ret == 0);
}

static int iotx_post_data_to_server(iotx_device_info_t *device_info, void *param)
{
    int ret = SUCCESS_RETURN;
    char *path = NULL;
    iotx_message_t message;
    iotx_coap_context_t *p_ctx = (iotx_coap_context_t *)param;

    path = (char *)HAL_Malloc(IOTX_URI_MAX_LEN + 1);
    if (path == NULL)
    {
        YUNIT_ASSERT_MSG((path != NULL), "no mem")
        return FAIL_RETURN;
    }

    memset(path, 0, IOTX_URI_MAX_LEN + 1);

    message.p_payload = (unsigned char *)"{\"name\":\"hello world\"}";
    message.payload_len = strlen("{\"name\":\"hello world\"}");
    message.resp_callback = iotx_response_handler;
    message.msg_type = IOTX_MESSAGE_CON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;

    HAL_Snprintf(path, IOTX_URI_MAX_LEN, "/topic/sys/%s/%s/thing/deviceinfo/update/",
                 (char *)device_info->product_key, (char *)device_info->device_name);

    ret = IOT_CoAP_SendMessage(p_ctx, path, &message);

    if (path)
    {
        HAL_Free(path);
    }

    return ret;
}

static int cloudcoap_init(const char *env, const char *secure)
{
    int ret = SUCCESS_RETURN;
    int send_interval = 0;
    int post_count = 0;
    int success_count = 0;
    char *url = NULL;
    iotx_coap_config_t config;
    iotx_device_info_t deviceinfo;

    memset(&config, 0x00, sizeof(iotx_coap_config_t));

    url = (char *)HAL_Malloc(CLOUD_COAP_URL_MAX_LEN);
    if (url == NULL)
    {
        YUNIT_ASSERT_MSG((url != NULL), "no mem")

        return FAIL_RETURN;
    }
    memset(url, 0, CLOUD_COAP_URL_MAX_LEN);

    iotx_device_info_get(&deviceinfo);

    if (0 == strncmp(env, "pre", strlen("pre")))
    {
        if (0 == strncmp(secure, "dtls", strlen("dtls")))
        {
            config.p_url = IOTX_PRE_DTLS_SERVER_URI;
        }
        else if (0 == strncmp(secure, "psk", strlen("psk")))
        {
            config.p_url = IOTX_PRE_PSK_SERVER_URI;
        }
        else
        {
            config.p_url = IOTX_PRE_NOSEC_SERVER_URI;
        }
    }
    else if (0 == strncmp(env, "online", strlen("online")))
    {
        if (0 == strncmp(secure, "dtls", strlen("dtls")))
        {
            HAL_Snprintf(url, CLOUD_COAP_URL_MAX_LEN, IOTX_ONLINE_DTLS_SERVER_URL, deviceinfo.product_key);
            config.p_url = url;
        }
        else if (0 == strncmp(secure, "psk", strlen("psk")))
        {
            HAL_Snprintf(url, CLOUD_COAP_URL_MAX_LEN, IOTX_ONLINE_PSK_SERVER_URL, deviceinfo.product_key);
            config.p_url = url;
        }
        else
        {
            ret = FAIL_RETURN;
            YUNIT_ASSERT_MSG((ret == SUCCESS_RETURN), "Online env must access with DTLS/PSK")
            goto ERROR;
        }
    }
    else if (0 == strncmp(env, "daily", strlen("daily")))
    {
        if (0 == strncmp(secure, "dtls", strlen("dtls")))
        {
            config.p_url = IOTX_DAILY_DTLS_SERVER_URI;
        }
        else if (0 == strncmp(secure, "psk", strlen("psk")))
        {
            config.p_url = IOTX_DAILY_PSK_SERVER_URI;
        }
        else
        {
            ret = FAIL_RETURN;
            YUNIT_ASSERT_MSG((ret == SUCCESS_RETURN), "Daily env must access with DTLS/PSK")
            goto ERROR;
        }
    }
    else
    {
        ret = FAIL_RETURN;
        YUNIT_ASSERT_MSG((ret == SUCCESS_RETURN), "Params error")
        goto ERROR;
    }

    config.p_devinfo = (iotx_device_info_t *)&deviceinfo;
    config.wait_time_ms = CLOUD_COAP_CONNECT_TIMEOUT;

    p_coap_ctx = IOT_CoAP_Init(&config);
    YUNIT_ASSERT(p_coap_ctx != NULL);

ERROR:
    if (url)
    {
        HAL_Free(url);
    }

    return ret;
}

static int init(void)
{
    int ret = -1;

    ret = cloudcoap_init(CLOUD_COAP_DEFAULT_ENV, CLOUD_COAP_DEFAULT_SECURE_TYPE);
    YUNIT_ASSERT(ret == SUCCESS_RETURN);

    return 0;
}

static int cleanup(void)
{
    YUNIT_ASSERT(p_coap_ctx != NULL);
    IOT_CoAP_Deinit(&p_coap_ctx);

    //Here sleep 1000ms or else init coap immediately will fail
    HAL_SleepMs(1000);
    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}

static void cloudcoap_auth_case(void)
{
    int ret = -1;

    YUNIT_ASSERT(p_coap_ctx != NULL);
    ret = IOT_CoAP_DeviceNameAuth(p_coap_ctx);
    YUNIT_ASSERT(ret == 0);
}

static void cloudcoap_request_case(void)
{
    int ret = -1;
    iotx_device_info_t device_info;

    memset(&device_info, 0, sizeof(iotx_device_info_t));

    ret = iotx_device_info_get(&device_info);
    YUNIT_ASSERT(ret == 0);

    ret = iotx_post_data_to_server(&device_info, p_coap_ctx);
    YUNIT_ASSERT(ret == 0);
    ret = IOT_CoAP_Yield(p_coap_ctx);
    YUNIT_ASSERT(ret == 0);
    HAL_SleepMs(CLOUD_COAP_YIELD_TIMEOUT);
}

static yunit_test_case_t cloudcoap_testcases[] = {
    {"auth", cloudcoap_auth_case},
    {"request", cloudcoap_request_case},
    YUNIT_TEST_CASE_NULL};

static yunit_test_suite_t suites[] = {
    {"cloudcoap", init, cleanup, setup, teardown, cloudcoap_testcases},
    YUNIT_TEST_SUITE_NULL};

void test_cloudcoap(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_cloudcoap);
