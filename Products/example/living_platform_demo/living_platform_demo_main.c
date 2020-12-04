/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "stdio.h"
#include "iot_export_linkkit.h"
#include "cJSON.h"
#include "app_entry.h"
#include "aos/kv.h"
#include "living_platform_demo_main.h"
#include "device_state_manger.h"


#define USER_EXAMPLE_YIELD_TIMEOUT_MS (30)

#define RESPONE_BUFFER_SIZE   128


static user_example_ctx_t g_user_example_ctx;

void user_post_property_json(const char *property);

user_example_ctx_t *user_example_get_ctx(void)
{
    return &g_user_example_ctx;
}

void *example_malloc(size_t size)
{
    return HAL_Malloc(size);
}

void example_free(void *ptr)
{
    HAL_Free(ptr);
}




static int user_connected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    LOG_TRACE("Cloud Connected");

    user_example_ctx->cloud_connected = 1;

    set_net_state(CONNECT_CLOUD_SUCCESS);

    return 0;
}

static int user_disconnected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    LOG_TRACE("Cloud Disconnected");

    set_net_state(CONNECT_CLOUD_FAILED);
    user_example_ctx->cloud_connected = 0;

    return 0;
}




static int property_setting_handle(const char *request, const int request_len)
{
    //解析处理
    LOG_TRACE("property set,  payload: \"%s\"", request);




    return 0;
}

static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len)
{
    cJSON *root = NULL;
    LOG_TRACE("Service Request Received, Devid: %d, Service ID: %.*s, Payload: %s", devid, serviceid_len, serviceid,
            request);

    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        LOG_TRACE("JSON Parse Error");
        return -1;
    }
    //Rhythm字符串请根据后台功能定义中的标识符来修改(异步调用)，若没有异步调用方式可以忽略此功能
    if (strlen("Rhythm") == serviceid_len && memcmp("Rhythm", serviceid, serviceid_len) == 0) {
        //解析处理




    }
    cJSON_Delete(root);
    return 0;
}


static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int ret = 0;

    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    printf("\n\n#####property set,  Devid: %d, payload: \"%s\"####\n\n", devid, request);
    property_setting_handle(request, request_len);

    //将获取到的数据上行回云端，可以自定义上行数据(下面上行数据只用做测试)
    IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
            (unsigned char *)request, request_len);
    return ret;
}

#ifdef ALCS_ENABLED
static int user_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
        int *response_len)
{
    int index = 0;
    cJSON *request_root = NULL, *item_propertyid = NULL;
    cJSON *response_root = NULL;

    LOG_TRACE("Property Get Received, Devid: %d, Request: %s", devid, request);
    request_root = cJSON_Parse(request);
    if (request_root == NULL || !cJSON_IsArray(request_root)) {
        LOG_TRACE("JSON Parse Error");
        return -1;
    }

    /* Prepare Response */
    response_root = cJSON_CreateObject();
    if (response_root == NULL)
    {
        cJSON_Delete(request_root);
        return -1;
    }

    for (index = 0; index < cJSON_GetArraySize(request_root); index++)
    {
        item_propertyid = cJSON_GetArrayItem(request_root, index);
        if (item_propertyid == NULL || !cJSON_IsString(item_propertyid))
        {
            cJSON_Delete(request_root);
            cJSON_Delete(response_root);
            return -1;
        }

        //根据实际产品定义，例如：获取开关状态
        // if (strcmp("powerstate", item_propertyid->valuestring) == 0) {
        //     cJSON_AddNumberToObject(response_root, "powerstate", １);
        // }
    }
    cJSON_Delete(request_root);

    *response = cJSON_PrintUnformatted(response_root);
    if (*response == NULL)
    {
        cJSON_Delete(response_root);
        return -1;
    }
    cJSON_Delete(response_root);
    *response_len = strlen(*response);


    return SUCCESS_RETURN;
}
#endif

static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    const char *reply_value = (reply == NULL) ? ("NULL") : (reply);
    const int reply_value_len = (reply_len == 0) ? (strlen("NULL")) : (reply_len);

    //LOG_TRACE("Message Post Reply Received, Devid: %d, Message ID: %d, Code: %d, Reply: %.*s", devid, msgid, code,
    //        reply_value_len, reply_value);
    return 0;
}

static int user_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
        const int eventid_len, const char *message, const int message_len)
{
    LOG_TRACE("Trigger Event Reply Received, Devid: %d, Message ID: %d, Code: %d, EventID: %.*s, Message: %.*s", devid,
            msgid, code, eventid_len, eventid, message_len, message);

    return 0;
}


static int user_initialized(const int devid)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    LOG_TRACE("Device Initialized, Devid: %d", devid);

    if (user_example_ctx->master_devid == devid) {
        user_example_ctx->master_initialized = 1;
    }

    return 0;
}

/** type:
 *
 * 0 - new firmware exist
 *
 */
// static int user_fota_event_handler(int type, const char *version)
// {
//     char buffer[128] = {0};
//     int buffer_length = 128;
//     user_example_ctx_t *user_example_ctx = user_example_get_ctx();

//     if (type == 0) {
//         LOG_TRACE("New Firmware Version: %s", version);

//         IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
//     }

//     return 0;
// }

static uint64_t user_update_sec(void)
{
    static uint64_t time_start_ms = 0;

    if (time_start_ms == 0) {
        time_start_ms = HAL_UptimeMs();
    }

    return (HAL_UptimeMs() - time_start_ms) / 1000;
}

void user_post_property_json(const char *property)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    int res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char *)property,
            strlen(property));

    LOG_TRACE("Property post Response: %s", property);
    return;
}

static int notify_msg_handle(const char *request, const int request_len)
{
    cJSON *request_root = NULL;
    cJSON *item = NULL;

    request_root = cJSON_Parse(request);
    if (request_root == NULL) {
        LOG_TRACE("JSON Parse Error");
        return -1;
    }

    item = cJSON_GetObjectItem(request_root, "identifier");
    if (item == NULL || !cJSON_IsString(item)) {
        cJSON_Delete(request_root);
        return -1;
    }
    if (!strcmp(item->valuestring, "awss.BindNotify")) {
        cJSON *value = cJSON_GetObjectItem(request_root, "value");
        if (value == NULL || !cJSON_IsObject(value)) {
            cJSON_Delete(request_root);
            return -1;
        }
        cJSON *op = cJSON_GetObjectItem(value, "Operation");
        if (op != NULL && cJSON_IsString(op)) {
            if (!strcmp(op->valuestring, "Bind")) {
                LOG_TRACE("Device Bind");
                vendor_device_bind();
            } else if (!strcmp(op->valuestring, "Unbind")) {
                LOG_TRACE("Device unBind");
                vendor_device_unbind();
            } else if (!strcmp(op->valuestring, "Reset")) {
                LOG_TRACE("Device reset");
                vendor_device_reset();
            }
        }
    }

    cJSON_Delete(request_root);
    return 0;
}

static int user_event_notify_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    LOG_TRACE("Event notify Received, Devid: %d, Request: %s", devid, request);

    notify_msg_handle(request, request_len);

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_EVENT_NOTIFY_REPLY,
            (unsigned char *)request, request_len);
    LOG_TRACE("Post Property Message ID: %d", res);

    return 0;
}

#if defined (CLOUD_OFFLINE_RESET)
static int user_offline_reset_handler(void)
{
    LOG_TRACE("callback user_offline_reset_handler called.");
    vendor_device_unbind();
}
#endif



static int user_master_dev_available(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    if (user_example_ctx->cloud_connected && user_example_ctx->master_initialized) {
        return 1;
    }

    return 0;
}

static int max_running_seconds = 0;
int linkkit_main(void *paras)
{
    uint64_t                        time_prev_sec = 0, time_now_sec = 0;
    uint64_t                        time_begin_sec = 0;
    int                             res = 0;
    iotx_linkkit_dev_meta_info_t    master_meta_info;
    user_example_ctx_t             *user_example_ctx = user_example_get_ctx();
#if defined(__UBUNTU_SDK_DEMO__)
    int                             argc = ((app_main_paras_t *) paras)->argc;
    char                          **argv = ((app_main_paras_t *) paras)->argv;

    if (argc > 1) {
        int tmp = atoi(argv[1]);

        if (tmp >= 60) {
            max_running_seconds = tmp;
            LOG_TRACE("set [max_running_seconds] = %d seconds\n", max_running_seconds);
        }
    }
#endif

#if !defined(WIFI_PROVISION_ENABLED) || !defined(BUILD_AOS)
    set_device_meta_info();
#endif

    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);       //与云端连接成功时回调
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);    //与云端连接断开时回调
    // IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, user_down_raw_data_arrived_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);//Linkkit收到服务（同步/异步）调用请求时回调
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);//Linkkit收到属性设置请求时回调
#ifdef ALCS_ENABLED
    /*Only for local communication service(ALCS) */
    IOT_RegisterCallback(ITE_PROPERTY_GET, user_property_get_event_handler);//Linkkit收到属性获取的请求时回调
#endif
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);//Linkkit收到上报消息的应答时回调
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);//Linkkit收到事件上报消息的应答时回调
    // IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);//当Linkkit收到查询时间戳请求的应答时回调
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);   //设备初始化完成时回调
    // IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);    //Linkkit收到可用固件的通知时回调
    IOT_RegisterCallback(ITE_EVENT_NOTIFY, user_event_notify_handler);
#if defined (CLOUD_OFFLINE_RESET)
    IOT_RegisterCallback(ITE_OFFLINE_RESET, user_offline_reset_handler);
#endif

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    HAL_GetProductKey(master_meta_info.product_key);
    HAL_GetDeviceName(master_meta_info.device_name);
    HAL_GetDeviceSecret(master_meta_info.device_secret);
    HAL_GetProductSecret(master_meta_info.product_secret);

    if ((0 == strlen(master_meta_info.product_key)) || (0 == strlen(master_meta_info.device_name))
            || (0 == strlen(master_meta_info.device_secret)) || (0 == strlen(master_meta_info.product_secret))) {
        LOG_TRACE("No device meta info found...\n");
        while (1) {
            aos_msleep(USER_EXAMPLE_YIELD_TIMEOUT_MS);
        }
    }

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Choose Whether You Need Post Property/Event Reply */
    int post_event_reply = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

    /* Create Master Device Resources */
    do {
        user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
        if (user_example_ctx->master_devid < 0) {
            LOG_TRACE("IOT_Linkkit_Open Failed, retry after 5s...\n");
            HAL_SleepMs(5000);
        }
    } while (user_example_ctx->master_devid < 0);
    /* Start Connect Aliyun Server */
    do {
        res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
        if (res < 0) {
            LOG_TRACE("IOT_Linkkit_Connect Failed, retry after 5s...\n");
            HAL_SleepMs(5000);
        }
    } while (res < 0);
    
    while (1) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);

        time_now_sec = user_update_sec();
        if (time_prev_sec == time_now_sec) {
            continue;
        }
        
        
        time_prev_sec = time_now_sec;
    }

    IOT_Linkkit_Close(user_example_ctx->master_devid);

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_SetLogLevel(IOT_LOG_NONE);

    return 0;
}
