#ifndef __CT_UT_H__
#define __CT_UT_H__

#include "ct_config.h"

#define ct_debug(...) log_debug("ct", __VA_ARGS__)
#define ct_info(...) log_info("ct", __VA_ARGS__)
#define ct_warn(...) log_warning("ct", __VA_ARGS__)
#define ct_err(...) log_err("ct", __VA_ARGS__)
#define ct_crit(...) log_crit("ct", __VA_ARGS__)

#define EXAMPLE_MASTER_DEVID (0)
#define PROPERTY_STRING_MAX_LEN (1024)
#define WORKTIME_MAX_LEN (64)
#define WIFI_BAND_MAX_LEN (64)
#define AP_BSSID_MAX_LEN (64)

typedef struct _RGBColor_s
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
} RGBColor_t;

typedef struct _WiFi_s
{
    int Channel;
    int32_t SNR;
    char bssid[AP_BSSID_MAX_LEN];
    char band[WIFI_BAND_MAX_LEN];
    int32_t rssi;
} WiFi_t;

typedef struct _ct_tsl_s
{
    char LightSwitch;
    char NightLightSwitch;
    RGBColor_t RGB;
    unsigned char Brightness;
    unsigned char WorkMode;
    float f;
    double d;
    int readonly;
    char *PropertyString;
    char WorkTime[WORKTIME_MAX_LEN];
    WiFi_t wifi;
} ct_tsl_t;

int ct_ut_init(void);
void ct_ut_misc_process(uint64_t time_now_sec);
ct_tsl_t* ct_ut_get_tsl_data(void);
int ct_ut_set_LightSwitch(char LightSwitch);
int ct_ut_get_LightSwitch(void);
int ct_ut_set_NightLightSwitch(char NightLightSwitch);
int ct_ut_set_WorkMode(unsigned char WorkMode);
int ct_ut_set_Brightness(unsigned char Brightness);
int ct_ut_set_RGB(unsigned char R, unsigned char G, unsigned char B);
int ct_ut_set_Float(float f);
int ct_ut_set_Double(double d);
int ct_ut_set_PropertyString(char *PropertyString);
int ct_ut_set_WorkTime(char *WorkTime);

int ct_main_down_raw_data_arrived_event_handler(const int devid, const unsigned char *payload,
                                                    const int payload_len);
int ct_main_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
                                              const char *request, const int request_len,
                                              char **response, int *response_len);
int ct_main_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
                                           int *response_len);
int ct_main_property_set_event_handler(const int devid, const char *request, const int request_len);

#endif
