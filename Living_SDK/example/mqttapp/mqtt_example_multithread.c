/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export.h"
#include "app_entry.h"


#define PRODUCT_KEY             "a1MZxOdcBnO"
#define DEVICE_NAME             "test_02"
#define DEVICE_SECRET           "kaavRFuVDjbeNOrl80EXle0gmymTawWA"


/* These are pre-defined topics */
#define TOPIC_UPDATE            "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"PRODUCT_KEY"/"DEVICE_NAME"/get"
#define TOPIC_DATA              "/"PRODUCT_KEY"/"DEVICE_NAME"/data"

#define MQTT_MSGLEN             (1024)

static void *g_thread_yield = NULL;
static void *g_thread_sub_unsub_1 = NULL;
static void *g_thread_sub_unsub_2 = NULL;
static void *g_thread_pub_1 = NULL;
static void *g_thread_pub_2 = NULL;

static int g_thread_yield_running = 1;
static int g_thread_sub_unsub_1_running = 1;
static int g_thread_sub_unsub_2_running = 1;
static int g_thread_pub_1_running = 1;
static int g_thread_pub_2_running = 1;

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)


void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            EXAMPLE_TRACE("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            EXAMPLE_TRACE("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            EXAMPLE_TRACE("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            EXAMPLE_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            EXAMPLE_TRACE("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            EXAMPLE_TRACE("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            EXAMPLE_TRACE("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            EXAMPLE_TRACE("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            EXAMPLE_TRACE("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        default:
            EXAMPLE_TRACE("Should NOT arrive here.");
            break;
    }
}

static void _demo_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    /* print topic name and topic message */
    EXAMPLE_TRACE("----");
    EXAMPLE_TRACE("Topic: '%.*s' (Length: %d)",
                  ptopic_info->topic_len,
                  ptopic_info->ptopic,
                  ptopic_info->topic_len);
    EXAMPLE_TRACE("Payload: '%.*s' (Length: %d)",
                  ptopic_info->payload_len,
                  ptopic_info->payload,
                  ptopic_info->payload_len);
    EXAMPLE_TRACE("----");
}

void *thread_subscribe1(void *pclient)
{
    int     ret = -1;

    while (g_thread_sub_unsub_1_running) {
        HAL_SleepMs(100);
        ret = IOT_MQTT_Subscribe(pclient, TOPIC_DATA, IOTX_MQTT_QOS1, _demo_message_arrive, NULL);
        if (ret < 0) {
            EXAMPLE_TRACE("subscribe error");
            return NULL;
        }
        HAL_SleepMs(20);
        ret = IOT_MQTT_Unsubscribe(pclient, TOPIC_GET);
        if (ret < 0) {
            EXAMPLE_TRACE("subscribe error");
            return NULL;
        }
        HAL_SleepMs(100);
    }
    return NULL;
}

void *thread_subscribe2(void *pclient)
{
    int     ret = -1;

    while (g_thread_sub_unsub_2_running) {
        HAL_SleepMs(300);
        ret = IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
        if (ret < 0) {
            EXAMPLE_TRACE("subscribe error");
            return NULL;
        }
        HAL_SleepMs(30);
        ret = IOT_MQTT_Subscribe_Sync(pclient, TOPIC_GET, IOTX_MQTT_QOS1, _demo_message_arrive, NULL, 500);
        if (ret < 0) {
            EXAMPLE_TRACE("subscribe error");
            return NULL;
        }
        HAL_SleepMs(30);
    }
    return NULL;
}

// subscribe
void CASE2(void *pclient)
{
    int   ret = -1;

    if (pclient == NULL) {
        EXAMPLE_TRACE("param error");
        return;
    }
    int stack_used = 0;
    hal_os_thread_param_t task_parms1 = {0};
    task_parms1.stack_size = 4096;
    task_parms1.name = "thread_subscribe1";
    ret = HAL_ThreadCreate(&g_thread_sub_unsub_1, thread_subscribe1, (void *)pclient, &task_parms1, &stack_used);
    if (ret != 0) {
        EXAMPLE_TRACE("Thread created failed!\n");
        return;
    }

    hal_os_thread_param_t task_parms2 = {0};
    task_parms2.stack_size = 4096;
    task_parms2.name = "thread_subscribe2";
    ret = HAL_ThreadCreate(&g_thread_sub_unsub_2, thread_subscribe2, (void *)pclient, &task_parms2, &stack_used);
    if (ret != 0) {
        EXAMPLE_TRACE("Thread created failed!\n");
        return;
    }
}

void *thread_publish1(void *pclient)
{
    int         ret = -1;
    char        msg_pub[MQTT_MSGLEN] = {0};

    iotx_mqtt_topic_info_t topic_msg;

    strcpy(msg_pub, "thread_publish1 message: hello! start!");
    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg_pub;
    topic_msg.payload_len = strlen(msg_pub);

    while (g_thread_pub_1_running) {
        ret = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
        EXAMPLE_TRACE("publish thread 1:ret = %d\n", ret);
        HAL_SleepMs(300);
    }

    return NULL;
}

void *thread_publish2(void *pclient)
{
    int         ret = -1;
    char        msg_pub[MQTT_MSGLEN] = {0};
    iotx_mqtt_topic_info_t topic_msg;

    strcpy(msg_pub, "thread_publish2 message: hello! start!");
    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg_pub;
    topic_msg.payload_len = strlen(msg_pub);

    while (g_thread_pub_2_running) {
        ret = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
        EXAMPLE_TRACE("publish thread 2:ret = %d\n", ret);
        HAL_SleepMs(200);
    }

    return NULL;
}

// publish
void CASE1(void *pclient)
{
    int   ret = -1;

    if (pclient == NULL) {
        EXAMPLE_TRACE("param error");
        return;
    }

    ret = IOT_MQTT_Subscribe(pclient, TOPIC_DATA, IOTX_MQTT_QOS1, _demo_message_arrive, NULL);
    if (ret < 0) {
        EXAMPLE_TRACE("subscribe error");
        return;
    }
    int stack_used = 0;
    hal_os_thread_param_t task_parms1 = {0};
    task_parms1.stack_size = 4096;
    task_parms1.name = "thread_publish1";
    ret = HAL_ThreadCreate(&g_thread_pub_1, thread_publish1, (void *)pclient, &task_parms1, &stack_used);
    if (ret != 0) {
        EXAMPLE_TRACE("Thread created failed!\n");
        return;
    }

    hal_os_thread_param_t task_parms2 = {0};
    task_parms2.stack_size = 4096;
    task_parms2.name = "thread_publish2";
    ret = HAL_ThreadCreate(&g_thread_pub_2, thread_publish2, (void *)pclient, &task_parms2, &stack_used);
    if (ret != 0) {
        EXAMPLE_TRACE("Thread created failed!\n");
        return;
    }
}

// yield thread
void *thread_yield(void *pclient)
{
    while (g_thread_yield_running) {
        IOT_MQTT_Yield(pclient, 200);

        HAL_SleepMs(200);
    }

    return NULL;
}

static uint64_t user_update_sec(void)
{
    static uint64_t time_start_ms = 0;

    if (time_start_ms == 0) {
        time_start_ms = HAL_UptimeMs();
    }

    return (HAL_UptimeMs() - time_start_ms) / 1000;
}

int mqtt_client(void *params)
{
    int rc = 0;//, msg_len, cnt = 0;
    void *pclient;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;
    uint64_t max_running_seconds = 30;

#if defined(__UBUNTU_SDK_DEMO__)
    int                             argc = ((app_main_paras_t *)params)->argc;
    char                          **argv = ((app_main_paras_t *)params)->argv;

    if (argc > 1) {
        int     tmp = atoi(argv[1]);

        if (tmp >= 60) {
            max_running_seconds = tmp;
            EXAMPLE_TRACE("set [max_running_seconds] = %d seconds\n", max_running_seconds);
        }
    }
#endif
    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        return -1;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = MQTT_MSGLEN;
    mqtt_params.write_buf_size = MQTT_MSGLEN;

    mqtt_params.handle_event.h_fp = event_handle;
    mqtt_params.handle_event.pcontext = NULL;


    /* Construct a MQTT client with specify parameter */
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return -1;
    }

    EXAMPLE_TRACE("TEST CASE");
    int stack_used = 0;

    hal_os_thread_param_t task_parms = {0};
    task_parms.stack_size = 6144;
    task_parms.name = "thread_yield";
    rc = HAL_ThreadCreate(&g_thread_yield, thread_yield, (void *)pclient, &task_parms, &stack_used);
    if (rc != 0) {
        goto do_exit;
    }

    // mutli thread publish
    CASE1(pclient);

    // mutli thread subscribe
    CASE2(pclient);

    while (1) {
        if (user_update_sec() > max_running_seconds) {
            break;
        }
        HAL_SleepMs(1000);
    }

    g_thread_sub_unsub_1_running = 0;
    g_thread_sub_unsub_2_running = 0;
    g_thread_pub_1_running = 0;
    g_thread_pub_2_running = 0;
    g_thread_yield_running = 0;
    HAL_SleepMs(5000);
    
do_exit:

    HAL_ThreadDelete(g_thread_sub_unsub_1);
    HAL_ThreadDelete(g_thread_sub_unsub_2);
    HAL_ThreadDelete(g_thread_pub_1);
    HAL_ThreadDelete(g_thread_pub_2);
    HAL_ThreadDelete(g_thread_yield);
    IOT_MQTT_Destroy(&pclient);
    return rc;
}


int linkkit_main(void *params)
{
    IOT_SetLogLevel(IOT_LOG_DEBUG);
    /**< set device info*/
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
    /**< end*/
    mqtt_client(params);
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_SetLogLevel(IOT_LOG_NONE);

    EXAMPLE_TRACE("out of sample!");

    return 0;
}


