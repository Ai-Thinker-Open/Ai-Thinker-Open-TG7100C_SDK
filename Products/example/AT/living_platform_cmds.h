#ifndef __LIVING_PLATFORM_CMDS_H__
#define __LIVING_PLATFORM_CMDS_H__

#define SDK_VERSION "1.6.6"
#define AT_VERSION  "1.2.0"
extern aos_timer_t awss_timeout_timer;


void uart_push_data(char *msg);
int at_register_cmds(void);

#endif

