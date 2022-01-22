#include "bl_rtc.h"


void bl_rtc_init(void)
{
#ifdef CFG_USE_XTAL32K
    HBN_32K_Sel(HBN_32K_XTAL);
#else
    HBN_32K_Sel(HBN_32K_RC);
#endif
    
    HBN_Clear_RTC_Counter();
    HBN_Enable_RTC_Counter();
}

uint64_t bl_rtc_get_counter(void)
{
    uint32_t valLow, valHigh;
    
    HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
    
    return ((uint64_t)valHigh << 32) | valLow;
}

uint64_t bl_rtc_get_timestamp_ms(void)
{
    uint64_t cnt;
    
    cnt = bl_rtc_get_counter();
    
    return (cnt >> 5) - (cnt >> 11) - (cnt >> 12);
}
