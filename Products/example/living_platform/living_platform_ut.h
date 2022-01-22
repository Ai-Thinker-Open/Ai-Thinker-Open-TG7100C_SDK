#ifndef __LIVING_PLATFORM_UT_H__
#define __LIVING_PLATFORM_UT_H__

#define KV_KEY_PK "pk"
#define KV_KEY_PS "ps"
#define KV_KEY_DN "dn"
#define KV_KEY_DS "ds"

#define MAX_KEY_LEN (6)

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
    char SNR;
    char bssid[AP_BSSID_MAX_LEN];
    char band[WIFI_BAND_MAX_LEN];
    char rssi;
} WiFi_t;

typedef struct _living_platform_tsl_s
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
} living_platform_tsl_t;

extern int living_platform_ut_init(void);
extern void living_platform_ut_misc_process(uint64_t time_now_sec);
extern living_platform_tsl_t* living_platform_ut_get_tsl_data(void);
extern int living_platform_ut_set_LightSwitch(char LightSwitch);
extern int living_platform_ut_get_LightSwitch(void);
extern int living_platform_ut_set_NightLightSwitch(char NightLightSwitch);
extern int living_platform_ut_set_WorkMode(unsigned char WorkMode);
extern int living_platform_ut_set_Brightness(unsigned char Brightness);
extern int living_platform_ut_set_RGB(unsigned char R, unsigned char G, unsigned char B);
extern int living_platform_ut_set_Float(float f);
extern int living_platform_ut_set_Double(double d);
extern int living_platform_ut_set_PropertyString(char *PropertyString);
extern int living_platform_ut_set_WorkTime(char *WorkTime);
extern void living_platform_deviceinfo_update(void);
#if defined(TSL_POST_UT_TEST_ENABLE)
extern int living_platform_ut_tsl_unify_post(char* type, char **tsl);
#endif

#if defined(SYS_TIMER_UT_TEST_ENABLE)
int living_platform_ut_systimer_test(char* action_str, char **timer_str);
#endif

#endif
