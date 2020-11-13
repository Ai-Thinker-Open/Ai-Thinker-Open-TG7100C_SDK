#ifndef __GATEWAY_ENTRY_H__
#define __GATEWAY_ENTRY_H__

#define GATEWAY_MAIN_THREAD_STACKSZIE (8 * 1024)
#define GATEWAY_DUMP_INTERVAL_KV_KEY "gwdump"

extern void user_event_monitor(int event);

#endif
