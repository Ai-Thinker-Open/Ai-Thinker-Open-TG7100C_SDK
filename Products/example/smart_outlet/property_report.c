/*
 *copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"
#include "iot_export.h"
#include "iot_import.h"

#include "iot_export_linkkit.h"
#include "linkkit_export.h"
#include "cJSON.h"
#include "aos/kv.h"
#include "iot_export.h"
#include "smart_outlet.h"
#include "vendor.h"
#include "device_state_manger.h"
#include "msg_process_center.h"
#include "property_report.h"
#ifdef EN_COMBO_NET
#include "combo_net.h"
#endif

extern aos_queue_t *g_property_report_queue_id;
extern char *g_property_report_queue_buff;

const char *g_light_property_report[] = {
#ifdef TSL_FY_SUPPORT
    "PowerSwitch",
#endif
    "powerstate",
    "allPowerstate",
    "CommonServiceResponse"
};

#define TIMER_UPDATE_INTERVAL   (30 * 60 * 1000)
#define NTP_SYNC_FAILED_TIMEOUT (1 * 60 * 1000)

static aos_timer_t ntp_timer;
static uint64_t ntp_time_ms = 0;
static uint32_t basic_time = 0;

void ntp_timer_update(const char *str)
{
    if (strlen(str) < 13)
        return;
    ntp_time_ms = (uint64_t) strtoll(str, NULL, 10);
    basic_time = aos_now_ms();
}

static void timer_callback(void *arg1, void *arg2)
{
    static int tc_flag = 0;

    if (ntp_time_ms != 0 && tc_flag == 0) {
        tc_flag = 1;
        aos_timer_stop(&ntp_timer);
        aos_timer_change(&ntp_timer, TIMER_UPDATE_INTERVAL);
        aos_timer_start(&ntp_timer);
    }
    linkkit_ntp_time_request(ntp_timer_update);
}

static void sync_localtime(void *arg)
{
    linkkit_ntp_time_request(ntp_timer_update);
}

void ntp_server_init(void)
{
    aos_post_delayed_action(500, sync_localtime, NULL);
    aos_timer_new_ext(&ntp_timer, timer_callback, NULL, NTP_SYNC_FAILED_TIMEOUT, 1, 1);
}

static int user_property_format(const char *request_in, const int request_len_in, char **request_out,
        uint32_t * request_len_out)
{
    cJSON *request_in_json = NULL, *request_out_json = NULL;
    int index = 0;

    if (ntp_time_ms == 0)
        return -1;

    /* Parse Request */
    request_in_json = cJSON_Parse(request_in);
    if (request_in_json == NULL) {
        LOG_TRACE("JSON Parse Error");
        return -1;
    }
    /* Prepare Response */
    request_out_json = cJSON_CreateObject();
    if (request_out_json == NULL) {
        LOG_TRACE("No Enough Memory");
        cJSON_Delete(request_in_json);
        return -1;
    }
    uint64_t timestamp = ntp_time_ms + aos_now_ms() - basic_time;
    for (index = 0; index < sizeof(g_light_property_report) / sizeof(const char *); index++) {
        cJSON *item_property = cJSON_CreateObject();
        if (item_property == NULL) {
            cJSON_Delete(request_in_json);
            cJSON_Delete(request_out_json);
            return -1;
        }

        cJSON *item_value = cJSON_GetObjectItemCaseSensitive(request_in_json, g_light_property_report[index]);
        if (item_value != NULL) {
            if (item_value->type == cJSON_Number) {
                cJSON_AddNumberToObject(item_property, "value", item_value->valueint);
            } else if (item_value->type == cJSON_String) {
                cJSON_AddStringToObject(item_property, "value", item_value->valuestring);
            } else if (item_value->type == cJSON_Object || item_value->type == cJSON_Array) {
                cJSON *new_item_value = cJSON_Duplicate(item_value, 1);
                if (new_item_value == NULL) {
                    cJSON_Delete(item_property);
                    cJSON_Delete(request_in_json);
                    cJSON_Delete(request_out_json);
                    return -1;
                }
                cJSON_AddItemToObject(item_property, "value", new_item_value);
            }
            cJSON_AddNumberToObject(item_property, "time", timestamp);
            cJSON_AddItemToObject(request_out_json, g_light_property_report[index], item_property);
        } else {
            cJSON_Delete(item_property);
        }
    }
    cJSON_Delete(request_in_json);

    *request_out = cJSON_PrintUnformatted(request_out_json);
    if (*request_out == NULL) {
        LOG_TRACE("No Enough Memory");
        cJSON_Delete(request_out_json);
        return -1;
    }
    cJSON_Delete(request_out_json);
    *request_len_out = strlen(*request_out);

    return SUCCESS_RETURN;
}

void user_post_property(property_report_msg_t * msg)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *property_payload = NULL;
    cJSON *response_root = NULL, *item_csr = NULL;

    response_root = cJSON_CreateObject();
    if (response_root == NULL) {
        return;
    }

    if (msg->seq != NULL && strcmp(msg->seq, SPEC_SEQ)) {
        item_csr = cJSON_CreateObject();
        if (item_csr == NULL) {
            cJSON_Delete(response_root);
            return;
        }
        cJSON_AddStringToObject(item_csr, "seq", msg->seq);
        cJSON_AddItemToObject(response_root, "CommonServiceResponse", item_csr);
    }
#ifdef TSL_FY_SUPPORT
    cJSON_AddNumberToObject(response_root, "PowerSwitch", msg->powerswitch);
#endif
    cJSON_AddNumberToObject(response_root, "powerstate", msg->powerswitch);
    cJSON_AddNumberToObject(response_root, "allPowerstate", msg->all_powerstate);
    property_payload = cJSON_PrintUnformatted(response_root);
    cJSON_Delete(response_root);

    char *property_formated;
    uint32_t len;
    res = user_property_format(property_payload, strlen(property_payload), &property_formated, &len);
    if (0 == res) {
        if (msg->seq != NULL && strcmp(msg->seq, SPEC_SEQ)) {
            res = IOT_Linkkit_Report_Ext(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                    (unsigned char *)property_formated, strlen(property_formated), msg->flag);
        } else {
            res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                    (unsigned char *)property_formated, strlen(property_formated));
        }
        LOG_TRACE("Post Property Message ID: %d Payload %s", res, property_formated);
        example_free(property_formated);
    } else {
        if (msg->seq != NULL && strcmp(msg->seq, SPEC_SEQ)) {
            res = IOT_Linkkit_Report_Ext(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                    (unsigned char *)property_payload, strlen(property_payload), msg->flag);
        } else {
            res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                    (unsigned char *)property_payload, strlen(property_payload));
        }
        LOG_TRACE("Post Property Message ID: %d Payload %s", res, property_payload);
    }
    example_free(property_payload);
}

void report_device_property(char *seq, int flag)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    device_status_t *device_status = &user_example_ctx->status;
    property_report_msg_t msg;

    if (flag == 0x07)
        return;

    msg.flag = flag;
    if (seq == NULL) {
        strcpy(msg.seq, SPEC_SEQ);
    } else {
        strcpy(msg.seq, seq);
    }

    msg.powerswitch = device_status->powerswitch;
    msg.all_powerstate = device_status->all_powerstate;

    int ret = aos_queue_send(g_property_report_queue_id, &msg, sizeof(property_report_msg_t));
    if (0 != ret) {
        LOG_TRACE("###############ERROR: report_light_property aos_queue_send failed! #################\r\n");
    }
}

#ifndef REPORT_MULTHREAD
void process_property_report(void)
{
    unsigned int rcvLen;
    property_report_msg_t msg;

    if (aos_queue_recv(g_property_report_queue_id, 0, &msg, &rcvLen) == 0) {
        //LOG_TRACE("===============aos_queue_recved: seq %s============.\r\n", msg.seq);
        user_post_property(&msg);
    }
}

#else
void process_property_report_task(void *argv)
{
    unsigned int rcvLen;
    property_report_msg_t msg;

    while (1) {
        if (aos_queue_recv(g_property_report_queue_id, AOS_WAIT_FOREVER, &msg, &rcvLen) == 0) {
            user_post_property(&msg);
        }
    }
}
#endif

