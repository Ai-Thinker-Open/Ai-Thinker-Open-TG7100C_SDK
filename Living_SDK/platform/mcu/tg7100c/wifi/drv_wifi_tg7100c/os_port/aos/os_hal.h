#ifndef __OS_HAL_H__
#define __OS_HAL_H__

#include <stdint.h>
#include <aos/kernel.h>
#include "k_api.h"

#define OS_TIMER_TYPE_ONESHOT  0
#define OS_TIMER_TYPE_REPEATED 1
#define OS_WAITING_FOREVER     AOS_WAIT_FOREVER

typedef ktask_t os_task_t;
typedef aos_event_t os_event_t;
typedef aos_mutex_t os_mutex_t;
typedef aos_sem_t os_sem_t;
typedef aos_queue_t os_messagequeue_t;
typedef void *timer_cb_arg_t;
typedef uint64_t os_sys_time_t;

typedef struct os_timer {
    aos_timer_t timer;
    void *p_arg;
    void (*cb)(void *);
} os_timer_t;

#define os_ticks_to_ms(ticks)  krhino_ticks_to_ms(ticks)
#define os_critical_enter()    CPSR_ALLOC(); RHINO_CPU_INTRPT_DISABLE();
#define os_critical_exit()     RHINO_CPU_INTRPT_ENABLE()

int os_printf(const char *fmt, ...);
void bl_os_free(void *ptr);
void *bl_os_malloc(uint32_t size);

os_sys_time_t bl_os_tick_get(void);
void bl_os_thread_delay(uint32_t ms);

os_task_t *bl_os_cur_thread_get(void);
int bl_os_thread_create(const char *name, void (*entry)(void *),
                     int prio, int stack_size, void *arg);

int bl_os_thread_notify_create(os_task_t *task, const char *name);
int bl_os_thread_notify_give(os_task_t *task);
int bl_os_thread_notify_take(uint32_t tick);
int bl_os_thread_notify_delete(os_task_t *task);

int bl_os_event_init(os_event_t *event);
int bl_os_event_send(os_event_t *event, uint32_t bits);
int bl_os_event_sendFromISR(os_event_t *event,uint32_t bits);
int bl_os_event_recv(os_event_t *event, uint32_t bits, uint32_t timeout, uint32_t *act_flags);
int bl_os_event_delete(os_event_t *event);

int bl_os_mutex_create(os_mutex_t *mutex);
int bl_os_mutex_take(os_mutex_t *mutex, uint32_t timeout);
int bl_os_mutex_give(os_mutex_t *mutex);
int bl_os_mutex_giveFromISR(os_mutex_t *mutex);

int bl_os_sem_create(os_sem_t *sem);
int bl_os_sem_is_valid(os_sem_t *sem);
int bl_os_sem_take(os_sem_t *sem, uint32_t timeout);
int bl_os_sem_give(os_sem_t *sem);
int bl_os_sem_delete(os_sem_t *sem);

int bl_os_mq_init(os_messagequeue_t *mq, const char *name, void *buf,
               uint32_t msgsize, uint32_t buffersize);
int bl_os_mq_send(os_messagequeue_t *mq, void *msg, uint32_t len);
int bl_os_mq_recv(os_messagequeue_t *mq, void *msg, uint32_t buf_size);

int bl_os_timer_delete(os_timer_t *timer);
int bl_os_timer_delete_nodelay(os_timer_t *timer);
int bl_os_timer_init(os_timer_t *timer, const char *name, void (*cb)(void *),
                  void *param, uint32_t ticks, int type);
int bl_os_timer_start(os_timer_t *timer);
int bl_os_timer_startFromISR(os_timer_t *timer);
timer_cb_arg_t bl_os_timer_data(timer_cb_arg_t arg);

#endif
