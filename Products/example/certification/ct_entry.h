#ifndef __CT_ENTRY_H__
#define __CT_ENTRY_H__

#define CT_MAIN_THREAD_STACKSZIE (8 * 1024)
#define CT_DUMP_INTERVAL_KV_KEY "ctdump"

extern void user_event_monitor(int event);

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
void *ct_entry_get_uota_ctx(void);
#endif
#endif
