/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __GENIE_AIS_H__
#define __GENIE_AIS_H__
#include "log.h"
#include "bzopt.h"

#define AIS_DISCONNECT_TIMEOUT 1000 //1s

#define AIS_SERVICE_UUID BT_UUID_DECLARE_16(0xFEB3)
#define AIS_READ_UUID BT_UUID_DECLARE_16(0xFED4)
#define AIS_WRITE_UUID BT_UUID_DECLARE_16(0xFED5)
#define AIS_INDICATE_UUID BT_UUID_DECLARE_16(0xFED6)
#define AIS_WRITE_WO_RESP_UUID BT_UUID_DECLARE_16(0xFED7)
#define AIS_NOTIFY_UUID BT_UUID_DECLARE_16(0xFED8)

typedef enum
{
    GENIE_EVT_NONE = 0,
    GENIE_EVT_SW_RESET,
    GENIE_EVT_HW_RESET_START, /* triggered from user */
    GENIE_EVT_BT_READY,
    GENIE_EVT_MESH_READY, // Used to sync device's state with cloud
    GENIE_EVT_NODE_RESET,

    GENIE_EVT_SDK_AIS_DISCON = 10,
    GENIE_EVT_SDK_AIS_CONNECT,
    GENIE_EVT_SDK_MESH_PBADV_START,
    GENIE_EVT_SDK_MESH_PBADV_TIMEOUT,
    GENIE_EVT_SDK_MESH_SILENT_START,

    GENIE_EVT_SDK_MESH_PROV_START = 20,
    GENIE_EVT_SDK_MESH_PROV_DATA,
    GENIE_EVT_SDK_MESH_PROV_TIMEOUT,
    GENIE_EVT_SDK_MESH_PROV_SUCCESS,
    GENIE_EVT_SDK_MESH_PROV_FAIL,

    GENIE_EVT_SDK_APPKEY_ADD = 30,
    GENIE_EVT_SDK_APPKEY_DEL,
    GENIE_EVT_SDK_APPKEY_UPDATE,
    GENIE_EVT_SDK_NETKEY_ADD,
    GENIE_EVT_SDK_NETKEY_DEL,
    GENIE_EVT_SDK_NETKEY_UPDATE,
    GENIE_EVT_SDK_IVI_UPDATE,
    GENIE_EVT_SDK_APPKEY1_ADD,

    GENIE_EVT_SDK_SUB_ADD = 40,
    GENIE_EVT_SDK_SUB_DEL,
    GENIE_EVT_SDK_HB_SET,
#ifdef CONFIG_BT_MESH_CTRL_RELAY
    GENIE_EVT_SDK_CTRL_RELAY_SET,
#endif
    GENIE_EVT_SDK_SEQ_UPDATE,

    GENIE_EVT_TIMEOUT = 50,
    GENIE_EVT_DOWN_MSG,
    GENIE_EVT_VENDOR_MODEL_MSG,
    GENIE_EVT_SIG_MODEL_MSG,
    GENIE_EVT_USER_TRANS_CYCLE,
    GENIE_EVT_USER_ACTION_DONE,

    GENIE_EVT_PROVISIONER_DISCOVER_DEVICE = 100,
    GENIE_EVT_PROVISIONER_COMFIRM_START,
    GENIE_EVT_PROVISIONER_AUTHORIZE_START,
    GENIE_EVT_PROVISIONER_PROV_FAIL,
    GENIE_EVT_PROVISIONER_PROV_COMPLETE,
    GENIE_EVT_PROVISIONER_BIND_DEVICE,
    GENIE_EVT_PROVISIONER_REPORT_DEVICE_STATUS,
    GENIE_EVT_PROVISIONER_MESH_STACK_READY,
    GENIE_EVT_PROVISIONER_NODE_ONLINE_STATUS,
    GENIE_EVT_PROVISIONER_DISCOVER_DEVICE_WITH_AUTH,
    GENIE_EVT_PROVISIONER_BIND_COMPLETE,

#ifdef __IOT_REMOTE_PROV_CLIENT_ENABLE__
    GENIE_EVT_REMOTE_PROV_DISCOVER_DEVICE = 150,
#endif
} genie_event_e;

enum
{
    AIS_STATE_DISCON,
    AIS_STATE_CONNECT,
    AIS_STATE_AUTH,
    AIS_STATE_IDLE,
    AIS_STATE_OTA,
    AIS_STATE_REBOOT,
};

enum
{
    AIS_RESP_ERR = 0x0F,
    AIS_SCRT_RANDOM = 0x10,
    AIS_SCRT_CIPHER = 0x11,
    AIS_SCRT_RESULT = 0x12,
    AIS_SCRT_ACK = 0x13,
    AIS_LINK_STATUS = 0x14,
    AIS_LINK_ACK = 0x15,
};

//AIS PDU FORMAT
typedef struct
{
    uint8_t msg_id : 4;
    uint8_t enc : 1;
    uint8_t ver : 3;
    uint8_t cmd;
    uint8_t seq : 4;
    uint8_t total_frame : 4;
    uint8_t payload_len;
} __attribute__((packed)) ais_header_t;

typedef struct
{
    uint8_t random[16];
} __attribute__((packed)) ais_scrt_random_t;

typedef struct
{
    uint8_t cipher[16];
} __attribute__((packed)) ais_scrt_cipher_t;

typedef struct
{
    uint8_t result;
} __attribute__((packed)) ais_scrt_result_t;

typedef struct
{
    uint8_t ack;
} __attribute__((packed)) ais_scrt_ack_t;

typedef struct
{
    ais_header_t header;
    uint8_t payload[16];
} __attribute__((packed)) ais_pdu_t;

typedef struct _genie_ais_ctx_s
{
    uint8_t state;
    struct bt_conn *p_conn;
    k_timer_t state_update_timer;
    k_timer_t disconnect_timer;
} genie_ais_ctx_t;

/**
 * @brief the ais service action when gatt is connected.
 * @param[in] the handle of connect.
 */
void genie_ais_connect(struct bt_conn *p_conn);

/**
 * @brief the ais service action when gatt is disconnected.
 * @param[in] the disconnect reason.
 */
void genie_ais_disconnect(uint8_t reason);

/**
 * @brief regist the ais service.
 * @return the result of registion.
 */
int genie_ais_init(void);

int genie_ais_pre_init(void);

int genie_ais_state_set(uint8_t state);

uint8_t genie_ais_state_get(void);

void genie_ais_notify(uint8_t msg_id, uint8_t cmd, uint8_t *p_msg, uint16_t len, uint8_t enc);

#endif
