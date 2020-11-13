#ifndef __OS_HAL_H__
#define __OS_HAL_H__

#define OS_USING_ALIOS
#ifdef OS_USING_ALIOS
#include <k_api.h>
#include <aos/kernel.h>

#define os_printf(...) printf(__VA_ARGS__)
#define os_free aos_free
#define os_malloc aos_malloc

#define os_tick_get krhino_sys_tick_get
#define os_thread_delay krhino_task_sleep

typedef aos_task_t os_task_t;
#define os_thread_create(name, entry, priority, stack_size, param) do { \
    aos_task_t task; \
    aos_task_new_ext( \
        &task, \
        name, \
        entry, \
        param, \
        stack_size * sizeof(cpu_stack_t), \
        priority); \
} while(0)

typedef aos_event_t os_event_t;
#define os_event_init(event)                                    aos_event_new(event, 0x00000000)
#define os_event_send(event, bits)                              aos_event_set(event, bits, RHINO_OR)
#define os_event_sendFromISR(event, bits)                       aos_event_set(event, bits, RHINO_OR)
#define os_event_recv(event, bits, timeout, act_flags)          aos_event_get(event, bits, RHINO_OR_CLEAR, (unsigned int *)&act_flags, timeout)
#define os_event_delete(event)                                  aos_event_free(event)

typedef aos_mutex_t os_mutex_t;
#define os_mutex_create(mutex)                                  aos_mutex_new(mutex)
#define os_mutex_take(mutex, timeout)                           aos_mutex_lock(&mutex, timeout)
#define os_mutex_give(mutex)                                    aos_mutex_unlock(&mutex)
#define os_mutex_giveFromISR(mutex)                             aos_mutex_unlock(&mutex)
#define OS_WAITING_FOREVER                                      AOS_WAIT_FOREVER

//#define os_interrupt_enter(...) do {} while(0)
//#define os_interrupt_leave(...) do {} while(0)
#define os_interrupt_enter() krhino_intrpt_enter()
#define os_interrupt_leave() krhino_intrpt_exit() 

typedef kbuf_queue_t os_messagequeue_t;
#define os_mq_init(mq, name, buf, msgsize, buffersize) krhino_buf_queue_create(mq, name, buf, buffersize, msgsize)
//#define os_mq_send(mq, msg, len) krhino_buf_queue_send(mq, msg, len)
static inline size_t os_mq_send(os_messagequeue_t *mq, void *msg, size_t len) {
    kstat_t ret;
    uint32_t retry_times = 0;
    while (1) {
        ret = krhino_buf_queue_send(mq, msg, len);
        if (RHINO_SUCCESS == ret) {
            break;
        }
        retry_times++;
        if ((retry_times & 0x3FF) == 0) {// 1023
            printf("os_mq_send retry = %ld, ret = %d\r\n", retry_times, (int)ret);
        }
        aos_msleep(1);
    }

    return 0;
}
//#define os_mq_recv(mq, msg, len) krhino_buf_queue_recv(mq, RHINO_WAIT_FOREVER, msg, len)
static inline size_t os_mq_recv(os_messagequeue_t *mq, void *msg, size_t buf_size) {
  kstat_t ret;
  size_t read_length;
  (void)buf_size;

  ret = krhino_buf_queue_recv(mq, RHINO_WAIT_FOREVER, msg, &read_length);
  if (ret != RHINO_SUCCESS) {
    return -1;
  }

  return 0;
}

typedef aos_timer_t os_timer_t;
typedef void * timer_cb_arg_t;
#define OS_TIMER_TYPE_ONESHOT 0
#define OS_TIMER_TYPE_REPEATED 1
#define os_timer_delete(timer) do { \
    aos_timer_stop(timer); \
    aos_timer_free(timer); \
} while (0)
#define os_timer_delete_nodelay(timer) do { \
    aos_timer_stop(timer); \
    aos_timer_free(timer); \
} while (0)
#define os_timer_init(timer, name, cb, param, ticks, type) aos_timer_new( \
    timer, \
    cb, \
    param, \
    ticks, \
    type \
)
#define os_timer_start(timer) aos_timer_start(timer);
#define os_timer_startFromISR(timer) aos_timer_stop(timer);
#define os_timer_data(timer) (timer)

#endif

#ifdef OS_USING_RTTHREAD
#include <rtthread.h>

#define os_printf(...) rt_kprintf(__VA_ARGS__)

/**
 ****************************************************************************************
 *
 * @file os_hal.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#define os_free rt_free
#define os_malloc rt_malloc

/*event definition*/
typedef struct rt_event os_event_t;
#define os_event_init(ev) rt_event_init(ev, "wifi_" #ev, RT_IPC_FLAG_FIFO);
#define os_event_send rt_event_send
#define os_event_sendFromISR rt_event_send
#define os_event_recv(irq_event, bits, timeout, val) rt_event_recv(irq_event, bits, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, timeout, &val)
#define os_event_delete rt_event_detach
/*mutex*/
typedef rt_mutex_t os_mutex_t;
#define os_mutex_create(name) rt_mutex_create(name, RT_IPC_FLAG_FIFO);
#define os_mutex_take rt_mutex_take
#define os_mutex_give rt_mutex_release
#define OS_WAITING_FOREVER RT_WAITING_FOREVER
/*int related*/
#define os_interrupt_enter(...) rt_interrupt_enter(...)
#define os_interrupt_leave(...) rt_interrupt_leave(...)
/*thread related*/
#define os_thread_delay rt_thread_delay
#define os_tick_get rt_tick_get
#define os_tick_getFromISR rt_tick_get
#define os_thread_create(name, entry, priority, stack_size, param) do { \
    rt_thread_t tid;\
    tid = rt_thread_create(name,\
        entry, param,\
        stack_size, priority, 20);\
    RT_ASSERT(RT_NULL != tid);\
    if (tid != RT_NULL) {\
        rt_thread_startup(tid);\
    }\
} while(0)
/*message queue related*/
typedef rt_messagequeue os_messagequeue_t;
#define os_mq_init(mq, name, buffer, msgsize, buffersize) rt_mq_init(\
            mq, \
            name, \
            buffer, \
            msgsize, \
            buffersize \
            RT_IPC_FLAG_FIFO \
);
#define os_mq_send(mq, msg, len) rt_mq_send(mq, msg, len);
#define os_mq_recv(mq, msg, len) rt_mq_recv(mq, msg, len, RT_WAITING_FOREVER)
/*timer related*/
typedef rt_timer_t os_timer_t;
typedef void* timer_cb_arg_t;
#define OS_TIMER_TYPE_ONESHOT RT_TIMER_FLAG_ONE_SHOT
#define os_timer_delete(timer) rt_timer_detach(timer)
#define os_timer_delete_nodelay(timer) rt_timer_detach(timer)
#define os_timer_init(timer, name, cb, param, ticks, type) rt_timer_init(timer, \
        name, \
        cb, \
        param, \
        ticks, \
        type \
);
#define os_timer_start(timer) rt_timer_start(timer)
#define os_timer_startFromISR(timer) rt_timer_start(timer)
#define os_timer_data(timer) (timer)

#endif

#ifdef OS_USING_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#include <event_groups.h>
#include <message_buffer.h>
#include <timers.h>
#include <stdio.h>

#if 0
int os_printf(const char* format, ...);
#else
#define os_printf(...) printf(__VA_ARGS__)
#endif
/*mem*/
#define os_free vPortFree
#define os_malloc pvPortMalloc

/*event definition*/
typedef StaticEventGroup_t os_event_t;
#define os_event_init(ev) xEventGroupCreateStatic(ev)
#define os_event_send(ev, bits) xEventGroupSetBits((EventGroupHandle_t)ev, bits)
#define os_event_sendFromISR(ev, bits) do { \
    BaseType_t xHigherPriorityTaskWoken, xResult; \
    (void) xHigherPriorityTaskWoken; \
    (void) xResult; \
    xResult = xEventGroupSetBitsFromISR(ev, bits, &xHigherPriorityTaskWoken); \
} while (0)

#define os_event_recv(irq_event, bits, timeout, val) do { \
    val = xEventGroupWaitBits((EventGroupHandle_t)irq_event, bits, pdTRUE, pdFALSE, timeout); \
} while (0)
#define os_event_delete vEventGroupDelete
/*mutex*/
typedef SemaphoreHandle_t os_mutex_t;
#define os_mutex_create(name) xSemaphoreCreateMutex()
#define os_mutex_take xSemaphoreTake
#define os_mutex_give xSemaphoreGive
#define os_mutex_giveFromISR xSemaphoreGiveFromISR
#define OS_WAITING_FOREVER portMAX_DELAY
/*int related*/
#define os_interrupt_enter(...) do {} while(0)
#define os_interrupt_leave(...) do {} while(0)
/*thread related*/
#define os_thread_delay vTaskDelay
#define os_tick_get xTaskGetTickCount
#define os_tick_getFromISR xTaskGetTickCountFromISR
#define os_thread_create(name, entry, priority, stack_size, param) do { \
    TaskHandle_t xHandle = NULL; \
    xTaskCreate( \
        entry, \
        name, \
        stack_size >> 2,      /* Stack size in words, not bytes. */ \
        param,    /* Parameter passed into the task. */ \
        priority, /* Priority at which the task is created. */ \
        &xHandle);      /* Used to pass out the created task's handle. */ \
} while(0)
/*message queue related*/
typedef StaticMessageBuffer_t os_messagequeue_t;
#define os_mq_init(mq, name, buffer, msgsize, buffersize) (NULL != xMessageBufferCreateStatic(buffersize, buffer, mq) ? 0 : 1)
#define os_mq_send(mq, msg, len) (xMessageBufferSend(mq, msg, len, portMAX_DELAY) > 0 ? 0 : 1)
#define os_mq_recv(mq, msg, len) (xMessageBufferReceive(mq, msg, len, portMAX_DELAY) > 0 ? 0 : 1)
/*timer related*/
typedef StaticTimer_t os_timer_t;
#define OS_TIMER_TYPE_ONESHOT pdFALSE
#define OS_TIMER_TYPE_REPEATED pdTRUE
#define os_timer_delete(timer) do { \
    xTimerStop(timer, portMAX_DELAY); \
    xTimerDelete(timer, portMAX_DELAY); \
} while (0)
#define os_timer_delete_nodelay(timer) do { \
    xTimerStop((TimerHandle_t)timer, 0); \
    xTimerDelete((TimerHandle_t)timer, 0); \
} while (0)
typedef TimerHandle_t timer_cb_arg_t;
#define os_timer_init(timer, name, cb, param, ticks, type) xTimerCreateStatic( \
    name, \
    ticks, \
    type, \
    param, \
    cb, \
    timer \
)
#define os_timer_start(timer) xTimerStart((TimerHandle_t)timer, portMAX_DELAY);
#define os_timer_startFromISR(timer) xTimerStartFromISR(timer, portMAX_DELAY);
#define os_timer_data(timer) pvTimerGetTimerID(timer)

#endif
#endif
