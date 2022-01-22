#include "bl_timer.h"
#include "bl_irq.h"
#include <tg7100c_timer.h>

//#include <FreeRTOS.h>
//#include <task.h>
#include <aos/kernel.h>
#include <k_api.h>

#define MTIMER_TICKS_PER_US     (1)
uint32_t bl_timer_now_us(void)
{
    uint32_t tick_now;

    tick_now = *(volatile uint32_t*)0x4000A52C;
    return MTIMER_TICKS_PER_US * tick_now;
}

int64_t bl2_timer_now_us(void)
{
    uint64_t h, l;

    l = (*(volatile uint32_t*)0x0200BFF8);
    h = (*(volatile uint32_t*)0x0200BFFC);

    return ((int64_t)(h<<32 | l))/10;
}

int64_t bl2_timer_now_ms(void)
{
    int64_t tmp;

    tmp = bl2_timer_now_us()/1000;

    return tmp;
}

uint32_t bl_delay_us(uint32_t us)
{
    volatile uint64_t now;
    uint64_t cur;

    now = (uint64_t)(bl2_timer_now_us());
    cur = now + us;

    while (now < cur) {
        now = bl2_timer_now_us();
    }

    return us;
}

static BL_Err_Type Timer_INT_Case(TIMER_CFG_Type *timerCfg)
{
    /* Disable all interrupt */
    TIMER_IntMask(timerCfg->timerCh,TIMER_INT_ALL, MASK);

    /* Disable timer before config */
    TIMER_Disable(timerCfg->timerCh);

    /* Timer init with default configuration */
    TIMER_Init(timerCfg);

    /* Clear interrupt status*/
    TIMER_ClearIntStatus(timerCfg->timerCh,TIMER_COMP_ID_0);
    TIMER_ClearIntStatus(timerCfg->timerCh,TIMER_COMP_ID_1);
    TIMER_ClearIntStatus(timerCfg->timerCh,TIMER_COMP_ID_2);

    /* Enable timer match interrupt */
    TIMER_IntMask(timerCfg->timerCh, TIMER_INT_COMP_0, UNMASK);
    TIMER_IntMask(timerCfg->timerCh, TIMER_INT_COMP_1, MASK);
    TIMER_IntMask(timerCfg->timerCh, TIMER_INT_COMP_2, MASK);

    /* Enable timer */
    TIMER_Enable(timerCfg->timerCh);

    return SUCCESS;
}

static void int_timer_cb(void)
{
    TIMER_ClearIntStatus(TIMER_CH1, TIMER_COMP_ID_0);
#if 0
    if (xTaskIncrementTick()) {
        vTaskSwitchContext();
    }
#else
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
#endif
}

int bl_timer_tick_enable(void)
{
    TIMER_CFG_Type timerCh1Cfg = 
    {
        TIMER_CH1,                           /* timer channel 1 */
        TIMER_CLKSRC_FCLK,                   /* timer clock source:bus clock */
        TIMER_PRELOAD_TRIG_COMP0,            /* reaload on comaparator 0  */
        TIMER_COUNT_PRELOAD,                 /* preload when match occur */
        15,                                 /* Timer clock divison value */
        //10 * 1000 * 1000 * 2,                /* match value 0  */
        10 * 1 * 1000 * 1 - 2,                /* match value 0  */
        360000,                              /* match value 1 */
        390000,                              /* match value 2 */
        0,                                   /* preload value */
    };

    Timer_INT_Case(&timerCh1Cfg);

    bl_irq_enable(TIMER_CH1_IRQn);
    bl_irq_register(TIMER_CH1_IRQn, int_timer_cb);

    return 0;
}
