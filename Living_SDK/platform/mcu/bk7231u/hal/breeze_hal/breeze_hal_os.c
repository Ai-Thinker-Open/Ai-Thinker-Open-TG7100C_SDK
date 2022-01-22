/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "breeze_hal_os.h"

int os_timer_new(os_timer_t *timer, os_timer_cb_t cb, void *arg, int ms)
{
    return aos_timer_new_ext(timer, cb, arg, ms, 0, 0);
}

int os_timer_start(os_timer_t *timer)
{
    return aos_timer_start(timer);
}

int os_timer_stop(os_timer_t *timer)
{
    return aos_timer_stop(timer);
}

void os_timer_free(os_timer_t *timer)
{
    aos_timer_free(timer);
}

void os_reboot()
{
    aos_reboot();
}

void os_msleep(int ms)
{
    aos_msleep(ms);
}

long long os_now_ms()
{
    return aos_now_ms();
}

int os_kv_set(const char *key, const void *value, int len, int sync)
{
    int   ret = 0;
    ret = aos_kv_set(key, value, len, sync);
    return ret;
}

int os_kv_get(const char *key, void *buffer, int *buffer_len)
{
    int   ret = 0;
    ret = aos_kv_get(key, buffer, buffer_len);
    return ret;
}

int os_kv_del(const char *key)
{
    int   ret = 0;
    ret = aos_kv_del(key);
    return ret;
}

int os_rand(void)
{
    return (rand() % 0xFFFFFFFF);
}
int os_mutex_new(void *mutex)
{
    return aos_mutex_new((aos_mutex_t *)mutex);
}

void os_mutex_free(void *mutex)
{
    return aos_mutex_free((aos_mutex_t *)mutex);
}

int os_mutex_lock(void *mutex, unsigned int timeout)
{
    return aos_mutex_lock((aos_mutex_t *)mutex, timeout);
}

int os_mutex_unlock(void *mutex)
{
    return aos_mutex_unlock((aos_mutex_t *)mutex);
}