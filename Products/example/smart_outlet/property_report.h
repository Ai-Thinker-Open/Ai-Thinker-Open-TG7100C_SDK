/*
 *copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _PROPERTY_REPORT_H
#define _PROPERTY_REPORT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#define SPEC_SEQ "123456@1578666696145"

typedef struct _PROPERTY_REPORT_MSG {
    uint8_t powerswitch;
    uint8_t all_powerstate;
    char seq[24];
    uint32_t flag;
} property_report_msg_t;

void report_device_property(char *seq, int flag);
void ntp_server_init(void);
void process_property_report(void);
void process_property_report_task(void *argv);

#ifdef __cplusplus
}
#endif
#endif // _PROPERTY_REPORT_H
