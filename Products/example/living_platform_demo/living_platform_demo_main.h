/*
 *copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __LIVING_PLATFORM_DEMO_METER_H__
#define __LIVING_PLATFORM_DEMO_METER_H__

#include "aos/aos.h"

#define DEBUG_OUT   1

#ifdef DEBUG_OUT
#define LOG_TRACE(...)                               \
    do {                                                     \
        HAL_Printf("%s.%d: ", __func__, __LINE__);  \
        HAL_Printf(__VA_ARGS__);                                 \
        HAL_Printf("\r\n");                                   \
    } while (0)
#else
#define LOG_TRACE(...)
#endif


typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
    int bind_notified;
    
} user_example_ctx_t;

user_example_ctx_t *user_example_get_ctx(void);
void example_free(void *ptr);

#endif
