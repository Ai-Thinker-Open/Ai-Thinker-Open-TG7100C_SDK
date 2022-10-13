/**
 * @file genie_ota.c
 * @brief genie ota by gatt
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_OTA)
// #include "genie_crypto.h"
#include "genie_ais.h"
#include "genie_ota.h"
#include "genie_sal_ota.h"

genie_ota_ctx_t genie_ota_ctx;
extern void do_encrypt(uint8_t *data, uint16_t len);

bool genie_ota_is_ready(void)
{
    return genie_ota_ctx.ota_ready;
}

bool genie_ota_get_indication(void)
{
    // uint8_t ota_indicat = 0xFF;
    // genie_storage_status_e ret;

    // ret = genie_storage_read_userdata(GFI_OTA_INDICAT, &ota_indicat, sizeof(ota_indicat));
    // if (ret == GENIE_STORAGE_SUCCESS && ota_indicat == 1)
    // {
    //     return 1;
    // }

    return 0;
}

#ifdef CONFIG_GENIE_OTA_PINGPONG
void ota_check_image_id(void)
{
    uint32_t saved_ota_image_id = 0;
    uint8_t ota_image_id = DFU_IMAGE_INVALID_ID;
    genie_storage_status_e ret;

    ret = genie_storage_read_reliable(GFI_OTA_IMAGE_ID, (uint8_t *)&saved_ota_image_id, sizeof(saved_ota_image_id));
    if (ret == GENIE_STORAGE_SUCCESS)
    {
        ota_image_id = saved_ota_image_id & 0xFF;
        if ((saved_ota_image_id >> 8) == GENIE_OTA_IMAGE_ID_MAGIC && ota_image_id < DFU_IMAGE_INVALID_ID)
        {
            if (ota_image_id != genie_sal_ota_get_current_image_id())
            {
                BREEZE_INFO("switch to %d", ota_image_id);
                if (genie_sal_ota_change_image_id(ota_image_id) == ota_image_id)
                {
                    // genie_storage_delete_reliable(GFI_OTA_IMAGE_ID);
                    hal_reboot();
                }
            }
        }
        BREEZE_DEBUG("del change %d", ota_image_id);
        // genie_storage_delete_reliable(GFI_OTA_IMAGE_ID);
    }
}
#endif

static void _ais_set_ota_change(void)
{
    // uint8_t ota_indicat = 1;
    genie_sal_ota_ctx_t sal_ota_ctx;

#ifdef CONFIG_GENIE_OTA_PINGPONG
    uint32_t ota_image_id = GENIE_OTA_IMAGE_ID_A;
#endif

    // genie_storage_write_userdata(GFI_OTA_INDICAT, &ota_indicat, sizeof(ota_indicat));

#ifdef CONFIG_GENIE_OTA_PINGPONG
    if (genie_sal_ota_get_current_image_id() == DFU_IMAGE_A)
    {
        ota_image_id = GENIE_OTA_IMAGE_ID_B;
    }
    else
    {
        ota_image_id = GENIE_OTA_IMAGE_ID_A;
    }

    BREEZE_DEBUG("switch to %d", ota_image_id & 0xFF);
    genie_storage_write_reliable(GFI_OTA_IMAGE_ID, (uint8_t *)&ota_image_id, sizeof(ota_image_id));
#endif

    genie_ota_ctx.ota_ready = 1;

    memset(&sal_ota_ctx, 0, sizeof(genie_sal_ota_ctx_t));
    sal_ota_ctx.ota_ready = genie_ota_ctx.ota_ready;
    sal_ota_ctx.image_type = genie_ota_ctx.image_type;
    sal_ota_ctx.image_size = genie_ota_ctx.image_size;
    sal_ota_ctx.image_crc16 = genie_ota_ctx.image_crc16;
    genie_sal_ota_update_boot_params(sal_ota_ctx);
}

void genie_ota_status_report(void)
{
    uint8_t plaine_data[GENIE_CRYPTO_UNIT_SIZE];
    uint8_t encrypt_data[GENIE_CRYPTO_UNIT_SIZE];

    ais_ota_status_report_t *p_status_report = (ais_ota_status_report_t *)plaine_data;

    memset(plaine_data, 11, sizeof(plaine_data));
    p_status_report->last_seq = genie_ota_ctx.last_seq;
    p_status_report->total_frame = genie_ota_ctx.total_frame;
    p_status_report->rx_size = genie_ota_ctx.rx_size;

    printf("ota last[%d] total[%d] size[%ld] err[%d]\r\n", p_status_report->last_seq, p_status_report->total_frame, p_status_report->rx_size, genie_ota_ctx.err_count);

    // genie_crypto_encrypt(plaine_data, encrypt_data);
    do_encrypt(plaine_data, 5);
    genie_ais_notify(0, AIS_OTA_STATUS, plaine_data, 16, 1);
}
int32_t genie_version_appver_get(uint8_t* ver)
{
    memset(ver, 0, sizeof(ver));
    printf("ver len=%d\r\n", strlen(aos_get_app_version()));
    memcpy(ver, aos_get_app_version(), strlen(aos_get_app_version()));
    return 0;
}
bool genie_ota_handle_version_request(uint8_t msg_id, ais_ota_ver_req_t *p_ver_req, uint8_t encrypt)
{
    uint8_t plaine_data[GENIE_CRYPTO_UNIT_SIZE];
    uint8_t encrypt_data[GENIE_CRYPTO_UNIT_SIZE];

    ais_ota_ver_resp_t *p_ver_resp = (ais_ota_ver_resp_t *)plaine_data;

    if (p_ver_req->image_type == 0)
    {
        memset(plaine_data, 0, sizeof(plaine_data));
        p_ver_resp->image_type = 0;
        memset(p_ver_resp->ver,0, sizeof(p_ver_resp->ver));
        genie_version_appver_get(p_ver_resp->ver);
        uint8_t len = strlen(p_ver_resp->ver);
        printf("--ver:(%s) len=%d\r\n", p_ver_resp->ver, len);
        if (encrypt > 0)
        {
            // genie_crypto_encrypt(plaine_data, encrypt_data);
            do_encrypt(plaine_data, len+1);
            genie_ais_notify(msg_id, AIS_OTA_LONG_VER_RESP, plaine_data, GENIE_CRYPTO_UNIT_SIZE, 1);
        }
        else
        {
            genie_ais_notify(msg_id, AIS_OTA_LONG_VER_RESP, plaine_data, strlen(p_ver_resp->ver) + 1, 0); //5=sizeof(ais_ota_ver_resp_t)
        }

        return true;
    }

    return false;
}

bool genie_ota_handle_update_request(uint8_t msg_id, ais_ota_upd_req_t *p_ota_req, uint8_t encrypt)
{
    uint8_t plaine_data[GENIE_CRYPTO_UNIT_SIZE];
    // uint8_t encrypt_data[GENIE_CRYPTO_UNIT_SIZE];
    uint8_t ver[GENIE_CRYPTO_UNIT_SIZE - 1] = {0};

    ais_ota_upd_resp_t *p_upd_resp = (ais_ota_upd_resp_t *)plaine_data;

    printf("\r\nota ver:%s imgT:%d imgS:%ld maxS:%ld\r\n", p_ota_req->ver, p_ota_req->image_type, p_ota_req->fw_size, genie_sal_ota_get_max_partition_size());
    genie_version_appver_get(ver);
    memset(plaine_data, 10, sizeof(plaine_data));
    if (p_ota_req->image_type != 0 || strcmp(p_ota_req->ver, ver) <= 0 ||
        genie_sal_ota_get_max_partition_size() < p_ota_req->fw_size || 0 == p_ota_req->fw_size) //Is illeagal
    {
        p_upd_resp->state = 0;
        p_upd_resp->rx_size = 0;
    }
    else
    {
        genie_ais_state_set(AIS_STATE_OTA);
        memset(&genie_ota_ctx, 0, sizeof(genie_ota_ctx));
        genie_ota_ctx.image_type = p_ota_req->image_type;
        memcpy(genie_ota_ctx.image_ver, p_ota_req->ver, strlen(p_ota_req->ver));
        genie_ota_ctx.image_size = p_ota_req->fw_size;
        genie_ota_ctx.image_crc16 = p_ota_req->crc16;
        genie_ota_ctx.ota_flag = p_ota_req->ota_flag;
        p_upd_resp->state = 1;

        /*If support breakpoint continue,you should set rx_size*/
        p_upd_resp->rx_size = 0;
    }

    p_upd_resp->total_frame = CONFIG_AIS_TOTAL_FRAME - 1;

    if (encrypt > 0)
    {
        // genie_crypto_encrypt(plaine_data, encrypt_data);

        extern void do_encrypt(uint8_t *data, uint16_t len);
        do_encrypt(plaine_data, 6);
        genie_ais_notify(msg_id, AIS_OTA_UPD_RESP, plaine_data, 16, 1);
    }
    else
    {
        genie_ais_notify(msg_id, AIS_OTA_UPD_RESP, plaine_data, 6, 0); //response use plain data
    }

#if (defined (TG7100CEVB))
    hal_update_mfg_ptable();
    genie_sal_ota_erase();
#endif

    return true;
}

bool genie_ota_parse_pdu(ais_pdu_t *p_msg)
{
    uint8_t *p_payload = p_msg->payload;
    uint16_t payload_len = p_msg->header.payload_len;
    BT_DBG("seq=%d  tt=%d\r\n", p_msg->header.seq, p_msg->header.total_frame);
    if (p_msg->header.seq > p_msg->header.total_frame)
    {
        BT_ERR("invalid");
        return false;
    }

    if (p_msg->header.seq != genie_ota_ctx.except_seq)
    {
        printf("except_seq:%d rx_seq:%d len:%d\r\n", genie_ota_ctx.except_seq, p_msg->header.seq, p_msg->header.payload_len);
        if (genie_ota_ctx.err_count++ == 0)
        {
            /* send fail */
            genie_ota_status_report();
            /* refresh timer */
            return true;
        }
        return false;
    }

    BT_DBG("b4:rx %d/%d\r\n", genie_ota_ctx.rx_size, genie_ota_ctx.image_size);
    if (genie_ota_ctx.rx_size + p_msg->header.payload_len > genie_ota_ctx.image_size)
    {
        BT_ERR("out of size, rx %ld, recv %d", genie_ota_ctx.rx_size, p_msg->header.payload_len);
        return false;
    }

    genie_ota_ctx.err_count = 0;

    if (p_msg->header.seq == 0)
    {
        genie_ota_ctx.rx_len = 0;
        memset(genie_ota_ctx.recv_buf, 0, sizeof(genie_ota_ctx.recv_buf));
    }

    if (payload_len)
    {
        BT_DBG("save %d", payload_len);
        genie_ota_ctx.flash_clean = 1;
        memcpy(genie_ota_ctx.recv_buf + genie_ota_ctx.rx_len, p_payload, payload_len);
        genie_ota_ctx.rx_len += payload_len;
    }

    genie_ota_ctx.last_seq = p_msg->header.seq;
    genie_ota_ctx.total_frame = p_msg->header.total_frame;
    genie_ota_ctx.rx_size += p_msg->header.payload_len;
    BT_DBG("rx %d/%d\r\n", genie_ota_ctx.rx_size, genie_ota_ctx.image_size);
    if (p_msg->header.seq == p_msg->header.total_frame)
    {
        genie_sal_ota_update_image(genie_ota_ctx.image_type,
                             genie_ota_ctx.rx_size - genie_ota_ctx.rx_len, genie_ota_ctx.rx_len,
                             (int *)genie_ota_ctx.recv_buf);
        genie_ota_ctx.rx_len = 0;
        genie_ota_ctx.except_seq = 0;
    }
    else
    {
        genie_ota_ctx.except_seq = p_msg->header.seq + 1;
    }

    if (genie_ota_ctx.rx_size == genie_ota_ctx.image_size || p_msg->header.seq == p_msg->header.total_frame)
    {
        genie_ota_status_report();
    }

    return true;
}

bool genie_ota_check_firmware(uint8_t msg_id, ais_ota_check_req_t *p_check_req)
{
    uint16_t crc16 = 0;
    uint8_t plaine_data[GENIE_CRYPTO_UNIT_SIZE];
    uint8_t encrypt_data[GENIE_CRYPTO_UNIT_SIZE];
    ais_ota_check_resp_t *p_check_resp = (ais_ota_check_resp_t *)plaine_data;

    genie_ais_state_set(AIS_STATE_IDLE);

    if (p_check_req->state == 1)
    {
        memset(plaine_data, 15, sizeof(plaine_data));

        p_check_resp->state = genie_sal_ota_check_checksum(genie_ota_ctx.image_type, &crc16);

        printf("check %d %04x %04x\r\n", p_check_resp->state, genie_ota_ctx.image_crc16, crc16);
        if (p_check_resp->state && crc16 != genie_ota_ctx.image_crc16)
        {
            p_check_resp->state = 0;
            BT_ERR("crc error");
        }

        if (p_check_resp->state)
        {
            genie_ais_state_set(AIS_STATE_REBOOT);
            _ais_set_ota_change();
            BT_WARN("ota success, reboot in 3s!");
        }
        else
        {
            BT_ERR("ota failed");
        }

        // genie_crypto_encrypt(plaine_data, encrypt_data);
        do_encrypt(plaine_data, GENIE_CRYPTO_UNIT_SIZE);
        genie_ais_notify(msg_id, AIS_OTA_CHECK_RESP, plaine_data, GENIE_CRYPTO_UNIT_SIZE, 1);

        return true;
    }
    else
    {
        return false;
    }
}

bool genie_ota_is_updating(void)
{
    return (genie_ota_ctx.rx_size > 0) ? 1 : 0;
}

static int ota_report_version_result_cb(transport_result_e result_e)
{
    if (result_e == SEND_RESULT_SUCCESS)
    {
        // genie_storage_delete_userdata(GFI_OTA_INDICAT);
    }

    return 0;
}

int genie_ota_report_version(void)
{
    uint8_t payload[8];
    // uint32_t version = genie_version_appver_get();
    genie_transport_payload_param_t transport_payload_param;

    // payload[0] = ATTR_TYPE_REPORT_VERSION & 0xff;
    // payload[1] = (ATTR_TYPE_REPORT_VERSION >> 8) & 0xff;
    // memcpy(&payload[2], &version, 4);
    // payload[6] = VENDOR_MODEL_VERSION;
    // payload[7] = SDK_VERSION;

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_INDICATE;
    transport_payload_param.p_payload = payload;
    transport_payload_param.payload_len = sizeof(payload);
    transport_payload_param.retry_cnt = GENIE_TRANSPORT_DEFAULT_RETRY_COUNT;
    transport_payload_param.result_cb = ota_report_version_result_cb;

    // return genie_transport_send_payload(&transport_payload_param);
    return 0;
}

int genie_ota_init(void)
{
    genie_ais_init();

    return 0;
}

int genie_ota_pre_init(void)
{
#ifdef CONFIG_GENIE_OTA_PINGPONG
    ota_check_image_id();
#endif

    genie_ais_pre_init();

    memset(&genie_ota_ctx, 0, sizeof(genie_ota_ctx_t));

    genie_sal_ota_erase();

    return 0;
}
