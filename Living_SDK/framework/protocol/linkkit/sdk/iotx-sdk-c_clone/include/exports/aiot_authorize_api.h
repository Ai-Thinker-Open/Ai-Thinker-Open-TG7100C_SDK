/**
 * @file aiot_bind_api.h
 * @brief bind module api header file
 * @version 0.1
 * @date 2019-05-28
 *
 * @copyright Copyright (c) 2015-2018 Alibaba Group Holding Limited
 *
 */

#ifndef _AIOT_AUTHORIZE_API_H_
#define _AIOT_AUTHORIZE_API_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "iot_import.h"

// #define STATE_SUCCESS 0
#define STATE_FAILED -1
#define STATE_USER_INPUT_UNKNOWN_OPTION (-0x0103)
// #define STATE_USER_INPUT_OUT_RANGE      (-0x0102)
#define STATE_SYS_DEPEND_MALLOC_FAILED  (-0x0201)


/**
 * @brief authorize option, all mandatory option shall be setup
 *
 */
typedef enum {
    AUTHORIZEOPT_MQTT_HANDLE,       /**< dataType:(void *), mqtt handle, @b mandatory option */
    AUTHORIZEOPT_AUTHORIZE_MAC,     /**< dataType:(char *), authorize mac/sn..., @b mandatory option */
    AUTHORIZEOPT_AUTHORIZE_CODE,    /**< dataType:(char *), authorize code, @b mandatory option */
    AUTHORIZEOPT_AUTHORIZE_REQUEST, /**< dataType:(char *), authorize request, @b mandatory option */
    AUTHORIZEOPT_AUTHORIZE_PARSE,   /**< dataType:(char *), authorize reply data parse, @b mandatory option */
    AUTHORIZEOPT_AUTHORIZE_CHECK,   /**< dataType:(char *), authorize check, @b mandatory option */
    AUTHORIZEOPT_AUTHORIZE_CHECK_CB,   /**< dataType:(char *), pointer check result callback, @b mandatory option */
    AUTHORIZEOPT_AUTHORIZE_RW_CB,   /**< dataType:(authorize_cb_t), pointer data read/write function */
    AUTHORIZEOPT_MAX
} aiot_authorize_option_t;

typedef int32_t(*authorize_check_cb_t)(char *random, char* digest);
typedef int32_t(*authorize_rw_cb_t)(char *data, int32_t rw);

typedef struct {
    void *mqtt_handle;
    void *mutex;
    authorize_check_cb_t check_result_cb;
    authorize_rw_cb_t auth_rw_cb;
    char *mac;
    char *authcode;
    uint8_t authcode_count;
    uint8_t req_count;
    uint8_t req_success;
} aiot_authorize_handle_t;


/**
 * @brief initializes the authorize module
 *
 * @return void*
 * @retval Not NULL handle of authorize module
 * @retval NULL initializes failed, system callbacks not complete or malloc failed.
 *
 * @brief
 * @brief --------------------------------------------------
 *
 * @brief 初始化授权模块
 *
 * @return void*
 * @retval Not NULL 授权模块句柄
 * @retval NULL 初始化失败, 系统回调不完整或者内存分配失败.
 *
 */
void *aiot_authorize_open(void *mqtt_handle);

/**
 * @brief set option of authorize moduel
 *
 * @param[in] handle handle of authorize module
 * @param[in] option the configuration option, see @ref aiot_authorize_option_t
 * @param[in] data   the configuration data, see @ref aiot_authorize_option_t
 *
 * @return int32_t
 * @retval ERRCODE_SUCCESS set option successfully
 * @retval <ERRCODE_SUCCESS set option failed
 *
 * @brief
 * @brief --------------------------------------------------
 *
 * @brief 配置绑定模块选项
 *
 * @param[in] handle 绑定模块句柄
 * @param[in] option 配置选项, 查看 @ref aiot_authorize_option_t
 * @param[in] data   配置数据, 查看 @ref aiot_authorize_option_t
 *
 * @return int32_t
 * @retval ERRCODE_SUCCESS 配置成功
 * @retval <ERRCODE_SUCCESS 配置失败
 */
int32_t aiot_authorize_setopt(void *handle, aiot_authorize_option_t option, void *data);

/**
 * @brief release resource of authorize module
 *
 * @param[in] handle the pointer to the authorize handle
 *
 * @return int32_t
 * @retval ERRCODE_SUCCESS execute success
 * @retval ERRCODE_INVALID_PARAMETER handle is NULL or the content of handle is NULL
 *
 * @brief
 * @brief --------------------------------------------------
 *
 * @brief 释放authorize句柄的资源
 *
 * @param[in] handle 指向授权句柄的指针
 *
 * @return int32_t
 * @retval ERRCODE_SUCCESS 执行成功
 * @retval ERRCODE_INVALID_PARAMETER handle为NULL或者handle的值为NULL
 *
 */
int32_t aiot_authorize_deinit(void **handle);

#if defined(__cplusplus)
}
#endif
#endif /* #ifndef _AIOT_AUTHORIZE_API_H_ */