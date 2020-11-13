/*
 *Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include <cJSON.h>
#include "ota_service.h"
#include "ota_hal_os.h"
#include "ota_verify.h"
#include "ota_hal_plat.h"
#include "ota_log.h"
#include "iot_export.h"

#if (defined SUPPORT_MCU_OTA)
#include "aos/kv.h"
#include "ota_hal_mcu.h"
#endif

#if (defined BOARD_ESP8266)
#include "k_api.h"
#endif

#if (defined (TG7100CEVB))
#include <hal_boot2.h>
#include "k_api.h"
#include <utils_sha256.h>
#endif

extern ota_hal_module_t ota_hal_module;
static unsigned char ota_is_on_going = 0;

void ota_on_going_reset()
{
    ota_is_on_going = 0;
}

unsigned char ota_get_on_going_status()
{
    return ota_is_on_going;
}

void ota_set_status_on_going()
{
    ota_is_on_going = 1;
}

const char *ota_to_capital(char *value, int len)
{
    if (value == NULL || len <= 0) {
        return NULL;
    }
    int i = 0;
    for (; i < len; i++) {
        if (*(value + i) >= 'a' && *(value + i) <= 'z') {
            *(value + i) -= 'a' - 'A';
        }
    }
    return value;
}

int ota_hex_str2buf(const char* src, char* dest, unsigned int dest_len)
{
    int i, n = 0;
    if(src == NULL || dest == NULL) {
        return -1;
    }
    if(dest_len < strlen(src) / 2) {
        return -1;
    }
    for(i = 0; src[i]; i += 2) {
        if(src[i] >= 'A' && src[i] <= 'F')
            dest[n] = src[i] - 'A' + 10;
        else
            dest[n] = src[i] - '0';
        if(src[i + 1] >= 'A' && src[i + 1] <= 'F')
            dest[n] = (dest[n] << 4) | (src[i + 1] - 'A' + 10);
        else dest[n] = (dest[n] << 4) | (src[i + 1] - '0');
        ++n;
    }
    return n;
}

static int ota_parse(void* pctx, const char *json)
{
    int ret = 0;
    cJSON *root = NULL;
    ota_service_t* ctx = pctx;
    if(!ctx) {
        ret = OTA_PARAM_FAIL;
        goto parse_failed;
    }
    ctx->upg_module_flag = 0;
    char *url = ctx->url;
    char *hash = ctx->hash;
    unsigned char *sign = ctx->sign;
    ota_boot_param_t *ota_param = (ota_boot_param_t *)ctx->boot_param;
    if(!url || !hash || !ota_param) {
        ret = OTA_PARAM_FAIL;
        goto parse_failed;
    }

    root = cJSON_Parse(json);
    if (!root) {
        ret = OTA_PARAM_FAIL;
        goto parse_failed;
    } else {
        if(ctx->trans_protcol != OTA_PROTCOL_COAP_LOCAL) {
            cJSON *message = cJSON_GetObjectItem(root, "message");
            if (NULL == message) {
                ret = OTA_PARSE_FAIL;
                goto parse_failed;
            }
            if ((strncmp(message->valuestring, "success", strlen("success")))) {
                ret = OTA_PARSE_FAIL;
                goto parse_failed;
            }
        }
        cJSON *json_obj = NULL;
        if(ctx->trans_protcol != OTA_PROTCOL_COAP_LOCAL) {
            json_obj = cJSON_GetObjectItem(root, "data");
        }else {
            json_obj = cJSON_GetObjectItem(root, "params");
        }
        if (!json_obj) {
            ret = OTA_PARSE_FAIL;
            goto parse_failed;
        }
        cJSON *resourceUrl = cJSON_GetObjectItem(json_obj, "url");
        if (!resourceUrl) {
            ret = OTA_PARSE_FAIL;
            goto parse_failed;
        }
        cJSON *version = cJSON_GetObjectItem(json_obj, "version");
        if (!version) {
            ret = OTA_PARSE_FAIL;
            goto parse_failed;
        }
        strncpy(ctx->ota_ver, version->valuestring, sizeof(ctx->ota_ver));
        strncpy(url, resourceUrl->valuestring, OTA_URL_LEN - 1);

        /* maybe no module provide.*/
        cJSON *module_name = cJSON_GetObjectItem(json_obj, "module");

        /*clear ota_module info first, overwrite the ota module last time received.*/
        if(module_name){
            ctx->upg_module_flag = 1;
            ctx->module_info = ota_malloc(sizeof(iotx_ota_module_info_t));
            if(ctx->module_info == NULL){
                goto parse_failed;
            }
            memset(ctx->module_info, 0, sizeof(iotx_ota_module_info_t));
            strncpy(ctx->module_info->module_name, module_name->valuestring, OTA_MODULE_NAME_LEN_MAX - 1);
            strncpy(ctx->module_info->module_version, ctx->ota_ver, strlen(ctx->ota_ver));
            /*for now only support master device,not support subdev*/
            strncpy(ctx->module_info->product_key, ctx->pk, strlen(ctx->pk));
            strncpy(ctx->module_info->device_name, ctx->dn, strlen(ctx->dn));
        }else{
            ctx->upg_module_flag = 0;
        }

        cJSON *signMethod = cJSON_GetObjectItem(json_obj, "signMethod");
        if (signMethod) {
            memset(hash, 0x00, OTA_HASH_LEN);
            ota_to_capital(signMethod->valuestring, strlen(signMethod->valuestring));
            if (0 == strncmp(signMethod->valuestring, "MD5", strlen("MD5"))) {
                cJSON *md5 = cJSON_GetObjectItem(json_obj, "sign");
                if (!md5) {
                    ret = OTA_PARSE_FAIL;
                    goto parse_failed;
                }
                ctx->hash_type = MD5;
                strncpy(hash, md5->valuestring, strlen(md5->valuestring)+1);
                hash[strlen(md5->valuestring)] = '\0';
                ota_to_capital(hash, strlen(hash));
            } else if (0 == strncmp(signMethod->valuestring, "SHA256", strlen("SHA256"))) {
                cJSON *sha256 = cJSON_GetObjectItem(json_obj, "sign");
                if (!sha256) {
                    ret = OTA_PARSE_FAIL;
                    goto parse_failed;
                }
                ctx->hash_type = SHA256;
                strncpy(hash, sha256->valuestring, strlen(sha256->valuestring) + 1);
                hash[strlen(sha256->valuestring)] = '\0';
                ota_to_capital(hash, strlen(hash));
            } else {
                ret = OTA_PARSE_FAIL;
                goto parse_failed;
            }
        } else { // old protocol
            memset(hash, 0x00, OTA_HASH_LEN);
            cJSON *md5 = cJSON_GetObjectItem(json_obj, "md5");
            if (!md5) {
                ret = OTA_PARSE_FAIL;
                goto parse_failed;
            }
            ctx->hash_type = MD5;
            strncpy(hash, md5->valuestring, strlen(md5->valuestring) + 1);
            hash[strlen(md5->valuestring)] = '\0';
            ota_to_capital(hash, strlen(hash));
        }
        cJSON *size = cJSON_GetObjectItem(json_obj, "size");
        if (!size) {
            ret = OTA_PARSE_FAIL;
            goto parse_failed;
        }
        ota_param->len = size->valueint;
        ota_param->upg_flag = OTA_RAW;
        cJSON *diff = cJSON_GetObjectItem(json_obj, "isDiff");
        if (diff) {
            int is_diff = diff->valueint;
            if (is_diff) {
                ota_param->upg_flag = OTA_DIFF;
                cJSON *splictSize = cJSON_GetObjectItem(json_obj, "splictSize");
                if (splictSize) {
                    ota_param->splict_size = splictSize->valueint;
                }
            }
        }

#ifdef DEV_OFFLINE_SECURE_OTA_ENABLE
        bool isSecureOfflineOta = true;
#else
        bool isSecureOfflineOta = false;
#endif

        cJSON *digestSign = cJSON_GetObjectItem(json_obj, "digestsign");
        if (digestSign) {
           if((ctx->trans_protcol == OTA_PROTCOL_COAP_LOCAL)&&(isSecureOfflineOta == false)) {
               /*1,only for offline normal ota*/
               ctx->sign_en = OTA_SIGN_OFF;
           } else {
                /*1,online OTA; 2,offline secure ota*/
                memset(sign, 0x00, OTA_SIGN_LEN);
                ctx->sign_en = OTA_SIGN_ON;
                ctx->sign_len = strlen(digestSign->valuestring);
                if(ota_base64_decode((const unsigned char*)digestSign->valuestring, strlen(digestSign->valuestring),sign, &ctx->sign_len) != 0 ) {
                        ret = OTA_PARSE_FAIL;
                        goto parse_failed;
                }
           }
        } else {
            /*1,online OTA; 2,offline normal ota*/
            ctx->sign_en = OTA_SIGN_OFF;

            /*1,only for offline secure ota*/
            if((ctx->trans_protcol == OTA_PROTCOL_COAP_LOCAL)&&(isSecureOfflineOta == true)) {
                ret = OTA_PARSE_FAIL;
                goto parse_failed;
            }
        }
    }
    goto parse_success;
parse_failed:
    OTA_LOG_E("parse failed err:%d", ret);
    if (root) {
        cJSON_Delete(root);
    }

    if(ctx->module_info){
        ota_free(ctx->module_info);
        ctx->module_info = NULL;
    }
    ctx->upg_module_flag = 0;
    return -1;

parse_success:
    if (root) {
        cJSON_Delete(root);
    }
    return ret;
}

#ifdef DEV_OFFLINE_OTA_ENABLE
static void notify_offline_ota_result(ota_service_t* ctx)
{
    int offline_ota_resp_code = DEV_OFFLINE_OTA_RSP_OK;
    if (ctx->upg_status == OTA_DOWNLOAD)
        offline_ota_resp_code = DEV_OFFLINE_OTA_RSP_OK;
    else if (ctx->upg_status == OTA_DOWNLOAD_FAIL)
        offline_ota_resp_code = DEV_OFFLINE_OTA_RSP_DOWNLOAD_FAILED;
    else
        offline_ota_resp_code = DEV_OFFLINE_OTA_RSP_VERIFY_FAILED;

    /*notify 10 times to assure APP to receive successfully */
    uint8_t i = 0;
    for(; i < 10; i++) {
        dev_notify_offline_ota_result(offline_ota_resp_code);
        ota_msleep(300);
    }
}
#endif

int ota_service_set_module_info(void *pctx, iotx_ota_module_info_t *info)
{
    iotx_ota_module_info_t *node;
    iotx_ota_module_info_t *save_info;

    if(info->product_key == NULL || info->device_name == NULL || info->module_name == NULL || info->module_version == NULL || info->hal == NULL ||
    strlen(info->product_key) > PRODUCT_KEY_LEN || strlen(info->device_name) > DEVICE_NAME_LEN || strlen(info->module_name) > MODULE_NAME_LEN || strlen(info->module_version) > MODULE_VERSION_LEN){
        return -1;
    }

    ota_service_t* ctx = pctx;
    if(!ctx) {
        return -1;
    }

    ctx->module_report = info;
    ota_service_inform(ctx);

    //if found in memmory, just update the version.
    slist_for_each_entry(&(ctx->module_list), node, iotx_ota_module_info_t, next) {
        if (memcmp(node->product_key, info->product_key, strlen(info->product_key)) == 0 &&
        memcmp(node->device_name, info->device_name, strlen(info->device_name)) == 0 &&
        memcmp(node->module_name, info->module_name, strlen(info->module_name)) == 0) {
            memset(node->module_version, 0, MODULE_VERSION_LEN + 1);
            memcpy(node->module_version, info->module_version, strlen(info->module_version));
            node->hal = info->hal;
            return 0;
        }
    }

    save_info = ota_malloc(sizeof(iotx_ota_module_info_t));

    if (info == NULL){
        return -1;
    }
    memcpy(save_info,info,sizeof(iotx_ota_module_info_t));

    //if not found, add new info in memory.
    slist_add_tail(&(save_info->next), &(ctx->module_list));
    ctx->module_report = NULL;

    return 0;
}

static void ota_download_thread(void *hand)
{
    int ret = 0;
    ota_service_t* ctx = hand;
    if (!ctx) {
        OTA_LOG_E("ctx is NULL.\n");
        ota_on_going_reset();
        return;
    }
    ota_boot_param_t *ota_param = (ota_boot_param_t *)ctx->boot_param;
    if (!ctx->boot_param) {
        ret = OTA_PARAM_FAIL;
        ctx->upg_status = OTA_DOWNLOAD_FAIL;
        goto ERR;
    }
#ifdef AOS_COMP_PWRMGMT
    pwrmgmt_suspend_lowpower();
#endif
#if ((defined (TG7100CEVB)) || (defined (BOARD_ESP8266)))
    ktask_t* h = NULL;
    #ifdef WIFI_PROVISION_ENABLED
       extern int awss_suc_notify_stop(void);
       awss_suc_notify_stop();
    #endif
    #ifdef DEV_BIND_ENABLED
    extern int awss_dev_bind_notify_stop(void);
    awss_dev_bind_notify_stop();
    #endif

#if (defined (TG7100CEVB))
    hal_update_mfg_ptable();
#endif

    h = krhino_task_find("linkkit");
    if(h)
    krhino_task_dyn_del(h);
    ota_msleep(500);
    h = krhino_task_find("netmgr_start");
    if(h)
    krhino_task_dyn_del(h);
    ota_msleep(500);
    h = krhino_task_find("reset");
    ota_msleep(500);
    OTA_LOG_I("awss_notify stop.");
    h = krhino_task_find("event_task");
    if(h)
    krhino_task_dyn_del(h);
    ota_msleep(500);
#endif
    ota_param->off_bp = 0;
    ota_param->res_type = OTA_FINISH;

    #if defined (SUPPORT_MCU_OTA)
    OTA_LOG_I("upg_mcu_flag:%d", ctx->upg_mcu_flag);
    if (ctx->upg_mcu_flag == 1) {
        ret = ota_mcu_init((void *)(ota_param));
        if(ret < 0) {
            ctx->upg_status = OTA_DOWNLOAD_FAIL;
            goto ERR;
        }
    } else {
        ret = ota_hal_init((void *)(ota_param));
        if(ret < 0) {
            ctx->upg_status = OTA_DOWNLOAD_FAIL;
            goto ERR;
        }
    }
    #else
    ret = ota_hal_init((void *)(ota_param));
    if(ret < 0) {
        ctx->upg_status = OTA_DOWNLOAD_FAIL;
        goto ERR;
    }
    #endif
    ret = ota_malloc_hash_ctx(ctx->hash_type);
    if (ret < 0) {
        ret = OTA_PARAM_FAIL;
        ctx->upg_status = OTA_DOWNLOAD_FAIL;
        goto ERR;
    }
    ctx->upg_status = OTA_DOWNLOAD;
    if(ctx->trans_protcol != OTA_PROTCOL_COAP_LOCAL) {
        ctx->h_tr->status(0, ctx);
    }
    ret = ctx->h_dl->start((void*)ctx);

    if (ret < 0) {
        ota_param->res_type = OTA_BREAKPOINT;
        #if defined (SUPPORT_MCU_OTA)
        if (ctx->upg_mcu_flag == 1) {
            ret = ota_mcu_boot((void*)(ota_param));
        } else {
            ret = ota_hal_boot((void*)(ota_param));
        }
        #else
        ret = ota_hal_boot((void*)(ota_param));
        #endif
        ctx->upg_status = OTA_DOWNLOAD_FAIL;
        goto ERR;
    }
    if (ret == OTA_CANCEL) {
        ota_param->res_type = OTA_BREAKPOINT;
        #if defined (SUPPORT_MCU_OTA)
        if (ctx->upg_mcu_flag == 1) {
            ret = ota_mcu_boot((void*)(ota_param));
        } else {
            ret = ota_hal_boot((void*)(ota_param));
        }
        #else
        ret = ota_hal_boot((void*)(ota_param));
        #endif
        ctx->upg_status = OTA_CANCEL;
        goto ERR;
    }
    ret = ota_check_hash((OTA_HASH_E)ctx->hash_type, ctx->hash);
    if (ret < 0) {
        ctx->upg_status = OTA_VERIFY_HASH_FAIL;
        goto ERR;
    }
#if !defined(BOARD_ESP32)
    if( ctx->sign_en == OTA_SIGN_ON) {
        ret = ota_verify_download_rsa_sign((unsigned char*)ctx->sign, (const char*)ctx->hash, (OTA_HASH_E)ctx->hash_type);
        if(ret < 0) {
            ctx->upg_status = OTA_VERIFY_RSA_FAIL;
            goto ERR;
        }
    }
#endif
    #if defined (SUPPORT_MCU_OTA)
    if (ctx->upg_mcu_flag == 1) {
        OTA_LOG_I("MCU upgrade no check image");

        /* need update save mcu version when upgrade ok */
        char mcu_ver[16] = {0};
        int mcu_ver_len = 0;
        char *pos1 = strstr(ctx->ota_ver, "mcu-");
        char *pos2 = strstr(ctx->ota_ver, "app-");
        mcu_ver_len = pos2 - pos1 - 1;
        strncpy(mcu_ver, ctx->ota_ver, mcu_ver_len);
        aos_kv_set("mcu_version", (void *)mcu_ver, sizeof(mcu_ver), 1);
        OTA_LOG_I("update save MCU ver:%s", mcu_ver);
    } else {
        if(ota_param->upg_flag != OTA_DIFF) {
            ret = ota_check_image(ota_param->len);
            if (ret < 0) {
                ctx->upg_status = OTA_VERIFY_HASH_FAIL;
                goto ERR;
            }
        }
    }
    #else
    if(ota_param->upg_flag != OTA_DIFF){
        ret = ota_check_image(ota_param->len);
        if (ret < 0) {
            ctx->upg_status = OTA_VERIFY_HASH_FAIL;
            goto ERR;
        }
    }
    #endif

    ota_param->res_type = OTA_FINISH;

#ifdef DEV_OFFLINE_OTA_ENABLE
    if(ctx->trans_protcol == OTA_PROTCOL_COAP_LOCAL) {
        notify_offline_ota_result(ctx);
    }
#endif
#if (defined (TG7100CEVB))
    {
        uint8_t activeID;
        HALPartition_Entry_Config ptEntry;
#define CHECK_IMG_BUF_SIZE   512
        uint8_t sha_check[32] = {0};
        uint8_t dst_sha[32] = {0};
        uint32_t bin_size, read_size;
        iot_sha256_context sha256_ctx;
        int offset = 0;
        char *r_buf;
        
        activeID = hal_boot2_get_active_partition();

        printf("[OTA] [TEST] activeID is %u\r\n", activeID);

        if (hal_boot2_get_active_entries(BOOT2_PARTITION_TYPE_FW, &ptEntry)) {
            printf("hal_boot2_get_active_entries error.\r\n");
            while (1);
            return;
        }
        printf("[OTA] [TCP] prepare OTA partition info\r\n");
        ptEntry.len = ota_param->len - 32;

        r_buf = ota_malloc(CHECK_IMG_BUF_SIZE);
        if (r_buf == NULL) {
            ret = OTA_VERIFY_HASH_FAIL;
            goto ERR;
        }

        bin_size = ota_param->len - 32;
        utils_sha256_init(&sha256_ctx);
        utils_sha256_starts(&sha256_ctx);

        offset = 0;
        while (offset < bin_size) {
            (bin_size - offset >= CHECK_IMG_BUF_SIZE) ? (read_size = CHECK_IMG_BUF_SIZE):(read_size = bin_size - offset);
            if (ota_hal_read(&offset, r_buf, read_size) < 0) {
                ret = OTA_UPGRADE_FAIL;
                ota_free(r_buf);
                goto ERR;
            }
            utils_sha256_update(&sha256_ctx, (const uint8_t *)r_buf, read_size);
        }

        utils_sha256_finish(&sha256_ctx, sha_check);
        utils_sha256_free(&sha256_ctx);
        ota_free(r_buf);

        ota_hal_read(&offset, (char *)dst_sha, 32);

        if (memcmp(sha_check, (const void *)dst_sha, 32) != 0) {
            ctx->upg_status = OTA_VERIFY_HASH_FAIL;
            ret = OTA_VERIFY_HASH_FAIL;
            goto ERR;
        }

        printf("[OTA] [TCP] Update PARTITION, partition len is %lu\r\n", ptEntry.len);
        hal_boot2_update_ptable(&ptEntry);
        ota_reboot();
    }
#endif

    #if defined (SUPPORT_MCU_OTA)
    if (ctx->upg_mcu_flag == 1) {
        ret = ota_mcu_boot((void*)(ota_param));
    } else {
        ret = ota_hal_boot((void*)(ota_param));
    }
    #else
    ret = ota_hal_boot((void*)(ota_param));
    #endif
    ctx->upg_status = OTA_REBOOT;

ERR:
    OTA_LOG_E("upgrade over err:%d", ret);
#if defined (RDA5981x) || defined (RDA5981A)
    OTA_LOG_I("clear ota part. \n");
    #if defined (SUPPORT_MCU_OTA)
    if (ctx->upg_mcu_flag == 1) {
        ota_mcu_init((void *)(ota_param));
    } else {
        ota_hal_init((void *)(ota_param));
    }
    #else
    ota_hal_init((void *)(ota_param));
    #endif
#endif
    if(ctx->trans_protcol != OTA_PROTCOL_COAP_LOCAL) {
#if (!defined BOARD_ESP8266)
        ctx->h_tr->status(100, ctx);
#endif
    }
#ifdef DEV_OFFLINE_OTA_ENABLE
    else {
        notify_offline_ota_result(ctx);
    }
#endif
    ota_free_hash_ctx();
#ifdef AOS_COMP_PWRMGMT
    pwrmgmt_resume_lowpower();
#endif
    ota_on_going_reset();
    ota_msleep(3000);
    #if defined (SUPPORT_MCU_OTA)
    if ((ctx->upg_mcu_flag == 1) && (ota_param->res_type == OTA_BREAKPOINT)) {
        OTA_LOG_I("ota breakpoint no reboot\n");
    } else {
        ota_reboot();
    }
    #else
#if ((defined (TG7100CEVB)) || (defined (BOARD_ESP8266)))
        if(ctx->module_info){
        ota_free(ctx->module_info);
        ctx->module_info = NULL;
    }
    ctx->upg_module_flag = 0;
    ota_reboot();
#else
    if(ctx->module_info){
        ota_free(ctx->module_info);
        ctx->module_info = NULL;
    }
    ctx->upg_module_flag = 0;
    if (ctx->upg_status == OTA_REBOOT) //only reboot status then do reboot
    {
        ota_reboot();
    }
#endif
    #endif
}

int ota_upgrade_cb(void* pctx, char *json)
{
    int ret = -1;
    int ret_offline_ota = DEV_OFFLINE_OTA_RSP_OK;
    int is_ota = 0;
    void *thread = NULL;
    ota_service_t *ctx = pctx;
    iotx_ota_module_info_t *node = NULL;

    if (!ctx || !json) {
        return ret;
    }
    if (0 == ota_parse(ctx, json)) {
        ret = 0;
        #if defined (SUPPORT_MCU_OTA)
        int is_upg_mcu = 0;
        int is_upg_app = 0;
        int mcu_ver_len = 0;
        char *pos1 = strstr(ctx->ota_ver, "mcu-");
        char *pos2 = strstr(ctx->ota_ver, "app-");
        mcu_ver_len = pos2 - pos1 - 1;
        if ((pos1 != NULL) && (pos2 != NULL) && (mcu_ver_len > 0)) {
            is_upg_mcu = strncmp(ctx->ota_ver, ctx->sys_ver, mcu_ver_len);
            OTA_LOG_I("is_upg_mcu:%d", is_upg_mcu);
            is_upg_app = strncmp(ctx->ota_ver + (mcu_ver_len + 1), ctx->sys_ver + (mcu_ver_len + 1), strlen(ctx->ota_ver) - (mcu_ver_len + 1));
            OTA_LOG_I("is_upg_app:%d", is_upg_app);
            if ((is_upg_mcu > 0) && (is_upg_app == 0)) {
                is_ota = 1;
                ctx->upg_mcu_flag = 1;
                OTA_LOG_I("MCU OTA");
            } else if ((is_upg_app > 0) && (is_upg_mcu == 0)) {
                is_ota = 1;
                ctx->upg_mcu_flag = 0;
                OTA_LOG_I("APP OTA");
            } else if ((is_upg_mcu > 0) && (is_upg_app > 0)) {
                OTA_LOG_E("no support MCU and APP ota at the same time, discard it.");
                is_ota = 0;
            } else {
                is_ota = 0;
            }
        }
        #else

        is_ota = 0;
        if(ctx->upg_module_flag == 1){
            slist_for_each_entry(&(ctx->module_list), node, iotx_ota_module_info_t, next) {

                //for now, there is no pk\dn provide ,so just the compare module_name instead.
                if (
                //     strcmp(node->product_key, product_key) == 0 &&
                // strcmp(node->device_name, device_name) == 0 &&
                    strcmp(node->module_name, ctx->module_info->module_name) == 0 &&
                    ctx->module_info != NULL) {
                    is_ota = strcmp(ctx->module_info->module_version,node->module_version);
                    if(is_ota > 0){
                        #ifdef OTA_MULTI_MODULE_DEBUG
                        HAL_Kv_Set("ota_m_name_cloud",ctx->module_info->module_name,strlen(ctx->module_info->module_name),1);
                        HAL_Kv_Set("ota_m_version_cloud",ctx->module_info->module_version,strlen(ctx->module_info->module_version),1);
                        OTA_LOG_I("write debug cloud ota info to KV.");
                        #endif
                        ota_hal_register_module(node->hal);
                    }
                    OTA_LOG_E("ota version %s, old version is %s!",ctx->module_info->module_version, node->module_version);
                    break;
                }
            }

            if(node == NULL || ctx->module_info == NULL){
                is_ota = 0;
            }
        }else{
            is_ota = strncmp(ctx->ota_ver,ctx->sys_ver,strlen(ctx->ota_ver));
        }

        #endif
        if(is_ota > 0) {
            if(ota_get_on_going_status() == 1) {
                OTA_LOG_E("ota is on going, go out!!!");
                if(ctx->trans_protcol != OTA_PROTCOL_COAP_LOCAL) {
                    return ret;
                } else {
                    return ret_offline_ota;
                }
            }
            ota_set_status_on_going();
            ret = ota_thread_create(&thread, (void *)ota_download_thread, (void *)ctx, NULL, 4096);
            if(ret < 0) {
                if(ctx->module_info){
                    ota_free(ctx->module_info);
                    ctx->module_info = NULL;
                }
                ctx->upg_module_flag = 0;
                ota_on_going_reset();
                OTA_LOG_E("ota create task failed!");
#if ((defined (TG7100CEVB)) || (defined (BOARD_ESP8266)))  /* workaround for ota pressure test */
                ota_msleep(200);
                ota_reboot();
                while (1);
#endif
            }
        } else {
            OTA_LOG_E("ota version is too old, discard it.");
            ret_offline_ota = DEV_OFFLINE_OTA_RSP_SAME_VERSION;
            ctx->upg_status = OTA_INIT_VER_FAIL;
            if(ctx->trans_protcol != OTA_PROTCOL_COAP_LOCAL) {
                ctx->h_tr->status(0, ctx);
            }
            if(ctx->module_info){
                ota_free(ctx->module_info);
                ctx->module_info = NULL;
            }
            ctx->upg_module_flag = 0;
        }
    }
    else
    {
        ret_offline_ota = DEV_OFFLINE_OTA_RSP_INVALID_PARAM;
    }

    if(ctx->trans_protcol != OTA_PROTCOL_COAP_LOCAL) {
        return ret;
    } else {
        return ret_offline_ota;
    }
}

#ifdef DEV_OFFLINE_OTA_ENABLE
static int offline_ota_upgrade_cb(void* pctx, char *json)
{
    ota_service_t* ctx =(ota_service_t*)pctx;
    ctx->trans_protcol = OTA_PROTCOL_COAP_LOCAL;
    //format req's json
    //start dl ota_thread_create
    return ota_upgrade_cb(pctx,json);
    //send resp
}
#endif

int ota_service_init(ota_service_t *ctx)
{
    int ret = 0;
    if (!ctx) {
        ctx = ota_malloc(sizeof(ota_service_t));
        memset(ctx, 0, sizeof(ota_service_t));
    }
    if(!ctx) {
        ret = OTA_INIT_FAIL;
        return ret;
    }
    ota_hal_register_module(&ota_hal_module);
    ctx->upgrade_cb = ota_upgrade_cb;
    ctx->boot_param = ota_malloc(sizeof(ota_boot_param_t));
    if(!ctx->boot_param) {
        ret = OTA_INIT_FAIL;
        return ret;
    }
    memset(ctx->boot_param, 0, sizeof(ota_boot_param_t));
    if(ctx->inited) {
        ret = OTA_INIT_FAIL;
        return ret;
    }
    ctx->inited = 1;
    ota_on_going_reset();
    ctx->url = ota_malloc(OTA_URL_LEN);
    if(NULL == ctx->url){
        ret = OTA_INIT_FAIL;
        return ret;
    }
    memset(ctx->url, 0, OTA_URL_LEN);
    ctx->hash = ota_malloc(OTA_HASH_LEN);
    if(NULL == ctx->hash){
        ret = OTA_INIT_FAIL;
        return ret;
    }
    memset(ctx->hash, 0, OTA_HASH_LEN);
    ctx->sign = ota_malloc(OTA_SIGN_LEN);
    if(NULL == ctx->sign){
        ret = OTA_INIT_FAIL;
        return ret;
    }
    strncpy(ctx->sys_ver, ota_hal_get_version(ctx->dev_type), sizeof(ctx->sys_ver) -1);
    memset(ctx->sign, 0, OTA_SIGN_LEN);
    ctx->h_tr = ota_get_transport();
    ctx->h_dl = ota_get_download();

    slist_init(&(ctx->module_list));

#ifdef DEV_OFFLINE_OTA_ENABLE
    dev_offline_ota_module_init(ctx, offline_ota_upgrade_cb);
#else
    ota_service_inform(ctx);
#endif
    OTA_LOG_I("ota init success, ver:%s type:%d", ctx->sys_ver, ctx->dev_type);
    ota_hal_rollback(NULL);
    return ret;
}

int ota_service_inform(ota_service_t *ctx)
{
    int ret = 0;
    ctx->h_tr->init();
    ret = ctx->h_tr->inform(ctx);
    if(ret < 0){
        return ret;
    }
    ret = ctx->h_tr->upgrade(ctx);
    return ret;
}

int ota_service_deinit(ota_service_t *ctx)
{
    if(!ctx) {
        return -1;
    }
    if(ctx->h_ch) {
        ota_free(ctx->h_ch);
        ctx->h_ch = NULL;
    }
    ctx->h_tr = NULL;
    ctx->h_dl = NULL;
    ctx->inited = 0;
    if(ctx->url){
        ota_free(ctx->url);
        ctx->url = NULL;
    }
    if(ctx->hash){
        ota_free(ctx->hash);
        ctx->hash = NULL;
    }
    if(ctx->sign){
        ota_free(ctx->sign);
        ctx->sign = NULL;
    }
    if(ctx->boot_param){
        ota_free(ctx->boot_param);
        ctx->boot_param = NULL;
    }
    if(ctx){
        ota_free(ctx);
        ctx = NULL;
    }
    return 0;
}
