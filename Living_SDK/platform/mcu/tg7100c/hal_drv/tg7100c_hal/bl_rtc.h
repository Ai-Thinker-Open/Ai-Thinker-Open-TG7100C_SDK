#ifndef __BL_RTC_H__
#define __BL_RTC_H__


#include "tg7100c_hbn.h"


void bl_rtc_init(void);
uint64_t bl_rtc_get_counter(void);
uint64_t bl_rtc_get_timestamp_ms(void);


#endif
