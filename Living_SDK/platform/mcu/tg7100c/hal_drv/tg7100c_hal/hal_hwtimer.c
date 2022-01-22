
#include "bl_irq.h"
#include <tg7100c_timer.h>
#include <utils_list.h>
#include <blog.h>
#include <hal_hwtimer.h>
#include <tg7100c_glb.h>
#include <aos/kernel.h>

#define HW_TIMER_CHANNEL   TIMER_CH0  
#define HW_TIMER_IRQn      TIMER_CH0_IRQn
#define HD_MS_TO_VAL       40000 

struct hw_timer_ctx {
    aos_mutex_t hwtimer_mux;
    utils_dlist_t *pstqueue;
};

static void hwtimer_process(void *ctx)
{
    hw_timer_t *node;
    void (*handler)(void *) = NULL;
    hw_timer_t tmpnode;
    struct hw_timer_ctx *pstctx;
    pstctx = (struct hw_timer_ctx *)ctx;
    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_ALL, MASK);
    TIMER_ClearIntStatus(HW_TIMER_CHANNEL, TIMER_COMP_ID_0);

    utils_dlist_for_each_entry(pstctx->pstqueue, node, hw_timer_t, dlist_item) {
        node->calc_time = node->calc_time + 1; 
        if (node->triggle_time <= node->calc_time) {
            handler = node->handler;
            if (node->repeat == 0) {
                memcpy(&tmpnode, node, sizeof(hw_timer_t));
                utils_dlist_del(&(node->dlist_item));
                aos_free(node);
                node = &tmpnode;
            } else {
                node->calc_time = 0;
            }
            handler(node->arg);
        }
    }

    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_COMP_0, UNMASK);
}

int hal_hwtimer_init(void)
{
    struct hw_timer_ctx *pstctx;
    TIMER_CFG_Type hw_timercfg = 
    {
        TIMER_CH0,                           /* timer channel 1 */
        TIMER_CLKSRC_XTAL,                    /* timer clock source:bus clock */
        TIMER_PRELOAD_TRIG_COMP0,            /* reaload on comaparator 0  */
        TIMER_COUNT_PRELOAD,                 /* preload when match occur */
        0,                                   /* Timer clock divison value */
        HD_MS_TO_VAL * 1 - 2,                 /* match value 0  */
        32000000,                            /* match value 1 */
        32000000,                            /* match value 2 */
        0,                                   /* preload value */
    };
    

    pstctx = aos_malloc(sizeof(struct hw_timer_ctx));
    if (pstctx == NULL) {
        blog_error("create ctx failed \r\n");
        return -1;
    }

    pstctx->pstqueue = aos_malloc(sizeof(utils_dlist_t));
    if (pstctx->pstqueue == NULL) {
        aos_free(pstctx);
        blog_error("create queue failed \r\n");

        return -1;
    }

    if (aos_mutex_new(&pstctx->hwtimer_mux)) {
        aos_free(pstctx->pstqueue);
        aos_free(pstctx);
        blog_error("create mux failed \r\n");

        return -1;
    }
    
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_TMR);
    TIMER_IntMask(hw_timercfg.timerCh,TIMER_INT_ALL, MASK);
    TIMER_Disable(hw_timercfg.timerCh);
    TIMER_Init(&hw_timercfg);

    INIT_UTILS_DLIST_HEAD(pstctx->pstqueue);
    bl_irq_register_with_ctx(HW_TIMER_IRQn, hwtimer_process, pstctx);
    bl_irq_enable(HW_TIMER_IRQn);
    TIMER_Enable(HW_TIMER_CHANNEL);

    return 0;
}

hw_timer_t *hal_hwtimer_create(uint32_t period, hw_t handler, void *arg, int repeat)
{ 
    hw_timer_t *pstnode;
    struct hw_timer_ctx *pstctx;
    
    printf("period:%lu, handler:%p,repeat:%d\r\n", period, handler, repeat);
    if (period <= 0 || handler == NULL || (repeat != 0 && repeat != 1)) {
        blog_error("para illegal , create timer failed \r\n");

        return NULL;
    }
    
    bl_irq_ctx_get(TIMER_CH0_IRQn, (void **)&pstctx);
    if( aos_mutex_lock(&pstctx->hwtimer_mux, AOS_WAIT_FOREVER) == 0 ) {
        blog_info("get mux success \r\n");
    }

    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_ALL, MASK); 
     
    pstnode = aos_malloc(sizeof(hw_timer_t));
    pstnode->triggle_time = period;
    pstnode->repeat = repeat;
    pstnode->calc_time = 0;
    pstnode->handler = handler;
    pstnode->arg = arg;
    utils_dlist_add(&(pstnode->dlist_item), pstctx->pstqueue);
    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_COMP_0, UNMASK);
    aos_mutex_unlock(&pstctx->hwtimer_mux);

    return pstnode;
}

int hal_hwtimer_delete(hw_timer_t *pstnode)
{
    hw_timer_t *node;
    int ret = 0;
    struct hw_timer_ctx *pstctx;
    
    bl_irq_ctx_get(TIMER_CH0_IRQn, (void **)&pstctx);
    if( aos_mutex_lock(&pstctx->hwtimer_mux, AOS_WAIT_FOREVER) == 0 ) {
        blog_info("get mux success \r\n");
    }

    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_ALL, MASK);
    utils_dlist_for_each_entry(pstctx->pstqueue, node, hw_timer_t, dlist_item) {
        if (pstnode == node) {
            break;
        }
    }

    if (&node->dlist_item == pstctx->pstqueue) {
        blog_error("not find node \r\n");
        ret  = -1;
    }

    if (ret == 0) {
        utils_dlist_del(&(node->dlist_item));
    }
   
    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_COMP_0, UNMASK);
    aos_mutex_unlock(&pstctx->hwtimer_mux);
    return ret;
}

int hal_hwtimer_change_period(hw_timer_t *pstnode, uint32_t period)
{
    hw_timer_t *node;
    int ret = 0;
    struct hw_timer_ctx *pstctx;

    if (period <= 0) {
        blog_error("period illegal , change period failed \r\n");
        return -1;
    }

    bl_irq_ctx_get(TIMER_CH0_IRQn, (void **)&pstctx);
    if( aos_mutex_lock(&pstctx->hwtimer_mux, AOS_WAIT_FOREVER) == 0 ) {
        blog_info("get mux success \r\n");
    }

    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_ALL, MASK);
    utils_dlist_for_each_entry(pstctx->pstqueue, node, hw_timer_t, dlist_item) {
        if (pstnode == node) {
            break;
        }
    }

    if (&node->dlist_item == pstctx->pstqueue) {
        blog_error("not find node \r\n");
        ret  = -1;
    }

    if (ret == 0) {
        node->triggle_time = period;
        node->calc_time = 0;
    }

    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_COMP_0, UNMASK);
    aos_mutex_unlock(&pstctx->hwtimer_mux);
    return ret;
}

