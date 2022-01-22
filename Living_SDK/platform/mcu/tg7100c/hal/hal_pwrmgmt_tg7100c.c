#include <stdio.h>
#include <k_api.h>
#include "pwrmgmt_api.h"
#include "wifi_mgmr_ext.h"

pwr_status_t board_cpu_pwr_init(void )
{

}

void systick_resume(void )
{

}

void systick_suspend(void )
{

}

int pwrmgmt_suspend_lowpower(void)
{
    printf("disable powersaving\r\n");
    wifi_mgmr_sta_powersaving(0);
    
    return 0;
}

int pwrmgmt_resume_lowpower(void)
{
    printf("enable powersaving\r\n");
    wifi_mgmr_sta_powersaving(2);
    
    return 0;
}
