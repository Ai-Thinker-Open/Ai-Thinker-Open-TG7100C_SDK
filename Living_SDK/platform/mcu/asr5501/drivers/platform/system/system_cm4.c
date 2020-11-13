#include "lega_cm4.h"
#include "core_cm4.h"

void lega_system_reset(void)
{
    //disable irq when reboot
    __disable_irq();

#ifdef HIGHFREQ_MCU160_SUPPORT
    if(system_core_clk == SYSTEM_CORE_CLOCK_HIGH)
        lega_clk_sel_low();
#endif

    NVIC_SystemReset();
}

void SystemInit(void)
{
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif
}


void SystemCoreClockUpdate(void)
{

}




/********END OF FILE ***********/
