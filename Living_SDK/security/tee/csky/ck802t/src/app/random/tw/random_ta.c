/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_srv.h"
#include "tee_types.h"
#include "tee_dbg.h"

#include "tee_tos.h"
#include "tee_crypto.h"
#include "tee_random_cmd.h"
#include "tee_random.h"

static TEE_Result _RANDOM_Test_CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

static void _RANDOM_Test_DestroyEntryPoint(void)
{
    return;
}

static TEE_Result _RANDOM_Test_OpenSessionEntryPoint(
        uint32_t paramTypes,
        TEE_Param params[4],
        void **sessionContext)
{
    return TEE_SUCCESS;
}

static void _RANDOM_Test_CloseSessionEntryPoint(void *sessionContext)
{
    return;
}

static TEE_Result _RANDOM_Test_InvokeCommandEntryPoint(
        void *sessionContext,
        uint32_t commandID,
        uint32_t paramTypes,
        TEE_Param params[4])
{
    TEE_Result ret = TEE_SUCCESS;
    uint8_t *data;
    uint32_t len;

    if (commandID == TEE_RANDOM_SEED_SET) {
        if (paramTypes != TEE_PARAM_TYPES(
                TEE_PARAM_TYPE_VALUE_INPUT,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE)) {
            return TEE_ERROR_BAD_PARAMETERS;
        }

        ret = TEE_SUCCESS;
    } else if (commandID == TEE_RANDOM_DATA_GET){
        if (paramTypes != TEE_PARAM_TYPES(
                TEE_PARAM_TYPE_MEMREF_OUTPUT,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE)) {
            return TEE_ERROR_BAD_PARAMETERS;
        }

        data = params[0].memref.buffer;
        len  = params[0].memref.size;
        if (tee_rand_gen(data, len) != TEE_CRYPTO_SUCCESS) {
            ret = TEE_CRYPTO_ERROR;
        } else {
            ret = TEE_SUCCESS;
        }
        {
            int i;
            if (ret == TEE_SUCCESS) {
                for (i = 0; i < len; i++) {
                    tee_dbg_print(INF, "%x ", data[i]);
                }
            }
        }

    } else {
        ret = TEE_ERROR_BAD_PARAMETERS;
    }

    return ret;
}

TEE_SRV_DATA_START(_RANDOM_Test_CreateEntryPoint,
        _RANDOM_Test_DestroyEntryPoint,
        _RANDOM_Test_OpenSessionEntryPoint,
        _RANDOM_Test_CloseSessionEntryPoint,
        _RANDOM_Test_InvokeCommandEntryPoint)
TEE_SRV_UUID_PROPERTY("gpd.ta.appID", RANDOM_SRV_UUID)
TEE_SRV_BOOL_PROPERTY("gpd.ta.singleInstance", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.multiSession", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.instanceKeepAlive", true)
TEE_SRV_INT_PROPERTY("gpd.ta.dataSize", 0x10000)
TEE_SRV_INT_PROPERTY("gpd.ta.stackSize", 0x1000)
TEE_SRV_DATA_END
