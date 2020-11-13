/**
 * @file lv_task.c
 * An 'lv_task'  is a void (*fp) (void* param) type function which will be called periodically.
 * A priority (5 levels + disable) can be assigned to lv_tasks. 
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "lv_task.h"
#include "../lv_hal/lv_hal_tick.h"
#include "../../lv_conf.h"

/*********************
 *      DEFINES
 *********************/
#define IDLE_MEAS_PERIOD    500     /*[ms]*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_task_exec(lv_task_t* lv_task_p, lv_task_prio_t prio_act);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_ll_t lv_task_ll;  /*Linked list to store the lv_tasks*/
static bool lv_task_run = false;
static uint8_t idle_last = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init the lv_task module
 */
void lv_task_init(void)
{
    lv_ll_init(&lv_task_ll, sizeof(lv_task_t));
    
    /*Initially enable the lv_task handling*/
    lv_task_enable(true);
}

/**
 * Call it  periodically to handle lv_tasks.
 */
inline void LV_ATTRIBUTE_TASK_HANDLER lv_task_handler(void)
{
    static uint32_t idle_period_start = 0;
    static uint32_t handler_start = 0;
    static uint32_t busy_time = 0;

	if(lv_task_run == false) return;

	handler_start = lv_tick_get();

    lv_task_t* lv_task_prio_a[LV_TASK_PRIO_NUM]; /*Lists for all prio.*/
    lv_task_prio_t prio_act;
    bool prio_reset = false;  /*Used to go back to the highest priority*/
    lv_task_t* lv_task_next;

    /*Init. the lists*/
    for(prio_act = LV_TASK_PRIO_LOWEST; prio_act <= LV_TASK_PRIO_HIGHEST; prio_act++) {
        lv_task_prio_a[prio_act] = lv_ll_get_head(&lv_task_ll);
    }

    /*Handle the lv_tasks on all priority*/
    for(prio_act = LV_TASK_PRIO_HIGHEST; prio_act > LV_TASK_PRIO_OFF; prio_act --) {
        /*Reset the prio. if necessary*/
        if(prio_reset != false) {
            prio_reset = false;
            prio_act = LV_TASK_PRIO_HIGHEST; /*Go again with highest prio */
        }

        /* Read all lv_task on 'prio_act' but stop on 'prio_reset' */
        while(lv_task_prio_a[prio_act] != NULL && prio_reset == false)  {
            /* Get the next task. (Invalid pointer if a lv_task deletes itself)*/
            lv_task_next = lv_ll_get_next(&lv_task_ll, lv_task_prio_a[prio_act]);

            /*Execute the current lv_task*/
            bool executed = lv_task_exec(lv_task_prio_a[prio_act], prio_act);
            if(executed != false) {     /*If the task is executed*/
                /* During the execution higher priority lv_tasks
                 * can be ready, so reset the priority if it is not highest*/
                if(prio_act != LV_TASK_PRIO_HIGHEST) {
                    prio_reset = true;
                }
            }

            lv_task_prio_a[prio_act] = lv_task_next; /*Load the next task*/
        }

        /*Reset higher priority lists on 'prio_reset' query*/
        if(prio_reset != false) {
            for(prio_act = prio_act + 1; prio_act <= LV_TASK_PRIO_HIGHEST; prio_act++) {
                lv_task_prio_a[prio_act] = lv_ll_get_head(&lv_task_ll);
            }
        }
    }


    busy_time += lv_tick_elaps(handler_start);
    uint32_t idle_period_time = lv_tick_elaps(idle_period_start);
    if(idle_period_time >= IDLE_MEAS_PERIOD) {

        idle_last = (uint32_t)((uint32_t)busy_time * 100) / IDLE_MEAS_PERIOD;   /*Calculate the busy percentage*/
        idle_last = idle_last > 100 ? 0 : 100 - idle_last;                      /*But we need idle time*/
        busy_time = 0;
        idle_period_start = lv_tick_get();


    }
}

/**
 * Create a new lv_task
 * @param task a function which is the task itself
 * @param period call period in ms unit
 * @param prio priority of the task (LV_TASK_PRIO_OFF means the task is stopped)
 * @param param free parameter
 * @return pointer to the new task
 */
lv_task_t* lv_task_create(void (*task) (void *), uint32_t period, lv_task_prio_t prio, void * param)
{
    lv_task_t* new_lv_task;
    
    new_lv_task = lv_ll_ins_head(&lv_task_ll);
    lv_mem_assert(new_lv_task);
    
    new_lv_task->period = period;
    new_lv_task->task = task;
    new_lv_task->prio = prio;
    new_lv_task->param = param;
    new_lv_task->once = 0;
    new_lv_task->last_run = lv_tick_get();

    return new_lv_task;
}

/**
 * Delete a lv_task
 * @param lv_task_p pointer to task created by lv_task_p
 */
void lv_task_del(lv_task_t* lv_task_p) 
{
    lv_ll_rem(&lv_task_ll, lv_task_p);
    
    lv_mem_free(lv_task_p);
}

/**
 * Set new priority for a lv_task
 * @param lv_task_p pointer to a lv_task
 * @param prio the new priority
 */
void lv_task_set_prio(lv_task_t* lv_task_p, lv_task_prio_t prio)
{
    lv_task_p->prio = prio;
}

/**
 * Set new period for a lv_task
 * @param lv_task_p pointer to a lv_task
 * @param period the new period
 */
void lv_task_set_period(lv_task_t* lv_task_p, uint32_t period)
{
    lv_task_p->period = period;
}

/**
 * Make a lv_task ready. It will not wait its period.
 * @param lv_task_p pointer to a lv_task.
 */
void lv_task_ready(lv_task_t* lv_task_p)
{
    lv_task_p->last_run = lv_tick_get() - lv_task_p->period - 1;
}

/**
 * Delete the lv_task after one call
 * @param lv_task_p pointer to a lv_task.
 */
void lv_task_once(lv_task_t * lv_task_p)
{
    lv_task_p->once = 1;
}

/**
 * Reset a lv_task. 
 * It will be called the previously set period milliseconds later.
 * @param lv_task_p pointer to a lv_task.
 */
void lv_task_reset(lv_task_t* lv_task_p)
{
    lv_task_p->last_run = lv_tick_get();
}

/**
 * Enable or disable the whole lv_task handling
 * @param en: true: lv_task handling is running, false: lv_task handling is suspended
 */
void lv_task_enable(bool en)
{
	lv_task_run = en;
}

/**
 * Get idle percentage
 * @return the lv_task idle in percentage
 */
uint8_t lv_task_get_idle(void)
{
    return idle_last;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Execute task if its the priority is appropriate 
 * @param lv_task_p pointer to lv_task
 * @param prio_act the current priority
 * @return true: execute, false: not executed
 */
static bool lv_task_exec (lv_task_t* lv_task_p, lv_task_prio_t prio_act)
{
    bool exec = false;
    
    /*Execute lv_task if its prio is 'prio_act'*/
    if(lv_task_p->prio == prio_act) {
        /*Execute if at least 'period' time elapsed*/
        uint32_t elp = lv_tick_elaps(lv_task_p->last_run);
        if(elp >= lv_task_p->period) {
            lv_task_p->last_run = lv_tick_get();
            lv_task_p->task(lv_task_p->param);

            /*Delete if it was a one shot lv_task*/
            if(lv_task_p->once != 0) lv_task_del(lv_task_p);

            exec = true;
        }
    }
    
    return exec;
}

