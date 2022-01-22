/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifdef AIOT_DEVICE_TIMER_ENABLE
#include "stdio.h"
#include "iot_export_timer.h"
#include "iot_import.h"
#include "cJSON.h"
#include "ccronexpr.h"

#define SDK_VER_16X
#define DAYS_OF_WEEK    7
typedef struct device_timer {
    char timer[28];		// cron格式表达式
    uint8_t enable;		// 是否启用
    uint8_t runTime;    // 循环开时长
    uint8_t sleepTime;	// 循环关时长
    uint8_t repeat;		// 是否为重复执行定时
    uint8_t type;		// 定时类型
    char endTime[6];	// 结束时间
    char *targets;		// malloc分配,最大128字节
    int timezone;		// 时区
    int offset_start[DAYS_OF_WEEK];	// 转换后定时触发时间
    int offset_end[DAYS_OF_WEEK];	// 转换后循环结束时间
    int event_ready[DAYS_OF_WEEK];  // 确保定时时间能被执行
} device_timer_t;

extern int linkkit_ntp_time_request(void (*ntp_reply)(const char *ntp_offset_time_ms));
static uint32_t utc = 0, uptime_on_get_utc= 0, ntp_time_s=0;
static int utc_week_second_offset = -1, g_inited = 0;

#define WEEK_START 1609603200 // 2021.01.03 00:00(北京时间 时区+8) 纽约时区-5
device_timer_t g_device_timer[DeviceTimerSize];
static void *check_timer = NULL;
static uint32_t pre_ticket = 0;
static void device_timer_runner(void);

int aiot_device_timer_inited(void) {
    return g_inited;
}
static uint32_t _get_uptime(void) {
    #ifndef SDK_VER_16X
    return (uint32_t)(aiot_al_time_ms()/1000);
    #else
    return (uint32_t)(HAL_UptimeMs()/1000);
    #endif
}
static void get_timer_from_kv(void) {
    int len = 1536;  // kv max len; ITEM_MAX_VAL_LEN=1536 in kvmgr.c
    char* tmp = NULL;
    #ifndef SDK_VER_16X
    tmp = aiot_al_malloc(len, LINKKIT_DEMO_MODULE_NAME); 
    #else
    tmp = (char *)HAL_Malloc(len);
    #endif
    if (tmp == NULL) {
	printf("get_timer malloc failed!\r\n");
	return;
    }
    memset(tmp, 0, len);
    #ifndef SDK_VER_16X
    if (0 == aiot_kv_get(DEVICETIMER, tmp, &len)) {
    #else
    if (0 == HAL_Kv_Get(DEVICETIMER, tmp, &len)) {
    #endif
        DS_DEBUG("get %s\r\n", tmp);
        deviceTimerParse(tmp, 1, 0);
    }
    #ifndef SDK_VER_16X
    aiot_al_free(tmp);
    #else
    HAL_Free(tmp);
    #endif
}

uint64_t up_tick_on_get_utc = 0;
static char str_time[16] = {0};
void _ntp_update(const char *str)
{   
    if (strlen(str) < 4)
        return;
    // char str_time[16] = {0};
    int str_len = strlen(str);
    memset(str_time, 0, sizeof(str_time));
    memcpy(str_time, str, str_len);
    // str_time[str_len - 3] = 0;
    // str_time[str_len - 2] = 0;
    // str_time[str_len - 1] = 0;

    // ntp_time_s = (uint32_t)atoi(str_time);
    ntp_time_s = (uint32_t)(strtoll(str_time, NULL, 10)/1000);

    utc = (ntp_time_s - WEEK_START) % (86400 * 7); // 当前UTC,转换为以周为周期的余数;
    #ifndef SDK_VER_16X
    up_tick_on_get_utc = aiot_al_time_ms();
    #else
    up_tick_on_get_utc = HAL_UptimeMs();
    #endif
    uptime_on_get_utc = _get_uptime();
    pre_ticket = 0;
    // EXAMPLE_TRACE("utc=%s,ntp_time_s=%d  utc_week_offset=%d \n", str, ntp_time_s, utc);
    printf("TS_utc=%s,ntp_time_s=%d  utc_week_offset=%d %d\r\n", str, ntp_time_s, utc, uptime_on_get_utc);
    if (check_timer == NULL) {
        #ifndef SDK_VER_16X
        check_timer = aiot_al_timer_create("device_timer", (void (*)(void *))device_timer_runner, NULL);
        #else
        check_timer = HAL_Timer_Create("device_timer", (void (*)(void *))device_timer_runner, NULL);
        #endif
    }
    if (g_inited == 0)
        get_timer_from_kv();
    g_inited = 1;
}

#ifdef ALCS_ENABLED
void utc_get(char *utc, char* tick_on_get_utc)
{
    if (ntp_time_s == 0)
        return;
    memcpy(utc, str_time, sizeof(str_time));
    memset(tick_on_get_utc, 0, 16);
    uint64_t split = 1000000;
    snprintf(tick_on_get_utc, 16, "%d%06d", (uint32_t)(up_tick_on_get_utc/split), (uint32_t)(up_tick_on_get_utc%split));
    // TS_INFO("utc_get=%s, utc=%s", tick_on_get_utc, utc);
}
void utc_set(char *tick, char *sync_utc, char* utc_base)
{
    uint64_t alcs_tick = 0, alcs_sync_utc = 0, alcs_utc_base = 0;
    char alcs_utc[16] = {0};
    // TS_INFO("tick=%s, utc_base=%s, utc=%s", tick, utc_base, sync_utc);
    if (ntp_time_s != 0)
        return;
    alcs_tick = strtoll(tick, NULL, 10);
    alcs_sync_utc = strtoll(sync_utc, NULL, 10);
    alcs_utc_base = strtoll(utc_base, NULL, 10);
    ntp_time_s = (uint32_t)((alcs_sync_utc + alcs_tick - alcs_utc_base + 20)/1000);

    snprintf(alcs_utc, 16, "%d%03d", ntp_time_s, (alcs_sync_utc + alcs_tick - alcs_utc_base + 20)%1000);
    _ntp_update(alcs_utc);
}
#endif

static const char** g_devicetimer_target_list = NULL;
static int g_num_devicetimer_list = 0;
static devicetimer_callback g_timer_service_cb = NULL;

int aiot_device_timer_init(const char **devicetimer_list, uint8_t num_devicetimer_list, devicetimer_callback timer_service_cb) {
    if (devicetimer_list == NULL || timer_service_cb == NULL)
        return -1;
    g_devicetimer_target_list = devicetimer_list;
    g_num_devicetimer_list = num_devicetimer_list;
    g_timer_service_cb = timer_service_cb; 
    linkkit_ntp_time_request(_ntp_update);
    #ifndef SDK_VER_16X
    aiot_al_sleep_ms(100);
    #else
    HAL_SleepMs(100);
    #endif

#ifdef ALCS_ENABLED
    extern void *iotx_tick_notify_init();
    iotx_tick_notify_init();
#endif

    if (aiot_device_timer_inited() == 1)
        return 1;
    else {
        #ifndef SDK_VER_16X
        aiot_al_sleep_ms(500);
        #else
        HAL_SleepMs(500);
        #endif
        return aiot_device_timer_inited();
    }
}
int aiot_device_timer_deinit() {
    g_inited = 0;
    int j = 0;
    for (j = 0; j < DeviceTimerSize; j++) {
        if (g_device_timer[j].targets != NULL) {
            #ifndef SDK_VER_16X
            aiot_al_free(g_device_timer[j].targets);
            #else
            HAL_Free(g_device_timer[j].targets);
            #endif
        }
    }
    memset(g_device_timer, 0, sizeof(g_device_timer));
    return 0;
}

int aiot_device_timer_clear(void) {
    aiot_device_timer_deinit();
    #ifndef SDK_VER_16X
    aiot_kv_del(DEVICETIMER);
    #else
    HAL_Kv_Del(DEVICETIMER);
    #endif
    return 0;
}
static int parse_action(int type, const char* action, int post) {
    int  i = 0;
    char *p = NULL, *val = NULL;
    DS_DEBUG("parse_action=%s\r\n", action);
    for (i = 0; i < g_num_devicetimer_list; i++) {
        p = strstr(action, g_devicetimer_target_list[i]);
        if (p != NULL && action[strlen(g_devicetimer_target_list[i])] == ':') {
            // val = atoi(&action[strlen(g_devicetimer_target_list[i])] + 1);
            val = (char*)(&action[strlen(g_devicetimer_target_list[i])] + 1);
            g_timer_service_cb(NULL, g_devicetimer_target_list[i], val);
        }
    }
    return 0;
}
static void device_timer_action(int type, const char* action, int run_sleep, int post) {
    char* p = NULL;
    int count = 0, i = 0, pre_str_start = 0;

    for (i = 0; i < strlen(action); i++) {
        if (action[i] == ',')
            count++;
    }
    DS_DEBUG("count=%d\r\n", count);

    if (type == 1 || type == 2) { // 倒计时、普通定时
        if (strchr(action, '|') != NULL || strlen(action) < 3)
            return;
        if (count == 0) { // 只设置一个属性
            parse_action(type, action, post);
        } else {
            for (i = 0; i < strlen(action) + 1; i++) {
                if (action[i] == ',' || action[i] == '\0') {
                    DS_DEBUG("loc=%d %d \r\n", i, pre_str_start);
                    #ifndef SDK_VER_16X
                    p = aiot_al_malloc(i + 1 - pre_str_start, LINKKIT_DEMO_MODULE_NAME);
                    #else
                    p = (char *)HAL_Malloc(i + 1 - pre_str_start);
                    #endif
                    memset(p, 0, i + 1 - pre_str_start);
                    memcpy(p, &action[pre_str_start], i - pre_str_start);
                    parse_action(type, p, post);
                    #ifndef SDK_VER_16X
                    aiot_al_free(p);
                    #else
                    HAL_Free(p);
                    #endif
                    pre_str_start = i + 1;
                }
            }
        }
    } else if (type == 3) { // 循环定时
        if (strchr(action, '|') == NULL)
            return;

        if (run_sleep == 0) { //run
            if (count == 0) {
                for (i = 0; i < strlen(action) + 1; i++) {
                    if (action[i] == '|') {
                        #ifndef SDK_VER_16X
                        p = aiot_al_malloc(i + 1, LINKKIT_DEMO_MODULE_NAME);
                        #else
                        p = (char *)HAL_Malloc(i + 1);
                        #endif
                        memset(p, 0, i + 1);
                        memcpy(p, action, i);
                        parse_action(type, p, post);
                        #ifndef SDK_VER_16X
                        aiot_al_free(p);
                        #else
                        HAL_Free(p);
                        #endif
                    }
                }
            }
        } else { //sleep
            if (count == 0) {
                for (i = 0; i < strlen(action) + 1; i++) {
                    if (action[i] == '|') {
                        #ifndef SDK_VER_16X
                        p = aiot_al_malloc(strlen(action) - i, LINKKIT_DEMO_MODULE_NAME);
                        #else
                        p = (char *)HAL_Malloc(strlen(action) - i);
                        #endif
                        memset(p, 0, strlen(action) - i);
                        memcpy(p, &action[i+1], strlen(action) - i - 1);
                        parse_action(type, p, post);
                        #ifndef SDK_VER_16X
                        aiot_al_free(p);
                        #else
                        HAL_Free(p);
                        #endif
                    }
                }
            }
        }
    } else {  // 随机定时
        // todo
    }
}

static void timer_ntp_sync(void *arg)
{
    linkkit_ntp_time_request(_ntp_update);
}

void timer_ntp_sync_post(void)
{
    aos_post_delayed_action(100, timer_ntp_sync, NULL);
}

static int _sec_next_event = -1;
static void device_timer_runner(void) {
    int i = 0, j = 0;
    int current_time = -1;
    int n_run_and_sleep = 0;
    int mod_run_and_sleep = 0;
    // int tmp = 0;
    uint32_t ticket = _get_uptime() - uptime_on_get_utc;
    if (ntp_time_s == 0 || (ticket > 24*60*60/4 && (_sec_next_event == -1 || _sec_next_event - utc_week_second_offset > 10))) {
        if (ntp_time_s == 0 || ticket < 0){
            // EXAMPLE_TRACE("ERR: must sync ntp time. ticket=%d, %d now=%d!\n", ticket, uptime_on_get_utc, _get_uptime());
            DS_ERR("TS_ERR: must sync ntp time. ticket=%d, %d now=%d!\r\n", ticket, uptime_on_get_utc, _get_uptime());
            timer_ntp_sync_post();
            return;
        }
        timer_ntp_sync_post();
        if (pre_ticket == 0 || ticket - pre_ticket > 24*60*60/6) {
            pre_ticket = ticket;
            get_timer_from_kv();
        }
    }
    _sec_next_event = -1;
    utc_week_second_offset = (utc + ticket) % 604800;
    DS_INFO("TS_offset = %d  uptime=%d uptime_on_get_utc=%d\r\n", utc_week_second_offset, ticket, uptime_on_get_utc);

    for (i = 0; i < DeviceTimerSize; i++){
        if (g_device_timer[i].enable == 1 && g_device_timer[i].type > 0){
            for ( j = 0; j < DAYS_OF_WEEK; j++){
                // 倒计时和普通定时 type == 1 or type == 2
                if (g_device_timer[i].type == 1 || g_device_timer[i].type == 2) {
                    if ((_sec_next_event == -1 || g_device_timer[i].offset_start[j] <= _sec_next_event) && 
                        g_device_timer[i].offset_start[j] > utc_week_second_offset) {
                        _sec_next_event = g_device_timer[i].offset_start[j];
                        g_device_timer[i].event_ready[j] = 1;
                    }
                    if (g_device_timer[i].offset_start[j] == utc_week_second_offset || 
                        (g_device_timer[i].event_ready[j] == 1 && utc_week_second_offset > g_device_timer[i].offset_start[j] && 
                        utc_week_second_offset - g_device_timer[i].offset_start[j] < 5 )){
                            if (g_device_timer[i].repeat == 0)
                                g_device_timer[i].enable = 0;
                            g_device_timer[i].event_ready[j] = 0;
                            // todo 本地定时
                            DS_DEBUG("DO: repeat=%d %s %d %d\r\n", g_device_timer[i].repeat, g_device_timer[i].targets, i, j);
                            device_timer_action(g_device_timer[i].type, g_device_timer[i].targets, 0, 0);
                            continue;
                    }
                }
                // 循环定时 type == 3
                if (g_device_timer[i].type == 3) {
                    int offset_start = g_device_timer[i].offset_start[j], offset_end = g_device_timer[i].offset_end[j];

                    //未进入循环定时窗口期，设置起始时间为定时配置
                    if (utc_week_second_offset < offset_start && offset_end > offset_start && (_sec_next_event >= offset_start || _sec_next_event == -1)){ 
                        // printf("TS_period00 next = %d  weed = %d\r\n", _sec_next_event, utc_week_second_offset);
                        // printf("TS_period00 set = %d  end = %d\r\n", offset_start, offset_end);
                            _sec_next_event = offset_start;
                            g_device_timer[i].event_ready[j] = 1;

                    } else if ((offset_start <= utc_week_second_offset && offset_end >= utc_week_second_offset) ||
                                (offset_start >= 518460 && offset_end < offset_start && (utc_week_second_offset >= offset_start || utc_week_second_offset <= offset_end))) { 
                    // 进入窗口期，分当天、夸天（夸周六、周日需特殊处理）

                        // printf("TS_period next = %d  weed = %d\r\n", _sec_next_event, utc_week_second_offset);
                        // printf("TS_period set = %d  end = %d\r\n", offset_start, offset_end);
                        if (offset_start >= 518460 && utc_week_second_offset < offset_start && offset_end < offset_start) // 周六且夸天的循环定时
                            n_run_and_sleep = (utc_week_second_offset + 604800 - offset_start)/(g_device_timer[i].runTime + g_device_timer[i].sleepTime);
                        else
                            n_run_and_sleep = (utc_week_second_offset - offset_start)/(g_device_timer[i].runTime + g_device_timer[i].sleepTime);
                        // mod_run_and_sleep = (utc_week_second_offset - offset_start)%(g_device_timer[i].runTime + g_device_timer[i].sleepTime);
                        mod_run_and_sleep = (offset_start + (g_device_timer[i].runTime + g_device_timer[i].sleepTime) * n_run_and_sleep)%604800;


                        if (utc_week_second_offset >= offset_start && (offset_start >= 518460 || utc_week_second_offset <= offset_end))
                            current_time = utc_week_second_offset - (offset_start + (g_device_timer[i].runTime + g_device_timer[i].sleepTime) * n_run_and_sleep);
                        else if (utc_week_second_offset < offset_start && (offset_start >= 518460 && offset_start > offset_end && utc_week_second_offset <= offset_end + 5))
                            current_time = utc_week_second_offset + 604800 - (offset_start + (g_device_timer[i].runTime + g_device_timer[i].sleepTime) * n_run_and_sleep);

                        // printf("TS_period next = %d  weed = %d\r\n", _sec_next_event, utc_week_second_offset);
                        printf("TS_period curr = %d  mod = %d  ready=%d\r\n", current_time, mod_run_and_sleep, g_device_timer[i].event_ready[j]);
                        if (current_time >= 0 && current_time <= 5 && g_device_timer[i].event_ready[j] && (utc_week_second_offset >= offset_start || 
                            (utc_week_second_offset < offset_start && offset_start >= 518460 && offset_start > offset_end && utc_week_second_offset <= offset_end + 5))){
                            g_device_timer[i].event_ready[j] = 0;
                            printf("DO: %s run  %d  %d\r\n", g_device_timer[i].targets, i, j);
                            device_timer_action(g_device_timer[i].type, g_device_timer[i].targets, 0, 0);
                        }
                        else if (current_time >= g_device_timer[i].runTime && current_time <= g_device_timer[i].runTime + 5 && g_device_timer[i].event_ready[j] && (utc_week_second_offset >= offset_start || 
                            (utc_week_second_offset < offset_start && offset_start >= 518460 && offset_start > offset_end && utc_week_second_offset <= offset_end + 5)) ) {
                            g_device_timer[i].event_ready[j] = 0;
                            printf("DO: %s sleep  %d  %d\r\n", g_device_timer[i].targets, i, j);
                            device_timer_action(g_device_timer[i].type, g_device_timer[i].targets, 1, 0);
                        }
                        if ((_sec_next_event >= (mod_run_and_sleep + g_device_timer[i].runTime)%604800 || _sec_next_event == -1) 
                            && utc_week_second_offset >= mod_run_and_sleep + g_device_timer[i].runTime) {
                            _sec_next_event = mod_run_and_sleep + g_device_timer[i].runTime + g_device_timer[i].sleepTime;
                            g_device_timer[i].event_ready[j] = 1;
                        } else if ((_sec_next_event >= mod_run_and_sleep || _sec_next_event == -1) && utc_week_second_offset >= mod_run_and_sleep) {
                            _sec_next_event = mod_run_and_sleep + g_device_timer[i].runTime;
                            g_device_timer[i].event_ready[j] = 1;
                        }
                    }
                }
                // 循环定时 type == 4  
                //todo
            }
        }
    }
    ticket = _get_uptime()- uptime_on_get_utc;
    utc_week_second_offset = (utc + ticket) % (86400 * 7);
    int delay_sec = _sec_next_event - utc_week_second_offset;
    DS_INFO("TS_delay=%d\r\n", delay_sec);
    #ifndef SDK_VER_16X
    if (delay_sec == 0) {
        aiot_al_sleep_ms(400);
        device_timer_runner();
    } else if (delay_sec > 0 && delay_sec < 5*60){
        aiot_al_timer_stop(check_timer);
        aiot_al_timer_start(check_timer, delay_sec * 1000);
    } else {
        aiot_al_timer_stop(check_timer);
        aiot_al_timer_start(check_timer, 5*60 * 1000);
    }
    #else
    if (delay_sec == 0) {
        HAL_SleepMs(400);
        device_timer_runner();
    } else if (delay_sec > 0 && delay_sec < 5*60){
        HAL_Timer_Stop(check_timer);
        HAL_Timer_Start(check_timer, delay_sec * 1000);
    } else {
        HAL_Timer_Stop(check_timer);
        HAL_Timer_Start(check_timer, 5*60 * 1000);
    }
    #endif
}

int string2minute(const char *string)
{
    char hour[3], min[3];
    int offset;
    if (string[0] > '9' || string[1] > '9' || string[3] > '9' || string[4] > '9')
        return -1;
    strncpy(&hour[0], &string[0], 2);
    strncpy(&min[0], &string[3], 2);

    offset = atoi(hour) * 60;
    offset += atoi(min);

    return offset;
}

static int days_calc(int years, int months, int day) {
    int i = 0, days = 0;
    for (i = 0; i <= months % 12; i++){
        if (i == 0) // 1月份
            days = day - 3;
        else if (i == 1) // 2月份
            days = days + 31;
        else if (i == 2) // 3月份
            days = days + 28;
        else if (i == 3) // 4月份
            days = days + 31;
        else if (i == 4) // 5月份
            days = days + 30;
        else if (i == 5) // 6月份
            days = days + 31;
        else if (i == 6) // 7月份
            days = days + 30;
        else if (i == 7) // 8月份
            days = days + 31;
        else if (i == 8) // 9月份
            days = days + 31;
        else if (i == 9) // 10月份
            days = days + 30;
        else if (i == 10) // 11月份
            days = days + 31;
        else if (i == 11) // 12月份
            days = days + 30;
    }
    // 计算年
    if (months % 12 >= 3) {// 2024年3月1日及之后
        if (years < 3) // 未经历闰年
            days +=  years * 365;
        else {
            days +=  years * 365 + (years + 1) / 4;
        }
    } else {// 2024年2月29日及之前
        if (years <= 3) // 未经历闰年
            days +=  years * 365;
        else {
            days +=  years * 365 + (years) / 4;
        }
    }
    DS_DEBUG("days=%d\r\n", days);
    return days;
}

static int cron_parse(char *data, uint8_t index, uint8_t type, char* end){
    const char *err = NULL;
    char cron_str[32];
    cron_expr target;
    cron_str[0] = '*';
    cron_str[1] = ' ';
    int offset_end = 0;
    
    strcpy(g_device_timer[index].timer, data);
    memset(cron_str, 0, sizeof(cron_str));
    snprintf(cron_str, sizeof(cron_str), "* %s", data);
    memset(&target, 0, sizeof(cron_expr));
    cron_parse_expr(cron_str, &target, &err);
    if (err){
        DS_ERR("TS_parse err:%s\r\n", err);
        // aiot_kv_del(DeviceTimer);
        return -10;
    }

    int i = 0, hour = 0, minute = 0, day = 0, month = 0, year = 0, offset = 0;
    for (i = 0; i < 24; i++) {
        if (cron_get_bit(target.hours, i)) {
            hour = i;
            DS_DEBUG("hour:%d\r\n", hour);
            break;
        }
    }

    for (i = 0; i < 60; i++) {
        if (cron_get_bit(target.minutes, i)) {
            minute = i;
            DS_DEBUG("minute:%d\r\n", minute);
            break;
        }
    }
    // 最后一个cron字段为年，*表示未指定年
    if (cron_str[strlen(cron_str) - 1] == '*'){
        g_device_timer[index].repeat = 0;
        for (i = 0; i < DAYS_OF_WEEK; i++) {
            if (cron_get_bit(target.days_of_week, i)) {
                offset = ((i * 24 + hour) * 60 * 60 + minute * 60 + 28800 - g_device_timer[index].timezone)%604800;
                if (type == 3 || type == 4) {
                    offset_end = ((i * 24) * 60 * 60 + string2minute(g_device_timer[index].endTime) * 60 
                                  + 28800 - g_device_timer[index].timezone)%604800;
                    if (offset_end < offset) // 结束时间小于起始时间，跨天配置
                        g_device_timer[index].offset_end[g_device_timer[index].repeat] = (offset_end + 24*60*60)%604800;
                    else 
                        g_device_timer[index].offset_end[g_device_timer[index].repeat] = offset_end;
                }
                DS_DEBUG("offset=%d end=%d repeat=%d\r\n", offset, g_device_timer[index].offset_end[g_device_timer[index].repeat], g_device_timer[index].repeat);
                g_device_timer[index].offset_start[g_device_timer[index].repeat++] = offset;
            }
        }

        if (g_device_timer[index].repeat == 0) {
            for (i = 0; i <= DAYS_OF_WEEK; i++) {
                offset = (((i * 24 + hour) * 60 * 60 + minute * 60 ) + 28800 - g_device_timer[index].timezone) % 604800;
                if (offset > 0) { // 大于当前时间
                    g_device_timer[index].offset_start[0] = offset;
                    DS_DEBUG("TS_offset=%d\r\n", g_device_timer[index].offset_start[0]);
                    break;
                } else { // 小于当前时间，第二天执行
                    g_device_timer[index].offset_start[0] = (offset + 24*60*60) % 604800;
                    DS_DEBUG("TS_offset=%d\r\n", g_device_timer[index].offset_start[0]);
                    break;
                }
            }
        }
    } else { // 指定了年，以指定日期，单次定时执行
        for (i = 0; i <= 31; i++) {
            if (cron_get_bit(target.days_of_month, i)) {
                day = i;
                DS_DEBUG("day:%d\r\n", day);
                break;
            }
        }

        for (i = 0; i < 12; i++) {
            if (cron_get_bit(target.months, i)) {
                month = i + 1;
                DS_DEBUG("month:%d\r\n", month);
                break;
            }
        }
        year = atoi(&cron_str[strlen(cron_str) - 4]);
        if (year < 2021 || (year == 2021 && month == 1 && day < 4) || month > 12 || day > 31)
            return -2;
        // printf("year:%d\r\n", year);
        // 计算指定日期的utc值。先月份差. 1609574400=2021.01.03 16:00(北京时间)  1609603200 2021.01.03 00:00(北京时间)
        // 31: 1,3,5,7,8,10,12;  30: 4,6,9,11;  28:28/29   31*7+30*5+28=365
        int months = 0, days = 0;
        months = (year - 2021) * 12 + month - 1;
        days = days_calc(year - 2021, months, day);
        int day_2_utc = days * 86400 + hour * 3600 + minute * 60 + 28800 - g_device_timer[index].timezone + WEEK_START;
            DS_DEBUG("uptime=%d  up_on_get=%d\r\n", _get_uptime(), uptime_on_get_utc);
        
        int timer_once = day_2_utc - ntp_time_s - (_get_uptime() - uptime_on_get_utc);
        if ( timer_once > 0 && timer_once < 604800) { // 指定日期大于当前时间，且小于一周的，可以配置。
            g_device_timer[index].offset_start[0] = (day_2_utc - (ntp_time_s - utc))%604800;
            DS_DEBUG("TS_offset=%d\r\n", g_device_timer[index].offset_start[0]);
        } else {
            g_device_timer[index].enable = 0;
        }
    }
    
    return 0;
}
// input: json数据；
// src: 数据来源：0 --云端； 1--KV
// save:是否更新kv数据
int deviceTimerParse(const char *input, uint8_t src, int save){
    cJSON *root, *item_JSON, *list, *prop;
    int ret = 0, j = 0;
    root = cJSON_Parse(input);
    if (root == NULL && !cJSON_IsObject(root)) {
        ret = -1;
        goto err;
    }

    list = cJSON_GetObjectItem(root, DEVICETIMER);
    int local_timer_arrySize = cJSON_GetArraySize(list);
    if (DeviceTimerSize != local_timer_arrySize) {
        ret = -3;
        goto err;
    }
    utc_week_second_offset = (utc + _get_uptime() - uptime_on_get_utc) % (86400 * 7);
    for (j = 0; j < DeviceTimerSize; j++) {
        if (g_device_timer[j].targets != NULL) {
            #ifndef SDK_VER_16X
            aiot_al_free(g_device_timer[j].targets);
            #else
            HAL_Free(g_device_timer[j].targets);
            #endif
        }
    }
    memset(g_device_timer, 0, sizeof(g_device_timer));
    for (j = 0; j < local_timer_arrySize; j++) {
        prop = cJSON_GetArrayItem(list, j);

        item_JSON = cJSON_GetObjectItem(prop, "Z"); //TimeZone
        if (item_JSON != NULL && cJSON_IsNumber(item_JSON)) {
            g_device_timer[j].timezone = item_JSON->valueint;
            // DS_DEBUG("TimeZone:%d", item_JSON->valueint);
        }

        item_JSON = cJSON_GetObjectItem(prop, "E"); //Enable
        if (item_JSON != NULL && cJSON_IsNumber(item_JSON)) {
            g_device_timer[j].enable = item_JSON->valueint;
            // DS_DEBUG("Enable:%d", item_JSON->valueint);
        }

        item_JSON = cJSON_GetObjectItem(prop, "A"); // Targets
        if (item_JSON != NULL && strlen(item_JSON->valuestring)) {
            #ifndef SDK_VER_16X
            g_device_timer[j].targets = aiot_al_malloc(strlen(item_JSON->valuestring) + 1, LINKKIT_DEMO_MODULE_NAME);
            #else
            g_device_timer[j].targets = (char *)HAL_Malloc(strlen(item_JSON->valuestring) + 1);
            #endif
            memset(g_device_timer[j].targets, 0, strlen(item_JSON->valuestring) + 1);
            strcpy(g_device_timer[j].targets, item_JSON->valuestring);
            // DS_DEBUG("Targets:%s\r\n", item_JSON->valuestring);
        }

        item_JSON = cJSON_GetObjectItem(prop, "Y"); //Type
        if (item_JSON != NULL && cJSON_IsNumber(item_JSON)) {
            g_device_timer[j].type = item_JSON->valueint;
            // DS_DEBUG("Type:%d", item_JSON->valueint);
        }
        if (g_device_timer[j].type == 3 || g_device_timer[j].type == 4) {// period or random timer
            item_JSON = cJSON_GetObjectItem(prop, "N"); // EndTime
            if (item_JSON != NULL && strlen(item_JSON->valuestring)) {
                strcpy(g_device_timer[j].endTime, item_JSON->valuestring);
                // DS_DEBUG("EndTime:%s\r\n", item_JSON->valuestring);
            }
        }
        if (g_device_timer[j].type == 3) { // period or random timer
            item_JSON = cJSON_GetObjectItem(prop, "R"); //RunTime
            if (item_JSON != NULL && cJSON_IsNumber(item_JSON)) {
                g_device_timer[j].runTime = item_JSON->valueint;
                // DS_DEBUG("RunTime:%d\r\n", item_JSON->valueint);
            }

            item_JSON = cJSON_GetObjectItem(prop, "S"); //SleepTime
            if (item_JSON != NULL && cJSON_IsNumber(item_JSON)) {
                g_device_timer[j].sleepTime = item_JSON->valueint;
                // DS_DEBUG("SleepTime:%d\r\n", item_JSON->valueint);
            }
        }

        if (g_device_timer[j].type == 4) { // random timer
            // todo
            // g_device_timer[j].runTime = 300; // 随机值 todo
            // g_device_timer[j].sleepTime = 600; // 随机值 todo
        }

        item_JSON = cJSON_GetObjectItem(prop, "T"); //Timer
        if (item_JSON != NULL && strlen(item_JSON->valuestring) > 5) {
            ret = cron_parse(item_JSON->valuestring, j, g_device_timer[j].type, g_device_timer[j].endTime);
            if (ret != 0) {
                goto err;
            }
            strcpy(g_device_timer[j].timer, item_JSON->valuestring);
            DS_DEBUG("Timer:%s\r\n", item_JSON->valuestring);
        }
    }
    cJSON_Delete(root);
    if (ret == 0 && save == 1){
        #ifndef SDK_VER_16X
        aiot_kv_set(DEVICETIMER, input, strlen(input), 1);
        #else
        HAL_Kv_Set(DEVICETIMER, input, strlen(input), 1);
        #endif
    }
    device_timer_runner();
    return 0;

    err:
        DS_ERR("deviceTimerParse err=%d", ret);
        cJSON_Delete(root);
        aiot_device_timer_clear();
    return ret;
}

char* device_timer_post(int save) {
    cJSON *root = NULL, *list = NULL, *items[DeviceTimerSize] = {NULL};
    int ret = 0, i = 0;
    root = cJSON_CreateObject();
    if (root == NULL){
        ret = -1;
        goto err;
    }
    list = cJSON_CreateArray();
    if (list == NULL){
        cJSON_Delete(root);
        ret = -2;
        goto err;
    }
    for (i =0; i < DeviceTimerSize; i++) {
        items[i] = cJSON_CreateObject();
        if (items[i] == NULL){
            cJSON_Delete(list);
            cJSON_Delete(root);
            ret = -3;
            goto err;
        }

        cJSON_AddStringToObject(items[i], "T", g_device_timer[i].timer);     // Timer
        cJSON_AddNumberToObject(items[i], "E", g_device_timer[i].enable);    // Enable
        cJSON_AddNumberToObject(items[i], "Y", g_device_timer[i].type);      // Type
        cJSON_AddStringToObject(items[i], "A", g_device_timer[i].targets);   // Targets
        cJSON_AddNumberToObject(items[i], "Z", g_device_timer[i].timezone);  // TimeZone

        cJSON_AddStringToObject(items[i], "N", g_device_timer[i].endTime);   // EndTime
        cJSON_AddNumberToObject(items[i], "R", g_device_timer[i].runTime);   // RunTime
        cJSON_AddNumberToObject(items[i], "S", g_device_timer[i].sleepTime); // SleepTime
        cJSON_AddItemToArray(list, items[i]);
    }
    cJSON_AddItemToObject(root, DEVICETIMER, list);
    char *property = cJSON_PrintUnformatted(root);
    if (property == NULL) {
        cJSON_Delete(root);
        ret = -8;
        goto err;
    }
    cJSON_Delete(root);
    DS_INFO("TS_post:%s\r\n", property);
    if (save) {
        #ifndef SDK_VER_16X
        aiot_kv_set(DEVICETIMER, property, strlen(property), 1);
        #else
        HAL_Kv_Set(DEVICETIMER, property, strlen(property), 1);
        #endif
    }

    return property;

    err:
        DS_ERR("err ret=%d", ret);
        return NULL;
}
#endif
