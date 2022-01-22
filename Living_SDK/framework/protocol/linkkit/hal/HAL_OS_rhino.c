/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <aos/aos.h>
#include <hal/wifi.h>
#include <hal/hal.h>

#include "activation.h"
#include "chip_code.h"
#include "iot_import.h"
#include "iot_import_product.h"
#include "utils_sysinfo.h"

#define DEFAULT_THREAD_PRI AOS_DEFAULT_APP_PRI

#define _RHINO_SDK_DEMO__ 1

#define PLATFORM_LINUX_LOG(format, ...)                              \
    do {                                                             \
        printf("Linux:%s:%d %s()| " format "\n", __FILE__, __LINE__, \
               __FUNCTION__, ##__VA_ARGS__);                         \
        fflush(stdout);                                              \
    } while (0);

void *HAL_MutexCreate(void)
{
    aos_mutex_t mutex;
    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return mutex.hdl;
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_free((aos_mutex_t *)&mutex);
    }
}

void HAL_MutexLock(_IN_ void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
    }
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
    if (NULL != mutex) {
        aos_mutex_unlock((aos_mutex_t *)&mutex);
    }
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    return aos_malloc(size);
}

void *HAL_Realloc(_IN_ void *ptr, _IN_ uint32_t size)
{
    return aos_realloc(ptr, size);
}

// void *HAL_Calloc(_IN_ uint32_t nmemb, _IN_ uint32_t size)
// {
//     return aos_calloc(nmemb, size);
// }

void HAL_Free(_IN_ void *ptr)
{
    aos_free(ptr);
}

void HAL_Reboot(void)
{
    aos_reboot();
}

int aliot_platform_ota_start(const char *md5, uint32_t file_size)
{
    printf("this interface is NOT support yet.");
    return -1;
}

int aliot_platform_ota_write(_IN_ char *buffer, _IN_ uint32_t length)
{
    printf("this interface is NOT support yet.");
    return -1;
}

int aliot_platform_ota_finalize(_IN_ int stat)
{
    printf("this interface is NOT support yet.");
    return -1;
}

uint64_t HAL_UptimeMs(void)
{
    return aos_now_ms();
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
    aos_msleep(ms);
}

uint64_t aliot_platform_time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}


int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}


int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format,
                  va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

extern int LITE_get_loglevel(void);

void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;

#ifdef AOS_DISABLE_ALL_LOGS
    if (0 == log_get_enable_aos_log_flag()) return;
#endif

    if (LITE_get_loglevel() == AOS_LL_V_NONE) return;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}


void HAL_Srandom(uint32_t seed)
{
    // srandom(seed);
    srand(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    return (region > 0) ? (rand() % region) : 0;
    // return 0;
}


void *HAL_SemaphoreCreate(void)
{
    aos_sem_t sem;

    if (0 != aos_sem_new(&sem, 0)) {
        return NULL;
    }

    return sem.hdl;
}

void HAL_SemaphoreDestroy(_IN_ void *sem)
{
    aos_sem_free((aos_sem_t *)&sem);
}

void HAL_SemaphorePost(_IN_ void *sem)
{
    aos_sem_signal((aos_sem_t *)&sem);
}

int HAL_SemaphoreWait(_IN_ void *sem, _IN_ uint32_t timeout_ms)
{
    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        return aos_sem_wait((aos_sem_t *)&sem, AOS_WAIT_FOREVER);
    } else {
        return aos_sem_wait((aos_sem_t *)&sem, timeout_ms);
    }
}

typedef struct {
    aos_task_t task;
    int        detached;
    void      *arg;
    void *(*routine)(void *arg);
} task_context_t;

static void task_wrapper(void *arg)
{
    task_context_t *task = arg;

    if (task == NULL)
    {
        printf("HAL_CreateThread arg is NULL\r\n");
        return;
    }

    task->routine(task->arg);

    if (task) {
        aos_free(task);
        task = NULL;
    }
}

#define DEFAULT_THREAD_NAME "AosThread"
#define DEFAULT_THREAD_SIZE 4096
int HAL_ThreadCreate(_OU_ void **thread_handle,
                     _IN_ void *(*work_routine)(void *), _IN_ void *arg,
                     _IN_ hal_os_thread_param_t *hal_os_thread_param,
                     _OU_ int                   *stack_used)
{
    int ret = -1;
    if (stack_used != NULL) {
        *stack_used = 0;
    }
    char *tname;
    size_t ssiz;
    int    detach_state = 0;
    int    priority;

    if (hal_os_thread_param) {
        detach_state = hal_os_thread_param->detach_state;
    }
    if (!hal_os_thread_param || !hal_os_thread_param->name) {
        tname = DEFAULT_THREAD_NAME;
    } else {
        tname = hal_os_thread_param->name;
    }

    if (!hal_os_thread_param || hal_os_thread_param->stack_size == 0) {
        ssiz = DEFAULT_THREAD_SIZE;
    } else {
        ssiz = hal_os_thread_param->stack_size;
    }

    if (!hal_os_thread_param || hal_os_thread_param->priority == 0) {
        priority = DEFAULT_THREAD_PRI;
    } else if (hal_os_thread_param->priority < os_thread_priority_idle ||
               hal_os_thread_param->priority > os_thread_priority_realtime) {
        priority = DEFAULT_THREAD_PRI;
    } else {
        priority = DEFAULT_THREAD_PRI - hal_os_thread_param->priority;
    }

    task_context_t *task = aos_malloc(sizeof(task_context_t));
    if (!task) {
        return -1;
    }
    memset(task, 0, sizeof(task_context_t));

    task->arg      = arg;
    task->routine  = work_routine;
    task->detached = detach_state;

    ret = aos_task_new_ext(&task->task, tname, task_wrapper, task, ssiz,
                           priority);

    *thread_handle = (void *)task;

    return ret;
}

void HAL_ThreadDetach(_IN_ void *thread_handle)
{
    task_context_t *task = thread_handle;
    task->detached       = 1;
}

void HAL_ThreadDelete(_IN_ void *thread_handle)
{
}

void HAL_Firmware_Persistence_Start(void) {}

int HAL_Firmware_Persistence_Write(_IN_ char *buffer, _IN_ uint32_t length)
{

    return 0;
}

int HAL_Firmware_Persistence_Stop(void)
{
    return 0;
}


int HAL_Config_Write(const char *buffer, int length)
{
    if (!buffer || length <= 0) {
        return -1;
    }

    return aos_kv_set("alink", buffer, length, 1);
}

int HAL_Config_Read(char *buffer, int length)
{
    if (!buffer || length <= 0) {
        return -1;
    }

    return aos_kv_get("alink", buffer, &length);
}

typedef struct {
    const char *name;
    int         ms;
    aos_call_t  cb;
    void       *data;
} schedule_timer_t;


static void schedule_timer(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata = p;
    aos_post_delayed_action(pdata->ms, pdata->cb, pdata->data);
}

static void schedule_timer_cancel(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata = p;
    aos_cancel_delayed_action(-1, pdata->cb, pdata->data);
}

static void schedule_timer_delete(void *p)
{
    if (p == NULL) {
        return;
    }

    schedule_timer_t *pdata = p;
    aos_cancel_delayed_action(-1, pdata->cb, pdata->data);
    aos_free(p);
}

#define USE_YLOOP
void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data)
{
#ifdef USE_YLOOP
    schedule_timer_t *timer =
                (schedule_timer_t *)aos_malloc(sizeof(schedule_timer_t));
    if (timer == NULL) {
        return NULL;
    }

    timer->name = name;
    timer->cb   = func;
    timer->data = user_data;

    return timer;
#else
    return NULL;
#endif
}

int HAL_Timer_Start(void *t, int ms)
{
#ifdef USE_YLOOP
    if (t == NULL) {
        return -1;
    }
    schedule_timer_t *timer = t;
    timer->ms               = ms;
    return aos_schedule_call(schedule_timer, t);
#else
    return 0;
#endif
}

int HAL_Timer_Stop(void *t)
{
#ifdef USE_YLOOP
    if (t == NULL) {
        return -1;
    }

    return aos_schedule_call(schedule_timer_cancel, t);
#else
    return 0;
#endif
}
int HAL_Timer_Delete(void *timer)
{
#ifdef USE_YLOOP
    if (timer == NULL) {
        return -1;
    }
    return aos_schedule_call(schedule_timer_delete, timer);
#else
    return 0;
#endif
}


static int64_t delta_ms = 0;
void           HAL_UTC_Set(long long ms)
{
    delta_ms = ms - aos_now_ms();
}

long long HAL_UTC_Get(void)
{
    long long ret = aos_now_ms() + delta_ms;
    return ret;
}


int get_aos_hex_version(const char *str, unsigned char hex[VERSION_NUM_SIZE])
{
    char *p           = NULL;
    char *q           = NULL;
    int   i           = 0;
    char  str_ver[32] = { 0 };
    if (str == NULL) {
        return -1;
    }
    if (hex == NULL) {
        return -1;
    }
    strncpy(str_ver, str, sizeof(str_ver) - 1);
    p = strtok(str_ver, "-");
    for (i = 0; i < 2; i++) {
        if (p == NULL) {
            return -1;
        }
        p = strtok(NULL, "-");
    }

    q = strtok(p, ".");
    for (i = 0; i < 4; i++) {
        if (q == NULL) {
            break;
        } else {
            hex[i] = atoi(q);
        }
        q = strtok(NULL, ".");
    }
    return 0;
}


/**
 * 激活使用，提供kernel版本号字节数组
 */
void aos_get_version_hex(unsigned char version[VERSION_NUM_SIZE])
{
    memset(version, 0, VERSION_NUM_SIZE);
    get_aos_hex_version(aos_version_get(), version);
}


/**
 * 激活使用，提供用字节数组表示mac地址，非字符串
 */
void aos_get_mac_hex(unsigned char mac[MAC_ADDRESS_SIZE])
{
    memset(mac, 0, MAC_ADDRESS_SIZE);
    hal_wifi_get_mac_addr(NULL, mac);
    // return mac;
}

/**
 * 激活使用，提供用字节数组表示芯片ID，非字符串
 */
void aos_get_chip_code(unsigned char chip_code[CHIP_CODE_SIZE])
{
    memset(chip_code, 0, CHIP_CODE_SIZE);
    // MCU_ID import by -D option
    chip_code_st *p_chip_code_obj = get_chip_code(MCU_FAMILY);
    if (p_chip_code_obj != NULL) {
        chip_code[0] = (uint8_t)(p_chip_code_obj->vendor >> 8);
        chip_code[1] = (uint8_t)p_chip_code_obj->vendor;
        chip_code[2] = (uint8_t)(p_chip_code_obj->id >> 8);
        chip_code[3] = (uint8_t)p_chip_code_obj->id;
    }
    // return chip_code;
}

int HAL_GetNetifInfo(char *nif_str)
{
    memset(nif_str, 0x0, NIF_STRLEN_MAX);
#ifdef __DEMO__
    /* if the device have only WIFI, then list as follow, note that the len MUST NOT exceed NIF_STRLEN_MAX */
    const char *net_info = "WiFi|03ACDEFF0032";
    strncpy(nif_str, net_info, strlen(net_info));
    /* if the device have ETH, WIFI, GSM connections, then list all of them as follow, note that the len MUST NOT exceed NIF_STRLEN_MAX */
    // const char *multi_net_info = "ETH|0123456789abcde|WiFi|03ACDEFF0032|Cellular|imei_0123456789abcde|iccid_0123456789abcdef01234|imsi_0123456789abcde|msisdn_86123456789ab");
    // strncpy(nif_str, multi_net_info, strlen(multi_net_info));
#else
    uint8_t mac[6];

    memset(mac, 0x0, 6);
    hal_wifi_get_mac_addr(NULL, mac);
    snprintf(nif_str, NIF_STRLEN_MAX, "WiFi|%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#endif
    return strlen(nif_str);
}

//#ifdef REPORT_UUID_ENABLE
/* 重要!!! 请阅读 include/imports/iot_import_product.h中关于这个函数的说明 */
// #define UUID_MAX_LEN (256)
int HAL_GetUUID(uint8_t *buf, int len)
{
    unsigned char uuid[MAC_ADDRESS_SIZE] = {0};

    /* 步骤1.获取唯一标识符(在多个设备都烧了相同三元组情况下能够区分不同设备的标识符,不要求全球唯一), 记为uuid */
    /* TODO:请根据实际情况来实现, mac地址仅仅是一个参考实现.  memcpy(uuid, mac_addr, strlen(mac_addr)) */
    aos_get_mac_hex(uuid);
    sprintf((unsigned char *)buf, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
                uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5]);
    // printf("HAL_GetUUID: %s\n", buf);
    return strlen((char *)buf);
    /* 步骤2.将uuid与当前的时间戳HAL_GetTimeStr拼接的字符串的结果记为uuid_time. */
    // HAL_GetTimeStr(time, sizeof(time));
    // snprintf(uuid_time, UUID_MAX_LEN, "%s%s", uuid, time);


    /* 步骤3.如果能够将uuid_time保存到一片存储介质(比如flash), 并且保证它在恢复出厂设置的时候也不会被erase掉(重要!!!),则
    *      a. 先尝试去读这一片存储介质, 如果读到了则把读的结果拷贝到buf中, 返回读到的字节数, 函数结束返回.
    *      b. 如果读不到(没有烧入过time_uuid的情况), 则将uuid_time写到该存储介质.
    *         i). 如果写成功, 则把uuid_time复制到buf中, 并返回uuid_time的字节数, 函数结束返回;
    *         ii).如果没有写成功, 则返回负数表示失败, 函数结束返回.
    * 如果3的条件不能满足, 则直接到第4步 */

    /* TODO: 参考实现如下:
    *              int size = vendor_impl_read_persistant_flash(tmp);
    *              if(size > 0) {
    *                   size = size > len ? len : size;
    *                   memcpy(buf, tmp, size);
    *                   return size;
    *              } else {
    *                  if (vendor_impl_write_persistant_flash(uuid_time) > 0) {
    *                     TODO:重要!!! 校验写入的结果, 确保与uuid_time一致;如果不一致, 则返回负数表示失败
    *                      memcpy(buf, uuid_time, size);
    *                      return size;
    *                  }
    *              }
    *  其中vendor_impl_read_persistant_flash 和 vendor_impl_write_persistant_flash需要用户实现
    * */

    /* 步骤4. 如果步骤3无法实现(flash在恢复出厂设置时还是会被全部erase掉), 但是能够保证uuid每次读到都是一样(重要!!!), 则
    *        a.如果可以写flash, 则首次读到uuid时将其写入到flash中(务必校验写入的结果与uuid一致). 每次读优先读flash, 并把读到则把结果拷贝到buf中, return读到的字节数, 函数结束返回
    *        b.如果无法写flash, 则直接从器件中读取uuid, 并将uuid拷贝到buf中, 并返回读到的长度(器件可能不稳定, 不推
荐). */

    /* TODO: 参考实现如下:
    *       a.可以参考3中的代码
    *       b.参考实现:
    *       int size = $(uuid)长度
    *              if(size > 0) {
    *                   size = size > len ? len : size;
    *                   memcpy(buf, uuid, size);
    *                   return size;
    *              };
    */

    /*. 步骤5.如果3/4都不能实现, 则返回负数 */
    // return -1;
}
//#endif
