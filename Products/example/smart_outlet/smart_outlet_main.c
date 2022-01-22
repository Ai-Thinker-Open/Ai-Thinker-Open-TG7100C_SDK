/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "stdio.h"
#include "iot_export_linkkit.h"
#include "cJSON.h"
#include "app_entry.h"
#include "aos/kv.h"
#if defined(AOS_TIMER_SERVICE)||defined(AIOT_DEVICE_TIMER_ENABLE)
#include "iot_export_timer.h"
#endif
#include "smart_outlet.h"
#include "vendor.h"
#include "device_state_manger.h"
#include "msg_process_center.h"
#include "property_report.h"

#ifdef EN_COMBO_NET
#include "combo_net.h"
#include "breeze_export.h"
#endif

#ifdef CERTIFICATION_TEST_MODE
#include "certification/ct_ut.h"
#endif

#define USER_EXAMPLE_YIELD_TIMEOUT_MS (30)

#define RESPONE_BUFFER_SIZE   128

#ifdef AIOT_DEVICE_TIMER_ENABLE
    //#define MULTI_ELEMENT_TEST
    #ifndef MULTI_ELEMENT_TEST
        #define NUM_OF_TIMER_PROPERTYS 3 /*  */
        const char *propertys_list[NUM_OF_TIMER_PROPERTYS] = { "PowerSwitch", "powerstate", "allPowerstate" };
    #else // only for test
        #define NUM_OF_TIMER_PROPERTYS 14 /*  */
        // const char *propertys_list[NUM_OF_TIMER_PROPERTYS] = { "testEnum", "testFloat", "testInt", "powerstate", "allPowerstate" };
        const char *propertys_list[NUM_OF_TIMER_PROPERTYS] = { "powerstate", "allPowerstate", "mode", "powerstate_1", "brightness", "PowerSwitch", "powerstate_2",
                    "powerstate_3", "heaterPower", "windspeed", "angleLR", "testEnum", "testFloat", "testInt" };
        typedef enum {
            T_INT = 1,
            T_FLOAT,
            T_STRING,
            T_STRUCT,
            T_ARRAY,
        }  data_type_t;
        const data_type_t propertys_type[NUM_OF_TIMER_PROPERTYS] = { T_INT,T_INT,T_INT,T_INT,T_INT,T_INT,T_INT,T_INT,T_FLOAT,T_INT,T_INT,T_INT,T_FLOAT,T_INT };

        static int propertys_handle(cJSON *root) {
            cJSON *item = NULL;
            int ret = -1, i = 0;

            for (i = 0; i < NUM_OF_TIMER_PROPERTYS; i++) {
                if (propertys_type[i] == T_STRUCT && (item = cJSON_GetObjectItem(root, propertys_list[i])) != NULL && cJSON_IsObject(item)) { //structs
                    printf(" %s\r\n", propertys_list[i]);
                    ret = 0;
                } else if (propertys_type[i] == T_FLOAT && (item = cJSON_GetObjectItem(root, propertys_list[i])) != NULL && cJSON_IsNumber(item)){ // float
                    printf(" %s %f\r\n", propertys_list[i], item->valuedouble);
                    ret = 0;
                } else if (propertys_type[i] == T_INT &&(item = cJSON_GetObjectItem(root, propertys_list[i])) != NULL && cJSON_IsNumber(item)){ // int
                    printf(" %s %d\r\n", propertys_list[i], item->valueint);
                    ret = 0;
                } else if (propertys_type[i] == T_STRING &&(item = cJSON_GetObjectItem(root, propertys_list[i])) != NULL && cJSON_IsString(item)){ // string
                    printf(" %s %s\r\n", propertys_list[i], item->valuestring);
                    ret = 0;
                } else if (propertys_type[i] == T_ARRAY &&(item = cJSON_GetObjectItem(root, propertys_list[i])) != NULL && cJSON_IsArray(item)){ // array
                    printf(" %s \r\n", propertys_list[i]);
                    ret = 0;
                }
            }

            return ret;
        }
    #endif
static void timer_service_cb(const char *report_data, const char *property_name, const char *data)
{
    uint8_t value = 0;

    // if (report_data != NULL)	/* post property to cloud */
    //     user_post_property_json(report_data);
    if (property_name != NULL) {	/* set value to device */
        LOG_TRACE("timer event callback=%s val=%s", property_name, data);
        #ifdef MULTI_ELEMENT_TEST
    	char property_payload[128] = {0};
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        if (strcmp(propertys_list[0], property_name) != 0 && strcmp(propertys_list[1], property_name) != 0) {
            snprintf(property_payload, sizeof(property_payload), "{\"%s\":%s}", property_name, data);
            IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                    property_payload, strlen(property_payload));
            return;
        }
        else
        #endif
        {
            // data is int; convert it.
            value = (uint8_t)atoi(data);
        }
        recv_msg_t msg;
        msg.powerswitch = value;
        msg.all_powerstate = value;
        msg.flag = 0x00;
        strcpy(msg.seq, SPEC_SEQ);
        send_msg_to_queue(&msg);
    }

    return;
}
#endif

#ifdef AOS_TIMER_SERVICE
    #define NUM_OF_PROPERTYS 3 /* <=30 dont add timer property */
    const char *control_targets_list[NUM_OF_PROPERTYS] = { "PowerSwitch", "powerstate","allPowerstate" };
    static int num_of_tsl_type[NUM_OF_TSL_TYPES] = { 3, 0, 0 }; /* 1:int/enum/bool; 2:float/double; 3:text/date */

    #define NUM_OF_COUNTDOWN_LIST_TARGET 3 /* <=10 */
    const char *countdownlist_target_list[NUM_OF_COUNTDOWN_LIST_TARGET] = { "PowerSwitch", "powerstate","allPowerstate" };

    #define NUM_OF_LOCAL_TIMER_TARGET 1 /* <=5 */
    const char *localtimer_target_list[NUM_OF_LOCAL_TIMER_TARGET] = { "powerstate" };
    static bool ntp_update = false;

static void timer_service_cb(const char *report_data, const char *property_name, int i_value,
        double d_value, const char *s_value, int prop_idx)
{
    if (prop_idx >= NUM_OF_CONTROL_TARGETS) {
        LOG_TRACE("ERROR: prop_idx=%d is out of limit=%d", prop_idx, NUM_OF_CONTROL_TARGETS);
    }
    if (report_data != NULL)	/* post property to cloud */
        user_post_property_json(report_data);
    if (property_name != NULL) {	/* set value to device */
        LOG_TRACE("timer event callback: property_name=%s prop_idx=%d", property_name, prop_idx);
        if ((prop_idx < num_of_tsl_type[0] && strcmp(control_targets_list[0], property_name) == 0)
            ||(prop_idx < num_of_tsl_type[0] && strcmp(control_targets_list[1], property_name) == 0)) {
            recv_msg_t msg;

            msg.powerswitch = i_value;
            msg.all_powerstate = i_value;
            msg.flag = 0x00;
            strcpy(msg.seq, SPEC_SEQ);
            send_msg_to_queue(&msg);
            LOG_TRACE("timer event callback: int_value=%d", i_value);
            /* set int value */
        } else if (prop_idx < num_of_tsl_type[0] + num_of_tsl_type[1]) {
            LOG_TRACE("timer event callback: double_value=%f", d_value);
            /* set doube value */
        } else {
            if (s_value != NULL)
                LOG_TRACE("timer event callback: test_value=%s", s_value);
            /* set string value */
        }

    }

    return;
}
#endif

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

void update_power_state(int state)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    user_example_ctx->status.powerswitch = state;
    user_example_ctx->status.all_powerstate = state;
}

static void user_deviceinfo_update(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *device_info_update = "[{\"attrKey\":\"OutletFWVersion\",\"attrValue\":\"smo_1.5.1\"}]";

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_DEVICEINFO_UPDATE,
            (unsigned char *)device_info_update, strlen(device_info_update));
    LOG_TRACE("Device Info Update Message ID: %d", res);
}

void user_post_property_after_connected(void);

#if (defined (TG7100CEVB))
extern int wifi_mgmr_sta_powersaving(int ps);
#endif
static int user_connected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    LOG_TRACE("Cloud Connected");
#if (defined (TG7100CEVB))
        wifi_mgmr_sta_powersaving(2);
#endif
    user_example_ctx->cloud_connected = 1;
#ifdef EN_COMBO_NET
    aiot_ais_report_awss_status(AIS_AWSS_STATUS_PROGRESS_REPORT, AIS_AWSS_PROGRESS_CONNECT_MQTT_SUCCESS);
#endif
    set_net_state(CONNECT_CLOUD_SUCCESS);
    user_post_property_after_connected();

    user_deviceinfo_update();

    return 0;
}

static int user_disconnected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    LOG_TRACE("Cloud Disconnected");
#if (defined (TG7100CEVB))
        wifi_mgmr_sta_powersaving(0);
#endif
    set_net_state(CONNECT_CLOUD_FAILED);
    user_example_ctx->cloud_connected = 0;

    return 0;
}

#define MAX_USER    5

struct Seq_Info {
    char userId[8];
    uint64_t timeStampe;
    int isValid;
};

static struct Seq_Info seqence_info[MAX_USER];

static int property_seq_handle(const char *seqenceId, recv_msg_t * msg)
{
    char userId[8], timeStr[16], *ptr;
    int isDiscard = 1, i;
    static int repalce_index = 0;

    if (strlen(seqenceId) > sizeof(msg->seq)) {
        LOG_TRACE("seq to long !!!");
        return -1;
    }
    if (NULL != (ptr = strchr(seqenceId, '@'))) {
        int len = ptr - seqenceId;

        if (len > (sizeof(userId) -1))
            len = sizeof(userId) -1;
        memset(userId, 0, sizeof(userId));
        memcpy(userId, seqenceId, len);

        len = strlen(seqenceId) - strlen(userId) - 1;
        if (len > (sizeof(timeStr) -1))
            len = sizeof(timeStr) -1;
        memset(timeStr, 0, sizeof(timeStr));
        memcpy(timeStr, ptr + 1, len);

        uint64_t time = atoll(timeStr);
        for (i = 0; i < MAX_USER; i++) {
            if (!strcmp(userId, seqence_info[i].userId)) {
                if (time >= seqence_info[i].timeStampe) {
                    isDiscard = 0;
                    seqence_info[i].timeStampe = time;
                }
                break;
            }
        }
        if (i == MAX_USER) {
            for (i = 0; i < MAX_USER; i++) {
                if (!seqence_info[i].isValid) {
                    strcpy(seqence_info[i].userId, userId);
                    seqence_info[i].timeStampe = time;
                    isDiscard = 0;
                    seqence_info[i].isValid = 1;
                    LOG_TRACE("new user %s", userId);
                    break;
                }
            }
            if (i == MAX_USER) {
                strcpy(seqence_info[repalce_index].userId, userId);
                seqence_info[repalce_index].timeStampe = time;
                isDiscard = 0;
                seqence_info[repalce_index].isValid = 1;
                repalce_index = (++repalce_index) % MAX_USER;
                LOG_TRACE("replace new user %s index %d", userId, repalce_index);
            }
        }
    }
    if (isDiscard == 1) {
        LOG_TRACE("Discard msg !!!");
        return -1;
    }
    if (NULL != seqenceId)
        strncpy(msg->seq, seqenceId, sizeof(msg->seq) - 1);

    return 0;
}

static int property_setting_handle(const char *request, const int request_len, recv_msg_t * msg)
{
    cJSON *root = NULL, *item = NULL;
    int ret = -1;

    if ((root = cJSON_Parse(request)) == NULL) {
        LOG_TRACE("property set payload is not JSON format");
        return -1;
    }

    if ((item = cJSON_GetObjectItem(root, "setPropsExtends")) != NULL && cJSON_IsObject(item)) {
        int isDiscard = 0;
        cJSON *seq = NULL, *flag = NULL;

        if ((seq = cJSON_GetObjectItem(item, "seq")) != NULL && cJSON_IsString(seq)) {
            if (property_seq_handle(seq->valuestring, msg)) {
                isDiscard = 1;
            }
        }
        if (isDiscard == 0) {
            if ((flag = cJSON_GetObjectItem(item, "flag")) != NULL && cJSON_IsNumber(flag)) {
                msg->flag = flag->valueint;
            } else {
                msg->flag = 0;
            }
        } else {
            cJSON_Delete(root);
            return 0;
        }
    }
    if ((item = cJSON_GetObjectItem(root, "powerstate")) != NULL && cJSON_IsNumber(item)) {
        msg->powerswitch = item->valueint;
        msg->all_powerstate = msg->powerswitch;
        ret = 0;
    }
#ifdef TSL_FY_SUPPORT /* support old feiyan TSL */
    else if ((item = cJSON_GetObjectItem(root, "PowerSwitch")) != NULL && cJSON_IsNumber(item)) {
        msg->powerswitch = item->valueint;
        ret = 0;
    }
#endif
    else if ((item = cJSON_GetObjectItem(root, "allPowerstate")) != NULL && cJSON_IsNumber(item)) {
        msg->powerswitch = item->valueint;
        msg->all_powerstate = msg->powerswitch;
        ret = 0;
    }
#ifdef AOS_TIMER_SERVICE
    else if (((item = cJSON_GetObjectItem(root, "LocalTimer")) != NULL && cJSON_IsArray(item)) || \
        ((item = cJSON_GetObjectItem(root, "CountDownList")) != NULL && cJSON_IsObject(item)) || \
        ((item = cJSON_GetObjectItem(root, "PeriodTimer")) != NULL && cJSON_IsObject(item)) || \
        ((item = cJSON_GetObjectItem(root, "RandomTimer")) != NULL && cJSON_IsObject(item)))
    {
        // Before LocalTimer Handle, Free Memory
        cJSON_Delete(root);
        timer_service_property_set(request);
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                (unsigned char *)request, request_len);
        return 0;
    }
#endif
#ifdef AIOT_DEVICE_TIMER_ENABLE
    else if ((item = cJSON_GetObjectItem(root, DEVICETIMER)) != NULL && cJSON_IsArray(item))
    {
        // Before LocalTimer Handle, Free Memory
        cJSON_Delete(root);
        ret = deviceTimerParse(request, 0, 1);
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        if (ret == 0) {
            IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                (unsigned char *)request, request_len);
        } else {
            char *report_fail = "{\"DeviceTimer\":[]}";
            IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                    (unsigned char *)report_fail, strlen(report_fail));
            ret = -1;
        }
        // char *property = device_timer_post(1);
        // if (property != NULL)
        //     HAL_Free(property);
        return 0;
    }
    #ifdef MULTI_ELEMENT_TEST
    else if (propertys_handle(root) >= 0) {
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        cJSON_Delete(root);
        IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                (unsigned char *)request, request_len);
        return 0;
    }
    #endif
#endif
    else {
        LOG_TRACE("property set payload is not JSON format");
        ret = -1;
    }

    cJSON_Delete(root);
    if (ret != -1)
        send_msg_to_queue(msg);

    return ret;
}

static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len)
{
    cJSON *root = NULL;

#ifdef CERTIFICATION_TEST_MODE
    return ct_main_service_request_event_handler(devid, serviceid, serviceid_len, request, request_len, response, response_len);
#endif

    LOG_TRACE("Service Request Received, Devid: %d, Service ID: %.*s, Payload: %s", devid, serviceid_len, serviceid,
            request);

    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        LOG_TRACE("JSON Parse Error");
        return -1;
    }
    if (strlen("CommonService") == serviceid_len && memcmp("CommonService", serviceid, serviceid_len) == 0) {
        cJSON *item;
        const char *response_fmt = "{}";
        recv_msg_t msg;
        int isDiscard = 0;

        strcpy(msg.seq, SPEC_SEQ);
        if ((item = cJSON_GetObjectItem(root, "seq")) != NULL && cJSON_IsString(item)) {
            if (property_seq_handle(item->valuestring, &msg)) {
                isDiscard = 1;
            }
        }
        if (isDiscard == 0) {
            if ((item = cJSON_GetObjectItem(root, "flag")) != NULL && cJSON_IsNumber(item)) {
                msg.flag = item->valueint;
            } else {
                msg.flag = 0;
            }

            if ((item = cJSON_GetObjectItem(root, "method")) != NULL && cJSON_IsNumber(item)) {
                msg.method = item->valueint;
            } else {
                msg.method = 0;
            }

            if ((item = cJSON_GetObjectItem(root, "params")) != NULL && cJSON_IsString(item)) {
                if (msg.method == 0) {
                    msg.from = FROM_SERVICE_SET;
                    property_setting_handle(item->valuestring, strlen(item->valuestring), &msg);
                } else
                    LOG_TRACE("todo!!");
            }
        }
        *response = HAL_Malloc(RESPONE_BUFFER_SIZE);
        if (*response == NULL) {
            LOG_TRACE("Memory Not Enough");
            cJSON_Delete(root);
            return -1;
        }
        memset(*response, 0, RESPONE_BUFFER_SIZE);
        memcpy(*response, response_fmt, strlen(response_fmt));
        *response_len = strlen(*response);
    }
    cJSON_Delete(root);
    return 0;
}

static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int ret = 0;
    recv_msg_t msg;

#ifdef CERTIFICATION_TEST_MODE
    return ct_main_property_set_event_handler(devid, request, request_len);
#endif

    LOG_TRACE("property set,  Devid: %d, payload: \"%s\"", devid, request);
    msg.from = FROM_PROPERTY_SET;
    strcpy(msg.seq, SPEC_SEQ);
    property_setting_handle(request, request_len, &msg);
    return ret;
}

#ifdef ALCS_ENABLED
static int user_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
        int *response_len)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    device_status_t *device_status = &user_example_ctx->status;
    cJSON *request_root = NULL, *item_propertyid = NULL;
    cJSON *response_root = NULL;

#ifdef CERTIFICATION_TEST_MODE
    return ct_main_property_get_event_handler(devid, request, request_len, response, response_len);
#endif

    LOG_TRACE("Property Get Received, Devid: %d, Request: %s", devid, request);
    request_root = cJSON_Parse(request);
    if (request_root == NULL || !cJSON_IsArray(request_root)) {
        LOG_TRACE("JSON Parse Error");
        return -1;
    }

    response_root = cJSON_CreateObject();
    if (response_root == NULL) {
        LOG_TRACE("No Enough Memory");
        cJSON_Delete(request_root);
        return -1;
    }
    int index = 0;
    for (index = 0; index < cJSON_GetArraySize(request_root); index++) {
        item_propertyid = cJSON_GetArrayItem(request_root, index);
        if (item_propertyid == NULL || !cJSON_IsString(item_propertyid)) {
            LOG_TRACE("JSON Parse Error");
            cJSON_Delete(request_root);
            cJSON_Delete(response_root);
            return -1;
        }
        LOG_TRACE("Property ID, index: %d, Value: %s", index, item_propertyid->valuestring);
        if (strcmp("powerstate", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "powerstate", device_status->powerswitch);
        }
        else if (strcmp("allPowerstate", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "allPowerstate", device_status->all_powerstate);
        }
#ifdef TSL_FY_SUPPORT /* support old feiyan TSL */
        else if (strcmp("PowerSwitch", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "PowerSwitch", device_status->powerswitch);
        }
#endif
#ifdef AOS_TIMER_SERVICE
        else if (strcmp("LocalTimer", item_propertyid->valuestring) == 0) {
            char *local_timer_str = NULL;

            if (NULL != (local_timer_str = timer_service_property_get("[\"LocalTimer\"]"))) {
                LOG_TRACE("local_timer %s", local_timer_str);
                cJSON *property = NULL, *value = NULL;

                property = cJSON_Parse(local_timer_str);
                if (property == NULL) {
                    LOG_TRACE("No Enough Memory");
                    continue;
                }
                value = cJSON_GetObjectItem(property, "LocalTimer");
                if (value == NULL) {
                    LOG_TRACE("No Enough Memory");
                    cJSON_Delete(property);
                    continue;
                }
                cJSON *dup_value = cJSON_Duplicate(value, 1);

                cJSON_AddItemToObject(response_root, "LocalTimer", dup_value);
                cJSON_Delete(property);
                example_free(local_timer_str);
            } else {
                cJSON *array = cJSON_CreateArray();
                cJSON_AddItemToObject(response_root, "LocalTimer", array);
            }
        } else if (strcmp("CountDownList", item_propertyid->valuestring) == 0) {
            char *count_down_list_str = NULL;

            if (NULL != (count_down_list_str = timer_service_property_get("[\"CountDownList\"]"))) {
                LOG_TRACE("CountDownList %s", count_down_list_str);
                cJSON *property = NULL, *value = NULL;

                property = cJSON_Parse(count_down_list_str);
                if (property == NULL) {
                    LOG_TRACE("No Enough Memory");
                    continue;
                }
                value = cJSON_GetObjectItem(property, "CountDownList");
                if (value == NULL) {
                    LOG_TRACE("No Enough Memory");
                    cJSON_Delete(property);
                    continue;
                }
                cJSON *dup_value = cJSON_Duplicate(value, 1);

                cJSON_AddItemToObject(response_root, "CountDownList", dup_value);
                cJSON_Delete(property);
                example_free(count_down_list_str);
            } else {
                cJSON_AddStringToObject(response_root, "CountDownList", "");
            }
        }
#endif
    }

    cJSON_Delete(request_root);

    *response = cJSON_PrintUnformatted(response_root);
    if (*response == NULL) {
        LOG_TRACE("cJSON_PrintUnformatted Error");
        cJSON_Delete(response_root);
        return -1;
    }
    cJSON_Delete(response_root);
    *response_len = strlen(*response);

    LOG_TRACE("Property Get Response: %s", *response);
    return SUCCESS_RETURN;
}
#endif

static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    //const char *reply_value = (reply == NULL) ? ("NULL") : (reply);
    //const int reply_value_len = (reply_len == 0) ? (strlen("NULL")) : (reply_len);

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
    IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char *)property,
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

void user_post_powerstate(int powerstate)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    device_status_t *device_status = &user_example_ctx->status;

    device_status->powerswitch = powerstate;
    device_status->all_powerstate = powerstate;
    report_device_property(NULL, 0);
}

void user_post_property_after_connected(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    device_status_t *device_status = &user_example_ctx->status;
    char property_payload[128];

#ifdef TSL_FY_SUPPORT /* support old feiyan TSL */
    snprintf(property_payload, sizeof(property_payload), \
             "{\"%s\":%d, \"%s\":%d, \"%s\":%d}", "PowerSwitch", device_status->powerswitch, \
             "powerstate", device_status->powerswitch, "allPowerstate", device_status->all_powerstate);
#else
    snprintf(property_payload, sizeof(property_payload), \
             "{\"%s\":%d, \"%s\":%d}", "powerstate", device_status->powerswitch, \
             "allPowerstate", device_status->powerswitch, device_status->all_powerstate);
#endif
    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
            property_payload, strlen(property_payload));

    LOG_TRACE("Post Event Message ID: %d payload %s", res, property_payload);
}

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
    device_status_t                *device_status = &user_example_ctx->status;
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

    device_status->powerswitch = (int)product_get_switch();
    device_status->all_powerstate = device_status->powerswitch;

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
#ifdef CERTIFICATION_TEST_MODE
    IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, ct_main_down_raw_data_arrived_event_handler);
#endif
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
#ifdef ALCS_ENABLED
    /*Only for local communication service(ALCS) */
    IOT_RegisterCallback(ITE_PROPERTY_GET, user_property_get_event_handler);
#endif
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);
    // IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    // IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);
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

#ifdef CERTIFICATION_TEST_MODE
#ifdef CT_PRODUCT_DYNAMIC_REGISTER_AND_USE_RAWDATA
    dynamic_register = 1;
#endif
#endif

    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
#ifdef REPORT_UUID_ENABLE
    int uuid_enable = 1;
    IOT_Ioctl(IOTX_IOCTL_SET_UUID_ENABLED, (void *)&uuid_enable);
#endif

    /* Choose Whether You Need Post Property/Event Reply */
    int post_event_reply = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

#ifdef CERTIFICATION_TEST_MODE
    ct_ut_init();
#endif

    /* Create Master Device Resources */
    do {
        user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
        if (user_example_ctx->master_devid < 0) {
            LOG_TRACE("IOT_Linkkit_Open Failed, retry after 5s...\n");
            HAL_SleepMs(5000);
        }
    } while (user_example_ctx->master_devid < 0);
#ifdef AOS_TIMER_SERVICE
    int ret = timer_service_init(control_targets_list, NUM_OF_PROPERTYS,
            countdownlist_target_list, NUM_OF_COUNTDOWN_LIST_TARGET,
            localtimer_target_list, NUM_OF_LOCAL_TIMER_TARGET,
            timer_service_cb, num_of_tsl_type, NULL);
    if (ret == 0)
        ntp_update = true;
#endif
#ifdef AIOT_DEVICE_TIMER_ENABLE
    aiot_device_timer_init(propertys_list, NUM_OF_TIMER_PROPERTYS, timer_service_cb);
#endif

#ifdef EN_COMBO_NET
    aiot_ais_report_awss_status(AIS_AWSS_STATUS_PROGRESS_REPORT, AIS_AWSS_PROGRESS_CONNECT_MQTT_START);
#endif

    /* Start Connect Aliyun Server */
    do {
        res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
        if (res < 0) {
            LOG_TRACE("IOT_Linkkit_Connect Failed, retry after 5s...\n");
            #ifdef EN_COMBO_NET
                aiot_ais_report_awss_status(AIS_AWSS_STATUS_PROGRESS_REPORT, AIS_AWSS_PROGRESS_CONNECT_MQTT_FAILED | (res & 0x00FF));
            #endif
            HAL_SleepMs(5000);
        }
    } while (res < 0);

    time_begin_sec = user_update_sec();
    ntp_server_init();

    while (1) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);

#ifndef REPORT_MULTHREAD
        process_property_report();
#endif
        time_now_sec = user_update_sec();
        if (time_prev_sec == time_now_sec) {
            continue;
        }
        if (max_running_seconds && (time_now_sec - time_begin_sec > max_running_seconds)) {
            LOG_TRACE("Example Run for Over %d Seconds, Break Loop!\n", max_running_seconds);
            break;
        }

        if (user_master_dev_available())
        {
#ifdef AIOT_DEVICE_TIMER_ENABLE
            if ((time_now_sec % 11 == 0) && !aiot_device_timer_inited())
                aiot_device_timer_init(propertys_list, NUM_OF_TIMER_PROPERTYS, timer_service_cb);
#endif
#ifdef CERTIFICATION_TEST_MODE
            ct_ut_misc_process(time_now_sec);
#endif
        }

        time_prev_sec = time_now_sec;
    }

    IOT_Linkkit_Close(user_example_ctx->master_devid);

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_SetLogLevel(IOT_LOG_NONE);

    return 0;
}
