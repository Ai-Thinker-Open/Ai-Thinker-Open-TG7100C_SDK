#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "cJSON.h"
#include <aos/aos.h>
#include <aos/yloop.h>
#include "iot_export.h"
#include "living_platform_cmds.h"
#include "at_main.h"


#define MB_RGBSTATUS_COUNT 10   //队列缓存10级
aos_queue_t *g_property_report_queue_id = NULL; //上行数据队列句柄
static char *g_property_report_queue_buff = NULL;

aos_queue_t *g_cmd_msg_queue_id = NULL;  //消息数据队列句柄
static char *g_cmd_msg_queue_buff = NULL;

void init_msg_queue(void)
{
    if(g_property_report_queue_buff == NULL){
        g_property_report_queue_id = (aos_queue_t *) aos_malloc(sizeof(aos_queue_t));
        g_property_report_queue_buff = aos_malloc(MB_RGBSTATUS_COUNT * 512);

        aos_queue_new(g_property_report_queue_id, g_property_report_queue_buff,
                MB_RGBSTATUS_COUNT * 512, 512);
    }

    if(g_cmd_msg_queue_buff == NULL){
        g_cmd_msg_queue_id = (aos_queue_t *) aos_malloc(sizeof(aos_queue_t));
        g_cmd_msg_queue_buff = aos_malloc(MB_RGBSTATUS_COUNT * 512);

        aos_queue_new(g_cmd_msg_queue_id, g_cmd_msg_queue_buff, MB_RGBSTATUS_COUNT * 512, 512);
    }
}


void user_port_recv(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    unsigned int rcvLen;
    char msg[512] = {0};
    int ret = -1;
    while(1){
        if (aos_queue_recv(g_property_report_queue_id, AOS_WAIT_FOREVER, msg, &rcvLen) == 0) {
            printf("\n\n\nqueue recv data: %s\n\n\n",(char *)msg);
            // uart_push_data(msg);
            ret = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char*)msg, strlen(msg));
            if(ret != -1)
            {
                uart_push_data("OK\r\n");
                memset(msg,0,sizeof(msg));
            }
            else
            {
                uart_push_data("ERROR\r\n");
                memset(msg,0,sizeof(msg));
            }      
        }
    }
}

void send_msg_to_queue(char *msg)
{
    int ret = aos_queue_send(g_cmd_msg_queue_id, msg, strlen(msg));
    if (0 != ret)
        LOG_TRACE("###############ERROR: CMD MSG: aos_queue_send failed! #################\r\n");
}