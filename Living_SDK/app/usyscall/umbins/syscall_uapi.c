/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <syscall_uapi.h>
#include <k_api.h>
#include <aos/aos.h>
#include <hal/hal.h>
#ifdef CONFIG_AOS_MESH
#include <umesh.h>
#endif

#ifdef MBEDTLS_IN_KERNEL
#include <ali_crypto.h>
#endif

#define SYSCALL(nr, func)

#include <syscall_tbl.h>

#ifdef WITH_LWIP
#include <aos/network.h>
#endif

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include "ali_common.h"
#include "ali_core.h"



/* --------------------function-------------------- */

int __wrap_vprintf(char *format, va_list param)
{
    return aos_vprintf(format, param);
}

int __wrap_fflush(FILE *stream)
{
    return aos_fflush(stream);
}
/* --------------------function-------------------- */

/* --------------------k_task-------------------- */
ktask_t *krhino_cur_task_get(void)
{
    return SYS_CALL0(SYS_KRHINO_CUR_TASK_GET, ktask_t *);
}

kstat_t krhino_task_info_set(ktask_t *task, size_t idx, void *info)
{
    return SYS_CALL3(SYS_KRHINO_TASK_INFO_SET, kstat_t, ktask_t *, task, 
                    size_t, idx, void *, info);
}

kstat_t krhino_task_info_get(ktask_t *task, size_t idx, void **info)
{
    return SYS_CALL3(SYS_KRHINO_TASK_INFO_GET, kstat_t, ktask_t *, task, 
                    size_t, idx, void **, info);
}

kstat_t krhino_task_sleep(tick_t dly)
{
    return SYS_CALL1(SYS_KRHINO_TASK_SLEEP, kstat_t, tick_t, dly);
}

kstat_t krhino_task_dyn_create(ktask_t **task, const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, size_t stack,
                               task_entry_t entry, uint8_t autorun)
{
    return SYS_CALL8(SYS_KRHINO_TASK_DYN_CREATE, kstat_t, ktask_t **, task, const name_t *, name,
                    void *, arg, uint8_t, pri, tick_t, ticks, size_t, stack,
                        task_entry_t, entry, uint8_t, autorun);
}

kstat_t krhino_task_dyn_del(ktask_t *task)
{
    return SYS_CALL1(SYS_KRHINO_TASK_DYN_DEL, kstat_t, ktask_t *, task);
}

/* --------------------k_timer-------------------- */

sys_time_t krhino_sys_time_get(void)
{
    return SYS_CALL0(SYS_KRHINO_SYS_TIME_GET, sys_time_t);
}

sys_time_t krhino_sys_tick_get(void)
{
    return SYS_CALL0(SYS_KRHINO_SYS_TICK_GET, sys_time_t);
}

tick_t krhino_ms_to_ticks(sys_time_t ms)
{
    return SYS_CALL1(SYS_KRHINO_MS_TO_TICKS, tick_t, sys_time_t, ms);
}

sys_time_t krhino_ticks_to_ms(tick_t ticks)
{
    return SYS_CALL1(SYS_KRHINO_TICKS_TO_MS, sys_time_t, tick_t, ticks);
}

kstat_t krhino_timer_dyn_create(ktimer_t **timer, const name_t *name,
                                timer_cb_t cb,
                                sys_time_t first, sys_time_t round, void *arg, uint8_t auto_run)
{
    return SYS_CALL7(SYS_KRHINO_TIMER_DYN_CREATE, kstat_t, ktimer_t **, timer, const name_t *, name,
                        timer_cb_t, cb, sys_time_t, first, sys_time_t, round, void *, arg, 
                            uint8_t, auto_run);
}

kstat_t krhino_timer_dyn_del(ktimer_t *timer)
{
    return SYS_CALL1(SYS_KRHINO_TIMER_DYN_DEL, kstat_t, ktimer_t *, timer);
}

kstat_t krhino_timer_start(ktimer_t *timer)
{
    return SYS_CALL1(SYS_KRHINO_TIMER_START, kstat_t, ktimer_t *, timer);
}

kstat_t krhino_timer_stop(ktimer_t *timer)
{
    return SYS_CALL1(SYS_KRHINO_TIMER_STOP, kstat_t, ktimer_t *, timer);
}

kstat_t krhino_timer_change(ktimer_t *timer, sys_time_t first, sys_time_t round)
{
    return SYS_CALL3(SYS_KRHINO_TIMER_CHANGE, kstat_t, ktimer_t *, timer, sys_time_t, first, sys_time_t, round);
}


/* --------------------k_mutex-------------------- */
kstat_t krhino_mutex_create(kmutex_t *mutex, const name_t *name)
{
    return SYS_CALL2(SYS_KRHINO_MUTEX_CREATE, kstat_t, kmutex_t *, mutex, const name_t *, name);
}

kstat_t krhino_mutex_del(kmutex_t *mutex)
{
    return SYS_CALL1(SYS_KRHINO_MUTEX_DEL, kstat_t, kmutex_t *, mutex);
}

kstat_t krhino_mutex_lock(kmutex_t *mutex, tick_t ticks)
{
    return SYS_CALL2(SYS_KRHINO_MUTEX_LOCK, kstat_t, kmutex_t *, mutex, tick_t, ticks);
}

kstat_t krhino_mutex_unlock(kmutex_t *mutex)
{
    return SYS_CALL1(SYS_KRHINO_MUTEX_UNLOCK, kstat_t, kmutex_t *, mutex);
}

/* --------------------k_sem-------------------- */

kstat_t krhino_sem_create(ksem_t *sem, const name_t *name, sem_count_t count)
{
    return SYS_CALL3(SYS_KRHINO_SEM_CREATE, kstat_t, ksem_t *, sem, const name_t *, name, 
                    sem_count_t, count);

}

kstat_t krhino_sem_del(ksem_t *sem)
{
    return SYS_CALL1(SYS_KRHINO_SEM_DEL, kstat_t, ksem_t *, sem);
}

kstat_t krhino_sem_take(ksem_t *sem, tick_t ticks)
{
    return SYS_CALL2(SYS_KRHINO_SEM_TAKE, kstat_t, ksem_t *, sem, tick_t, ticks);
}

kstat_t krhino_sem_give(ksem_t *sem)
{
    return SYS_CALL1(SYS_KRHINO_SEM_GIVE, kstat_t, ksem_t *, sem);
}

/* --------------------k_mm-------------------- */
void *krhino_mm_alloc(size_t size)
{
    return SYS_CALL1(SYS_KRHINO_MM_ALLOC, void *, size_t, size);
}

void krhino_mm_free(void *ptr)
{
    SYS_CALL1(SYS_KRHINO_MM_FREE, void, void *, ptr);
}

void *krhino_mm_realloc(void *oldmem, size_t newsize)
{
    return SYS_CALL2(SYS_KRHINO_MM_REALLOC, void *, void *, oldmem, size_t, newsize);
}

/* ----------------k_buf_queue----------------- */
kstat_t krhino_buf_queue_send(kbuf_queue_t *queue, void *msg, size_t size)
{
    return SYS_CALL3(SYS_KRHINO_BUF_QUEUE_SEND, kstat_t, kbuf_queue_t *, queue, void *, msg,
                    size_t, size);
}

kstat_t krhino_buf_queue_recv(kbuf_queue_t *queue, tick_t ticks, void *msg,
                              size_t *size)
{
    return SYS_CALL4(SYS_KRHINO_BUF_QUEUE_RECV, kstat_t, kbuf_queue_t *, queue, tick_t, ticks,
                    void *, msg, size_t *, size);
}

kstat_t krhino_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                void *buf, size_t size, size_t max_msg)
{
    return SYS_CALL5(SYS_KRHINO_BUF_QUEUE_CREATE, kstat_t, kbuf_queue_t *, queue,
                    const name_t *, name, void *, buf, size_t, size, size_t, max_msg);

}

kstat_t krhino_buf_queue_del(kbuf_queue_t *queue)
{
    return SYS_CALL1(SYS_KRHINO_BUF_QUEUE_DEL, kstat_t, kbuf_queue_t *, queue);
}

/* --------------------vfs-------------------- */
int aos_poll(struct pollfd *fds, int nfds, int timeout)
{
    return SYS_CALL3(SYS_AOS_POLL, int, struct pollfd *, fds, int, nfds, int, timeout);
}


/* --------------------Framework-------------------- */
typedef void (*aos_event_cb)(input_event_t *event, void *private_data);
typedef void (*aos_call_t)(void *arg);
typedef void (*aos_poll_call_t)(int fd, void *arg);

int aos_register_event_filter(uint16_t type, aos_event_cb cb, void *priv)
{
    return SYS_CALL3(SYS_REGISTER_EVENT_FILTER, int, uint16_t, type,
                     aos_event_cb, cb, void *, priv);
}

int aos_unregister_event_filter(uint16_t type, aos_event_cb cb, void *priv)
{
    return SYS_CALL3(SYS_UNREGISTER_EVENT_FILTER, int, uint16_t, type,
                     aos_event_cb, cb, void *, priv);
}

int aos_post_event(uint16_t type, uint16_t code, unsigned long value)
{
    return SYS_CALL3(SYS_POST_EVENT, int, uint16_t, type, uint16_t, code,
                     unsigned long, value);
}

int aos_poll_read_fd(int fd, aos_poll_call_t action, void *param)
{
    return SYS_CALL3(SYS_POLL_READ_FD, int, int, fd, aos_poll_call_t, action,
                     void *, param);
}

void aos_cancel_poll_read_fd(int fd, aos_poll_call_t action, void *param)
{
    return SYS_CALL3(SYS_CANCEL_POLL_READ_FD, void, int, fd,
                     aos_poll_call_t, action, void *, param);
}

int aos_post_delayed_action(int ms, aos_call_t action, void *arg)
{
    return SYS_CALL3(SYS_POST_DELAYED_ACTION, int, int, ms, aos_call_t, action,
                     void *, arg);
}

void aos_cancel_delayed_action(int ms, aos_call_t action, void *arg)
{
    return SYS_CALL3(SYS_CANCEL_DELAYED_ACTION, void, int, ms,
                     aos_call_t, action, void *, arg);
}

int aos_schedule_call(aos_call_t action, void *arg)
{
    return SYS_CALL2(SYS_SCHEDULE_CALL, int, aos_call_t, action, void *, arg);
}

typedef void *aos_loop_t;

aos_loop_t aos_loop_init(void)
{
    return SYS_CALL0(SYS_LOOP_INIT, aos_loop_t);
}

aos_loop_t aos_current_loop(void)
{
    return SYS_CALL0(SYS_CURRENT_LOOP, aos_loop_t);
}

void aos_loop_run(void)
{
    return SYS_CALL0(SYS_LOOP_RUN, void);
}

void aos_loop_exit(void)
{
    return SYS_CALL0(SYS_LOOP_EXIT, void);
}

void aos_loop_destroy(void)
{
    return SYS_CALL0(SYS_LOOP_DESTROY, void);
}

int aos_loop_schedule_call(aos_loop_t *loop, aos_call_t action, void *arg)
{
    return SYS_CALL3(SYS_LOOP_SCHEDULE_CALL, int, aos_loop_t *, loop,
                     aos_call_t, action, void *, arg);
}

void *aos_loop_schedule_work(int ms, aos_call_t action, void *arg1,
                             aos_call_t fini_cb, void *arg2)
{
    return SYS_CALL5(SYS_LOOP_SCHEDULE_WORK, void *, int, ms,
                     aos_call_t, action, void *, arg1, aos_call_t, fini_cb,
                     void *, arg2);
}

void aos_cancel_work(void *work, aos_call_t action, void *arg1)
{
    return SYS_CALL3(SYS_CANCEL_WORK, void, void *, work, aos_call_t, action,
                     void *, arg1);
}


/* --------------------OTA-------------------- */

int ais_ota_bt_storage_init(void)
{
    return SYS_CALL0(SYS_AIS_OTA_BT_STORAGE_INIT, int);
}

int ais_ota_get_local_addr(bt_addr_le_t *addr)
{
    return SYS_CALL1(SYS_AIS_OTA_GET_LOCAL_ADDR, int, bt_addr_le_t *, addr);
}

/* --------------------ALINK-------------------- */

int alink_start(struct device_config *dev_conf)
{
    SYS_CALL1(SYS_ALINK_START, int, struct device_config *, dev_conf);
}

int alink_end(void)
{
    SYS_CALL0(SYS_ALINK_END, int);
}

void alink_post(uint8_t *buffer, uint32_t length)
{
    SYS_CALL2(SYS_ALINK_POST, void, uint8_t *, buffer, uint32_t, length);
}

void alink_post_fast(uint8_t *buffer, uint32_t length)
{
    SYS_CALL2(SYS_ALINK_POST_FAST, void, uint8_t *, buffer, uint32_t, length);

}

/* --------------------BLE-------------------- */

void hrs_init(u8_t blsc)
{
    SYS_CALL1(SYS_HRS_INIT, void, u8_t, blsc);
}

void hrs_notify(void)
{
    SYS_CALL0(SYS_HRS_NOTIFY, void);
}

void bas_init(void)
{
    SYS_CALL0(SYS_BAS_INIT, void);
}

void bas_notify(void)
{
    SYS_CALL0(SYS_BAS_NOTIFY, void);
}

void dis_init(const char *model, const char *manuf)
{
    SYS_CALL2(SYS_DIS_INIT, void, const char *, model, const char *, manuf);
}

int bt_le_adv_start(const struct bt_le_adv_param *param,
		    const struct bt_data *ad, size_t ad_len,
		    const struct bt_data *sd, size_t sd_len)
{
    return SYS_CALL5(SYS_BT_LE_ADV_START, int, const struct bt_le_adv_param *, param,
        const struct bt_data *, ad, size_t, ad_len,
        const struct bt_data *, sd, size_t, sd_len);
}

const bt_addr_le_t *bt_conn_get_dst(const struct bt_conn *conn)
{
    return SYS_CALL1(SYS_BT_CONN_GET_DST, const bt_addr_le_t *, const struct bt_conn *, conn);
}

struct bt_conn *bt_conn_ref(struct bt_conn *conn)
{
    return SYS_CALL1(SYS_BT_CONN_REF, struct bt_conn *, struct bt_conn *, conn);
}

int bt_conn_security(struct bt_conn *conn, bt_security_t sec)
{
    return SYS_CALL2(SYS_BT_CONN_SECURITY, int, struct bt_conn *, conn, bt_security_t, sec);
}

int hci_driver_init(void)
{
    return SYS_CALL0(SYS_HCI_DRIVER_INIT, int);
}

int bt_enable(bt_ready_cb_t cb)
{
    return SYS_CALL1(SYS_BT_ENABLE, int, bt_ready_cb_t, cb);
}

int bt_conn_auth_cb_register(const struct bt_conn_auth_cb *cb)
{
    return SYS_CALL1(SYS_BT_CONN_AUTH_CB_REGISTER, int, const struct bt_conn_auth_cb *, cb);
}

void bt_conn_cb_register(struct bt_conn_cb *cb)
{
    SYS_CALL1(SYS_BT_CONN_CB_REGISTER, void, struct bt_conn_cb *, cb);
}

void bt_conn_unref(struct bt_conn *conn)
{
    SYS_CALL1(SYS_BT_CONN_UNREF, void, struct bt_conn *, conn);
}

void ali_reset(void * p_ali)
{
    SYS_CALL1(SYS_BT_ALI_RESET, void, void *, p_ali);
}

ret_code_t ali_init(void * p_ali, ali_init_t const * p_init)
{
    return SYS_CALL2(SYS_BT_ALI_INIT, ret_code_t, void *, p_ali, ali_init_t const *, p_init);
}

ret_code_t ali_send_indicate(void * p_ali, uint8_t * p_data, uint16_t length)
{
    return SYS_CALL3(SYS_BT_ALI_SEND_INDICATE, ret_code_t, void *, p_ali, uint8_t *, p_data, uint16_t, length);
}

ret_code_t ali_send_notify(void * p_ali, uint8_t * p_data, uint16_t length)
{
    return SYS_CALL3(SYS_BT_ALI_SEND_NOTIFY, ret_code_t, void *, p_ali, uint8_t *, p_data, uint16_t, length);
}

ret_code_t ali_get_manuf_spec_adv_data(void * p_ali, uint8_t * p_data, uint16_t * length)
{
    return SYS_CALL3(SYS_BT_ALI_GET_MANUF_SPEC_ADV_DATA, ret_code_t, void *, p_ali, uint8_t *, p_data, uint16_t, length);
}

uint32_t *fetch_ali_context()
{
    return SYS_CALL0(SYS_BT_FETCH_ALI_CONTEXT, uint32_t *);
}


/* -----------------end BLE-------------------- */


/* -----------------OTHERS--------------------- */

int get_errno(void)
{
    return SYS_CALL0(SYS_GET_ERRNO, int);
}

void set_errno(int err)
{
    return SYS_CALL1(SYS_SET_ERRNO, void, int, err);
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    return SYS_CALL4(SYS_HAL_UART_SEND, int32_t, uart_dev_t *, uart, const void *, data,
        uint32_t, size, uint32_t, timeout);
}

int aos_vprintf(char *format, va_list param)
{
    return SYS_CALL2(SYS_VPRINTF, int, char *, format, va_list, param);
}

int aos_fflush(FILE *stream)
{
    return SYS_CALL1(SYS_FFLUSH, int, FILE *, stream);
}

/* -----------------end OTHERS-------------------- */


