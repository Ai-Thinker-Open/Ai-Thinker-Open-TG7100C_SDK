/*
 *copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __MSG_PROCESS_CENTER_H__
#define __MSG_PROCESS_CENTER_H__

typedef enum _MSG_FROM{
    FROM_PROPERTY_SET = 0,
    FROM_SERVICE_SET
} msg_from_t;

typedef struct _RECV_MSG{
    uint8_t powerswitch;
    uint8_t all_powerstate;
    int flag;
    char seq[24];
    uint8_t method;
    uint8_t from;
} recv_msg_t;

void init_msg_queue(void);
void send_msg_to_queue(recv_msg_t* cmd_msg);
void msg_process_task(void *argv);

#endif
