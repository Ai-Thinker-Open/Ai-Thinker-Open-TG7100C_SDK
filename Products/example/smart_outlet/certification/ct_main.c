/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "cJSON.h"
#include "iot_import.h"
#include "iot_export_linkkit.h"

#include "iotx_log.h"

#include "ct_ut.h"
#include "ct_ota.h"

int ct_main_down_raw_data_arrived_event_handler(const int devid, const unsigned char *payload,
                                                    const int payload_len)
{
    ct_info("Down Raw Message, Devid: %d, Payload Length: %d", devid, payload_len);
    if (payload[0] == 0x02 && payload_len == 6)
    {
        ct_ut_set_LightSwitch(payload[5]);
    }

    return 0;
}

int ct_main_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
                                              const char *request, const int request_len,
                                              char **response, int *response_len)
{
    int transparency = 0;
    cJSON *root = NULL, *item_transparency = NULL;

    ct_info("Service Request Received, Devid: %d, Service ID: %.*s, Payload: %s", devid, serviceid_len,
            serviceid,
            request);

    /* Parse Root */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root))
    {
        ct_err("JSON Parse Error");
        return -1;
    }

    if (strlen("Custom") == serviceid_len && memcmp("Custom", serviceid, serviceid_len) == 0)
    {
        /* Parse Item */
        const char *response_fmt = "{\"transparency\":%d}";
        item_transparency = cJSON_GetObjectItem(root, "transparency");
        if (item_transparency == NULL || !cJSON_IsNumber(item_transparency))
        {
            cJSON_Delete(root);
            return -1;
        }
        ct_info("transparency: %d", item_transparency->valueint);
        transparency = item_transparency->valueint + 1;
        if (transparency > 100)
        {
            transparency = 100;
        }

        /* Send Service Response To Cloud */
        *response_len = strlen(response_fmt) + 10 + 1;
        *response = (char *)HAL_Malloc(*response_len);
        if (*response == NULL)
        {
            ct_err("Memory Not Enough");
            return -1;
        }
        memset(*response, 0, *response_len);
        HAL_Snprintf(*response, *response_len, response_fmt, transparency);
        *response_len = strlen(*response);
    }
    else if (strlen("ToggleLightSwitch") == serviceid_len && memcmp("ToggleLightSwitch", serviceid, serviceid_len) == 0)
    {
        /* Parse Item */
        const char *response_fmt = "{\"LightSwitch\":%d}";

        /* Send Service Response To Cloud */
        *response_len = strlen(response_fmt) + strlen("LightSwitch") + 10;
        *response = (char *)HAL_Malloc(*response_len);
        if (*response == NULL)
        {
            ct_err("Memory Not Enough");
            return -1;
        }
        memset(*response, 0, *response_len);
        if (ct_ut_get_LightSwitch() == 0)
        {
            ct_ut_set_LightSwitch(1);
        }
        else
        {
            ct_ut_set_LightSwitch(0);
        }

        HAL_Snprintf(*response, *response_len, response_fmt, ct_ut_get_LightSwitch());
        *response_len = strlen(*response);
    }

    cJSON_Delete(root);

    return 0;
}

#ifdef ALCS_ENABLED
//Just for reference,user have to change his owner properties
int ct_main_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
                                           int *response_len)
{
    int index = 0;
    ct_tsl_t *p_ct_tsl_data = ct_ut_get_tsl_data();
    cJSON *response_root = NULL;
    cJSON *request_root = NULL, *item_propertyid = NULL;
    ct_info("Property Get Received, Devid: %d, Request: %s", devid, request);

    /* Parse Request */
    request_root = cJSON_Parse(request);
    if (request_root == NULL || !cJSON_IsArray(request_root))
    {
        ct_info("JSON Parse Error");
        return -1;
    }

    /* Prepare Response */
    response_root = cJSON_CreateObject();
    if (response_root == NULL)
    {
        ct_info("No Enough Memory");
        cJSON_Delete(request_root);
        return -1;
    }

    for (index = 0; index < cJSON_GetArraySize(request_root); index++)
    {
        item_propertyid = cJSON_GetArrayItem(request_root, index);
        if (item_propertyid == NULL || !cJSON_IsString(item_propertyid))
        {
            ct_info("JSON Parse Error");
            cJSON_Delete(request_root);
            cJSON_Delete(response_root);
            return -1;
        }

        ct_info("Property ID, index: %d, Value: %s", index, item_propertyid->valuestring);
        if (strcmp("WIFI_Band", item_propertyid->valuestring) == 0)
        {
            cJSON_AddStringToObject(response_root, "WIFI_Band", p_ct_tsl_data->wifi.band);
        }
        else if (strcmp("WIFI_AP_BSSID", item_propertyid->valuestring) == 0)
        {
            cJSON_AddStringToObject(response_root, "WIFI_AP_BSSID", p_ct_tsl_data->wifi.bssid);
        }
        else if (strcmp("WIFI_Channel", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "WIFI_Channel", p_ct_tsl_data->wifi.Channel);
        }
        else if (strcmp("WiFI_SNR", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "WiFI_SNR", p_ct_tsl_data->wifi.SNR);
        }
        else if (strcmp("WiFI_RSSI", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "WiFI_RSSI", p_ct_tsl_data->wifi.rssi);
        }
        else if (strcmp("LightSwitch", item_propertyid->valuestring) == 0)
        {
            cJSON_AddBoolToObject(response_root, "LightSwitch", p_ct_tsl_data->LightSwitch);
        }
        else if (strcmp("NightLightSwitch", item_propertyid->valuestring) == 0)
        {
            cJSON_AddBoolToObject(response_root, "NightLightSwitch", p_ct_tsl_data->NightLightSwitch);
        }
        else if (strcmp("WorkMode", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "WorkMode", p_ct_tsl_data->WorkMode);
        }
        else if (strcmp("worktime", item_propertyid->valuestring) == 0)
        {
            cJSON_AddStringToObject(response_root, "worktime", p_ct_tsl_data->WorkTime);
        }
        else if (strcmp("Brightness", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "Brightness", p_ct_tsl_data->Brightness);
        }
        else if (strcmp("onlyread", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "onlyread", p_ct_tsl_data->readonly);
        }
        else if (strcmp("floatid", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "floatid", p_ct_tsl_data->f);
        }
        else if (strcmp("doubleid", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "doubleid", p_ct_tsl_data->d);
        }
        else if (strcmp("PropertyString", item_propertyid->valuestring) == 0)
        {
            cJSON_AddStringToObject(response_root, "PropertyString", p_ct_tsl_data->PropertyString);
        }
        else if (strcmp("RGBColor", item_propertyid->valuestring) == 0)
        {
            cJSON *item_RGBColor = cJSON_CreateObject();
            if (item_RGBColor == NULL)
            {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);

                return -1;
            }
            cJSON_AddNumberToObject(item_RGBColor, "Red", p_ct_tsl_data->RGB.R);
            cJSON_AddNumberToObject(item_RGBColor, "Green", p_ct_tsl_data->RGB.G);
            cJSON_AddNumberToObject(item_RGBColor, "Blue", p_ct_tsl_data->RGB.B);

            cJSON_AddItemToObject(response_root, "RGBColor", item_RGBColor);
        }
    }
    cJSON_Delete(request_root);

    *response = cJSON_PrintUnformatted(response_root);
    if (*response == NULL)
    {
        ct_info("No Enough Memory");
        cJSON_Delete(response_root);
        return -1;
    }
    cJSON_Delete(response_root);
    *response_len = strlen(*response);

    ct_info("Property Get Response: %s", *response);

    return SUCCESS_RETURN;
}
#endif

/**
 * @brief 解析所有属性设置的值
 * @param request 指向属性设置请求payload的指针
 * @param request_len 属性设置请求的payload长度
 * @return 解析成功: 0, 解析失败: <0
 */
static int32_t ct_main_parse_property(const char *request, uint32_t request_len)
{
    cJSON *lightswitch = NULL;
    cJSON *rgbcolor = NULL;
    cJSON *nightlightswitch = NULL;
    cJSON *workmode = NULL;
    cJSON *brightness = NULL;
    cJSON *worktime = NULL;
    cJSON *floatid = NULL;
    cJSON *doubleid = NULL;
    cJSON *propertystring = NULL;

    cJSON *req = cJSON_Parse(request);
    if (req == NULL || !cJSON_IsObject(req))
    {
        return -0x911;
    }

    lightswitch = cJSON_GetObjectItem(req, "LightSwitch");
    if (lightswitch != NULL && cJSON_IsNumber(lightswitch))
    {
        /* process property LightSwitch here */

        ct_info("property id: LightSwitch, value: %d", lightswitch->valueint);
        ct_ut_set_LightSwitch(lightswitch->valueint);
    }

    rgbcolor = cJSON_GetObjectItem(req, "RGBColor");
    if (rgbcolor != NULL && cJSON_IsObject(rgbcolor))
    {
        /* process property RGBColor here */
        cJSON *R = cJSON_GetObjectItem(rgbcolor, "Red");
        cJSON *G = cJSON_GetObjectItem(rgbcolor, "Green");
        cJSON *B = cJSON_GetObjectItem(rgbcolor, "Blue");

        if ((R != NULL && cJSON_IsNumber(R)) &&
            (G != NULL && cJSON_IsNumber(G)) &&
            (B != NULL && cJSON_IsNumber(B)))
        {
            ct_info("struct property id: RGBColor R:%d G:%d B:%d", R->valueint, G->valueint, B->valueint);
            ct_ut_set_RGB(R->valueint, G->valueint, B->valueint);
        }
    }

    nightlightswitch = cJSON_GetObjectItem(req, "NightLightSwitch");
    if (nightlightswitch != NULL && cJSON_IsNumber(nightlightswitch))
    {
        /* process property NightLightSwitch here */

        ct_info("property id: NightLightSwitch, value: %d", nightlightswitch->valueint);
        ct_ut_set_NightLightSwitch(nightlightswitch->valueint);
    }

    workmode = cJSON_GetObjectItem(req, "WorkMode");
    if (workmode != NULL && cJSON_IsNumber(workmode))
    {
        /* process property WorkMode here */

        ct_info("property id: WorkMode, value: %d", workmode->valueint);
        ct_ut_set_WorkMode(workmode->valueint);
    }

    brightness = cJSON_GetObjectItem(req, "Brightness");
    if (brightness != NULL && cJSON_IsNumber(brightness))
    {
        /* process property Brightness here */

        ct_info("property id: Brightness, value: %d", brightness->valueint);
        ct_ut_set_Brightness(brightness->valueint);
    }

    worktime = cJSON_GetObjectItem(req, "worktime");
    if (worktime != NULL && cJSON_IsString(worktime))
    {
        /* process property worktime here */

        ct_info("property id: worktime, value: %s", worktime->valuestring);
        ct_ut_set_WorkTime(worktime->valuestring);
    }

    floatid = cJSON_GetObjectItem(req, "floatid");
    if (floatid != NULL && cJSON_IsNumber(floatid))
    {
        /* process property float here */

        ct_info("property id: float, value: %f", floatid->valuedouble);
        ct_ut_set_Float(floatid->valuedouble);
    }

    doubleid = cJSON_GetObjectItem(req, "doubleid");
    if (doubleid != NULL && cJSON_IsNumber(doubleid))
    {
        /* process property double here */

        ct_info("property id: double, value: %f", doubleid->valuedouble);
        ct_ut_set_Double(doubleid->valuedouble);
    }

    propertystring = cJSON_GetObjectItem(req, "PropertyString");
    if (propertystring != NULL && cJSON_IsString(propertystring))
    {
        /* process property PropertyString here */

        ct_info("property id: PropertyString, value: %s", propertystring->valuestring);
        ct_ut_set_PropertyString(propertystring->valuestring);
    }

    cJSON_Delete(req);
    return 0;
}

int ct_main_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;

    ct_info("Property Set Received, Devid: %d, Request: %s", devid, request);

    ct_main_parse_property(request, request_len);
    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)request, request_len);

    ct_info("Post Property Message ID: %d", res);

    return 0;
}
