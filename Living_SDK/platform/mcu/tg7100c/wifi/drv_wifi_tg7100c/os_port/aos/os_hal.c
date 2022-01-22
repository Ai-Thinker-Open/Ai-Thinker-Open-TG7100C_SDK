#include "os_hal.h"
#include "stdarg.h"

int os_printf(const char *fmt, ...)
{
#if CFG_WIFI_DEBUG
extern void vprint(const char *fmt, va_list argp);

    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
#endif
    return 0;
}

void bl_os_free(void *ptr)
{
    aos_free(ptr);
}

void *bl_os_malloc(uint32_t size)
{
    return aos_malloc(size);
}

os_sys_time_t bl_os_tick_get(void)
{
    return krhino_sys_tick_get();
}

void bl_os_thread_delay(uint32_t ms)
{
    aos_msleep(ms);
}

os_task_t *bl_os_cur_thread_get(void)
{
    return krhino_cur_task_get();
}

int bl_os_thread_create(const char *name, void (*entry)(void *), int prio, int stack_size, void *arg)
{
    aos_task_t task;
    return aos_task_new_ext(&task, name, entry, arg, stack_size, prio);
}

int bl_os_thread_notify_create(os_task_t *task, const char *name)
{
    ksem_t *p_sem = bl_os_malloc(sizeof(ksem_t));
    if (p_sem == NULL) {
        return -1;
    }
    return krhino_task_sem_create(task, p_sem, name, 0);
}

int bl_os_thread_notify_give(os_task_t *task)
{
    return krhino_task_sem_give(task);
}

int bl_os_thread_notify_take(uint32_t tick)
{
    krhino_task_sem_take(tick);
    return 0;
}

int bl_os_thread_notify_delete(os_task_t *task)
{
    krhino_task_sem_del(task);
    bl_os_free(task->task_sem_obj);
    return 0;
}

int bl_os_event_init(os_event_t *event)
{
    return aos_event_new(event, 0);
}

int bl_os_event_send(os_event_t *event, uint32_t bits)
{
    return aos_event_set(event, bits, RHINO_OR);
}

int bl_os_event_sendFromISR(os_event_t *event,uint32_t bits)
{
    
    return aos_event_set(event, bits, RHINO_OR);
}

int bl_os_event_recv(os_event_t *event, uint32_t bits, uint32_t timeout, uint32_t *act_flags)
{
    return aos_event_get(event, bits, RHINO_OR_CLEAR, (unsigned int *)act_flags, timeout);
}

int bl_os_event_delete(os_event_t *event)
{
    aos_event_free(event);
    return 0;
}

int bl_os_mutex_create(os_mutex_t *mutex) 
{
    return aos_mutex_new(mutex);
}

int bl_os_mutex_take(os_mutex_t *mutex, uint32_t timeout)
{ 
    return aos_mutex_lock(mutex, timeout);
}

int bl_os_mutex_give(os_mutex_t *mutex)                                    
{
    return aos_mutex_unlock(mutex);
}
int bl_os_mutex_giveFromISR(os_mutex_t *mutex)                             
{
    return aos_mutex_unlock(mutex);
}

int bl_os_sem_create(os_sem_t *sem)
{
    return aos_sem_new(sem, 0);
}
int bl_os_sem_is_valid(os_sem_t *sem)
{
    return aos_sem_is_valid(sem);
}
int bl_os_sem_take(os_sem_t *sem, uint32_t timeout)
{
    return aos_sem_wait(sem, timeout);
}
int bl_os_sem_give(os_sem_t *sem)
{
    aos_sem_signal(sem);
    return 0;
}
int bl_os_sem_delete(os_sem_t *sem)
{
    aos_sem_free(sem);
    return 0;
}

int bl_os_mq_init(os_messagequeue_t *mq, const char *name, void *buf, uint32_t msgsize, uint32_t buffersize)
{
    return aos_queue_new(mq, buf, buffersize, msgsize);
}

int bl_os_mq_send(os_messagequeue_t *mq, void *msg, uint32_t len)
{
    return krhino_buf_queue_send(mq->hdl, msg, len);
}

int bl_os_mq_recv(os_messagequeue_t *mq, void *msg, uint32_t buf_size)
{
    int ret;
    unsigned int read_length;
    (void)buf_size;
    
    ret = aos_queue_recv(mq, OS_WAITING_FOREVER, msg, &read_length);
    if (ret != 0) {
      return -1;
    }
    
    return 0;
}

int bl_os_timer_delete(os_timer_t *timer) 
{ 
    aos_timer_stop(&timer->timer); 
    aos_timer_free(&timer->timer); 
    return 0;
} 

int bl_os_timer_delete_nodelay(os_timer_t *timer)
{ 
    aos_timer_stop(&timer->timer); 
    aos_timer_free(&timer->timer); 
    return 0;
} 

static void __timer_cb(void *timer, void *p_arg)
{
    os_timer_t *p_timer = (os_timer_t *)p_arg;
    if (p_timer->cb) {
        p_timer->cb(p_timer->p_arg);
    }
}

int bl_os_timer_init(os_timer_t *timer, const char *name, void (*cb)(void *), void *param, uint32_t ticks, int type) 
{
    timer->cb = cb;
    timer->p_arg = param;
    return aos_timer_new(&timer->timer, __timer_cb, timer, ticks, type);
}

int bl_os_timer_start(os_timer_t *timer) 
{
    return aos_timer_start(&timer->timer);
}

int bl_os_timer_startFromISR(os_timer_t *timer) 
{
    return aos_timer_stop(&timer->timer);
}

timer_cb_arg_t bl_os_timer_data(timer_cb_arg_t arg)
{
    return arg;
}

