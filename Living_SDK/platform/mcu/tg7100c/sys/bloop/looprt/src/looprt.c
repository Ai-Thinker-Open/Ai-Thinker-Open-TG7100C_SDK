#include <stdio.h>

#include <looprt.h>
#include <aos/kernel.h>
#include <k_api.h>

/* Build the rocket with wheels*/
static struct loop_ctx looprt;

static void proc_entry_looprt(void *pvParameters)
{
    bloop_run(&looprt);

    /* Never Reach Here*/
    while (1) {
        puts("--->>> Error terminated looprt\r\n");
        krhino_task_sleep(1000);
    }
}

void looprt_evt_notify_async(unsigned int task, uint32_t evt_map)
{
    bloop_evt_set_async(&looprt, task, evt_map);
}

void looprt_evt_notify_async_fromISR(unsigned int task, uint32_t evt_map)
{
    bloop_evt_set_async_fromISR(&looprt, task, evt_map);
}

void looprt_evt_status_dump(void)
{
    bloop_status_dump(&looprt);
}

void looprt_evt_schedule(int task, uint32_t evt_map, int delay_ms)
{
    struct loop_timer *timer;

    timer = aos_malloc(sizeof(struct loop_timer));
    if (NULL == timer) {
        return;
    }
    bloop_timer_init(timer, 1);
    bloop_timer_configure(timer, delay_ms, NULL, NULL, task, evt_map);
    bloop_timer_register(&looprt, timer);
}

#if 0
int looprt_start(StackType_t *proc_stack_looprt, int stack_count, StaticTask_t *proc_task_looprt)
{
    bloop_init(&looprt);

    /* Register necessary looper handler before looper starts*/
    bloop_handler_register(&looprt, &bloop_handler_sys, LOOP_TASK_PRIORITY_HIGHEST);

    looprt_evt_status_dump();

    xTaskCreateStatic(proc_entry_looprt, (char*)"bloop_rt", stack_count, NULL, 26, proc_stack_looprt, proc_task_looprt);

    bloop_wait_startup(&looprt);

    return 0;
}
#endif

int looprt_start_auto(void)
{
    ktask_t *thread;
    ksem_t *sem;

    bloop_init(&looprt);

    /* Register necessary looper handler before looper starts*/
    bloop_handler_register(&looprt, &bloop_handler_sys, LOOP_TASK_PRIORITY_HIGHEST);

    looprt_evt_status_dump();

    //xTaskCreate(proc_entry_looprt, (char*)"bloop_rt", 4096, NULL, 26, NULL);
    // XXX(Zhuoran.rong) 
    krhino_task_dyn_create(&thread, "bloop_rt", NULL, 26, 10, 4096, proc_entry_looprt, RHINO_TRUE);
    krhino_sem_dyn_create(&sem, "bloop_rt_sem", 0);
    
    // XXX(Zhuoran.rong) trick code here
    thread->task_sem_obj = sem;

    bloop_wait_startup(&looprt);

    return 0;
}

int looprt_handler_register(const struct loop_evt_handler *handler, int priority)
{
    if (NULL == looprt.looper) {
        return -1;
    }
    return bloop_handler_register(&looprt, handler, priority);
}

int looprt_timer_register(struct loop_timer *timer)
{
    bloop_timer_register(&looprt, timer);

    return 0;
}
