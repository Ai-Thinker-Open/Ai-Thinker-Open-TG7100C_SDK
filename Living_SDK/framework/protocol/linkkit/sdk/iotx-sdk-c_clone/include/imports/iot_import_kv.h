/**
 * @file iot_import_kv.h
 * @brief 
 * @date 2021-04-29
 *
 * @copyright Copyright (C) 2017-2020 Alibaba Group Holding Limited
 *
 * @details
 *
 */

#ifndef __IOT_IMPORTS_KV_H__
#define __IOT_IMPORTS_KV_H__

#if defined(__cplusplus)
extern "C"
{
#endif

    extern int aos_kv_init(void);
    extern int aiot_kv_init(void);
    extern void aos_kv_deinit(void);

    extern int aos_kv_set(const char *key, const void *val, int len, int sync);
    extern int aos_kv_get(const char *key, void *buffer, int *buffer_len);
    extern int aos_kv_del(const char *key);
    extern int aos_kv_del_by_prefix(const char *prefix);

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef __AIOT_KV_API_H__ */
