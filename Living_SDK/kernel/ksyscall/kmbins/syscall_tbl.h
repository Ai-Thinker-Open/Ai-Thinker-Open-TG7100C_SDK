/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


/* --------------------k_task-------------------- */
#define K_TASK_BASE 0

#define SYS_KRHINO_CUR_TASK_GET (K_TASK_BASE + 0)
SYSCALL(SYS_KRHINO_CUR_TASK_GET, krhino_cur_task_get)

#define SYS_KRHINO_TASK_INFO_SET (K_TASK_BASE + 1)
SYSCALL(SYS_KRHINO_TASK_INFO_SET, krhino_task_info_set)

#define SYS_KRHINO_TASK_INFO_GET (K_TASK_BASE + 2)
SYSCALL(SYS_KRHINO_TASK_INFO_GET, krhino_task_info_get)

#define SYS_KRHINO_TASK_SLEEP (K_TASK_BASE + 3)
SYSCALL(SYS_KRHINO_TASK_SLEEP, krhino_task_sleep)

#define SYS_KRHINO_TASK_DYN_CREATE (K_TASK_BASE + 4)
SYSCALL(SYS_KRHINO_TASK_DYN_CREATE, krhino_task_dyn_create)

#define SYS_KRHINO_TASK_DYN_DEL (K_TASK_BASE + 5)
SYSCALL(SYS_KRHINO_TASK_DYN_DEL, krhino_task_dyn_del)


/* --------------------k_timer-------------------- */
#define K_TIMER_BASE 10

#define SYS_KRHINO_SYS_TIME_GET (K_TIMER_BASE + 0)
SYSCALL(SYS_KRHINO_SYS_TIME_GET, krhino_sys_time_get)

#define SYS_KRHINO_SYS_TICK_GET (K_TIMER_BASE + 1)
SYSCALL(SYS_KRHINO_SYS_TICK_GET, krhino_sys_tick_get)

#define SYS_KRHINO_MS_TO_TICKS (K_TIMER_BASE + 2)
SYSCALL(SYS_KRHINO_MS_TO_TICKS, krhino_ms_to_ticks)

#define SYS_KRHINO_TICKS_TO_MS (K_TIMER_BASE + 3)
SYSCALL(SYS_KRHINO_TICKS_TO_MS, krhino_ticks_to_ms)

#define SYS_KRHINO_TIMER_DYN_CREATE (K_TIMER_BASE + 4)
SYSCALL(SYS_KRHINO_TIMER_DYN_CREATE, krhino_timer_dyn_create)

#define SYS_KRHINO_TIMER_DYN_DEL (K_TIMER_BASE + 5)
SYSCALL(SYS_KRHINO_TIMER_DYN_DEL, krhino_timer_dyn_del)

#define SYS_KRHINO_TIMER_START (K_TIMER_BASE + 6)
SYSCALL(SYS_KRHINO_TIMER_START, krhino_timer_start)

#define SYS_KRHINO_TIMER_STOP (K_TIMER_BASE + 7)
SYSCALL(SYS_KRHINO_TIMER_STOP, krhino_timer_stop)

#define SYS_KRHINO_TIMER_CHANGE (K_TIMER_BASE + 8)
SYSCALL(SYS_KRHINO_TIMER_CHANGE, krhino_timer_change)


/* --------------------k_mutex-------------------- */
#define K_MUTEX_BASE 20

#define SYS_KRHINO_MUTEX_CREATE (K_MUTEX_BASE + 0)
SYSCALL(SYS_KRHINO_MUTEX_CREATE, krhino_mutex_create)

#define SYS_KRHINO_MUTEX_DEL (K_MUTEX_BASE + 1)
SYSCALL(SYS_KRHINO_MUTEX_DEL, krhino_mutex_del)

#define SYS_KRHINO_MUTEX_LOCK (K_MUTEX_BASE + 2)
SYSCALL(SYS_KRHINO_MUTEX_LOCK, krhino_mutex_lock)

#define SYS_KRHINO_MUTEX_UNLOCK (K_MUTEX_BASE + 3)
SYSCALL(SYS_KRHINO_MUTEX_UNLOCK, krhino_mutex_unlock)

/* --------------------k_sem-------------------- */
#define K_SEM_BASE 25

#define SYS_KRHINO_SEM_CREATE (K_SEM_BASE + 0)
SYSCALL(SYS_KRHINO_SEM_CREATE, krhino_sem_create)

#define SYS_KRHINO_SEM_DEL (K_SEM_BASE + 1)
SYSCALL(SYS_KRHINO_SEM_DEL, krhino_sem_del)

#define SYS_KRHINO_SEM_TAKE (K_SEM_BASE + 2)
SYSCALL(SYS_KRHINO_SEM_TAKE, krhino_sem_take)

#define SYS_KRHINO_SEM_GIVE (K_SEM_BASE + 3)
SYSCALL(SYS_KRHINO_SEM_GIVE, krhino_sem_give)

/* --------------------k_mm-------------------- */
#define K_MM_BASE 30

#define SYS_KRHINO_MM_ALLOC (K_MM_BASE + 0)
SYSCALL(SYS_KRHINO_MM_ALLOC, krhino_mm_alloc)

#define SYS_KRHINO_MM_FREE (K_MM_BASE + 1)
SYSCALL(SYS_KRHINO_MM_FREE, krhino_mm_free)

#define SYS_KRHINO_MM_REALLOC (K_MM_BASE + 2)
SYSCALL(SYS_KRHINO_MM_REALLOC, krhino_mm_realloc)

/* ----------------k_buf_queue----------------- */
#define K_BUF_QUEUE_BASE 35

#define SYS_KRHINO_BUF_QUEUE_SEND (K_BUF_QUEUE_BASE + 0)
SYSCALL(SYS_KRHINO_BUF_QUEUE_SEND, krhino_buf_queue_send)

#define SYS_KRHINO_BUF_QUEUE_RECV (K_BUF_QUEUE_BASE + 1)
SYSCALL(SYS_KRHINO_BUF_QUEUE_RECV, krhino_buf_queue_recv)

#define SYS_KRHINO_BUF_QUEUE_CREATE (K_BUF_QUEUE_BASE + 2)
SYSCALL(SYS_KRHINO_BUF_QUEUE_CREATE, krhino_buf_queue_create)

#define SYS_KRHINO_BUF_QUEUE_DEL (K_BUF_QUEUE_BASE + 3)
SYSCALL(SYS_KRHINO_BUF_QUEUE_DEL, krhino_buf_queue_del)


/* --------------------vfs-------------------- */
#define VFS_BASE 40

#define SYS_AOS_POLL (VFS_BASE + 0)
SYSCALL(SYS_AOS_POLL, aos_poll)


/* --------------------yloop-------------------- */
#define YLOOP_BASE 60

#define SYS_REGISTER_EVENT_FILTER (YLOOP_BASE + 0)
SYSCALL(SYS_REGISTER_EVENT_FILTER, aos_register_event_filter)

#define SYS_UNREGISTER_EVENT_FILTER (YLOOP_BASE + 1)
SYSCALL(SYS_UNREGISTER_EVENT_FILTER, aos_unregister_event_filter)

#define SYS_POST_EVENT (YLOOP_BASE + 2)
SYSCALL(SYS_POST_EVENT, aos_post_event)

#define SYS_POLL_READ_FD (YLOOP_BASE + 3)
SYSCALL(SYS_POLL_READ_FD, aos_poll_read_fd)

#define SYS_CANCEL_POLL_READ_FD (YLOOP_BASE + 4)
SYSCALL(SYS_CANCEL_POLL_READ_FD, aos_cancel_poll_read_fd)

#define SYS_POST_DELAYED_ACTION (YLOOP_BASE + 5)
SYSCALL(SYS_POST_DELAYED_ACTION, aos_post_delayed_action)

#define SYS_CANCEL_DELAYED_ACTION (YLOOP_BASE + 6)
SYSCALL(SYS_CANCEL_DELAYED_ACTION, aos_cancel_delayed_action)

#define SYS_SCHEDULE_CALL (YLOOP_BASE + 7)
SYSCALL(SYS_SCHEDULE_CALL, aos_schedule_call)

#define SYS_LOOP_INIT (YLOOP_BASE + 8)
SYSCALL(SYS_LOOP_INIT, aos_loop_init)

#define SYS_CURRENT_LOOP (YLOOP_BASE + 9)
SYSCALL(SYS_CURRENT_LOOP, aos_current_loop)

#define SYS_LOOP_RUN (YLOOP_BASE + 10)
SYSCALL(SYS_LOOP_RUN, aos_loop_run)

#define SYS_LOOP_EXIT (YLOOP_BASE + 11)
SYSCALL(SYS_LOOP_EXIT, aos_loop_exit)

#define SYS_LOOP_DESTROY (YLOOP_BASE + 12)
SYSCALL(SYS_LOOP_DESTROY, aos_loop_destroy)

#define SYS_LOOP_SCHEDULE_CALL (YLOOP_BASE + 13)
SYSCALL(SYS_LOOP_SCHEDULE_CALL, aos_loop_schedule_call)

#define SYS_LOOP_SCHEDULE_WORK (YLOOP_BASE +14)
SYSCALL(SYS_LOOP_SCHEDULE_WORK, aos_loop_schedule_work)

#define SYS_CANCEL_WORK (YLOOP_BASE + 15)
SYSCALL(SYS_CANCEL_WORK, aos_cancel_work)
/* ----------------end YLOOP------------------ */

/* ------------------ALINK---------------------- */
#define OTA_BASE 80

#define SYS_AIS_OTA_BT_STORAGE_INIT (OTA_BASE + 0)
SYSCALL(SYS_AIS_OTA_BT_STORAGE_INIT, ais_ota_bt_storage_init)

#define SYS_AIS_OTA_GET_LOCAL_ADDR (OTA_BASE + 1)
SYSCALL(SYS_AIS_OTA_GET_LOCAL_ADDR, ais_ota_get_local_addr)


/* ------------------ALINK---------------------- */
#define ALINK_BASE 90

#define SYS_ALINK_START (ALINK_BASE + 0)
SYSCALL(SYS_ALINK_START, alink_start)

#define SYS_ALINK_END (ALINK_BASE + 1)
SYSCALL(SYS_ALINK_END, alink_end)

#define SYS_ALINK_POST (ALINK_BASE + 2)
SYSCALL(SYS_ALINK_POST, alink_post)

#define SYS_ALINK_POST_FAST (ALINK_BASE + 3)
SYSCALL(SYS_ALINK_POST_FAST, alink_post_fast)


/* ------------------BLE---------------------- */
#define BLE_BASE 100

#define SYS_HRS_INIT (BLE_BASE + 0)
SYSCALL(SYS_HRS_INIT, hrs_init)

#define SYS_HRS_NOTIFY (BLE_BASE + 1)
SYSCALL(SYS_HRS_NOTIFY, hrs_notify)

#define SYS_BAS_INIT (BLE_BASE + 2)
SYSCALL(SYS_BAS_INIT, bas_init)

#define SYS_BAS_NOTIFY (BLE_BASE + 3)
SYSCALL(SYS_BAS_NOTIFY, bas_notify)

#define SYS_DIS_INIT (BLE_BASE + 4)
SYSCALL(SYS_DIS_INIT, dis_init)

#define SYS_BT_LE_ADV_START (BLE_BASE + 5)
SYSCALL(SYS_BT_LE_ADV_START, bt_le_adv_start)

#define SYS_BT_CONN_GET_DST (BLE_BASE + 6)
SYSCALL(SYS_BT_CONN_GET_DST, bt_conn_get_dst)

#define SYS_BT_CONN_REF (BLE_BASE + 7)
SYSCALL(SYS_BT_CONN_REF, bt_conn_ref)

#define SYS_BT_CONN_SECURITY (BLE_BASE + 8)
SYSCALL(SYS_BT_CONN_SECURITY, bt_conn_security)

#define SYS_HCI_DRIVER_INIT (BLE_BASE + 9)
SYSCALL(SYS_HCI_DRIVER_INIT, hci_driver_init)

#define SYS_BT_ENABLE (BLE_BASE + 10)
SYSCALL(SYS_BT_ENABLE, bt_enable)

#define SYS_BT_CONN_AUTH_CB_REGISTER (BLE_BASE + 11)
SYSCALL(SYS_BT_CONN_AUTH_CB_REGISTER, bt_conn_auth_cb_register)

#define SYS_BT_CONN_CB_REGISTER (BLE_BASE + 12)
SYSCALL(SYS_BT_CONN_CB_REGISTER, bt_conn_cb_register)

#define SYS_BT_CONN_UNREF (BLE_BASE + 13)
SYSCALL(SYS_BT_CONN_UNREF, bt_conn_unref)

#define SYS_BT_ALI_RESET (BLE_BASE + 14)
SYSCALL(SYS_BT_ALI_RESET, ali_reset)

#define SYS_BT_ALI_INIT (BLE_BASE + 15)
SYSCALL(SYS_BT_ALI_INIT, ali_init)

#define SYS_BT_ALI_SEND_INDICATE (BLE_BASE + 16)
SYSCALL(SYS_BT_ALI_SEND_INDICATE, ali_send_indicate)

#define SYS_BT_ALI_SEND_NOTIFY (BLE_BASE + 17)
SYSCALL(SYS_BT_ALI_SEND_NOTIFY, ali_send_notify)

#define SYS_BT_ALI_GET_MANUF_SPEC_ADV_DATA (BLE_BASE + 18)
SYSCALL(SYS_BT_ALI_GET_MANUF_SPEC_ADV_DATA, ali_get_manuf_spec_adv_data)

#define SYS_BT_FETCH_ALI_CONTEXT (BLE_BASE + 19)
SYSCALL(SYS_BT_FETCH_ALI_CONTEXT, fetch_ali_context)


/* --------------------OTHERS-------------------- */
#define OTHERS_BASE 130

#define SYS_GET_ERRNO (OTHERS_BASE + 0)
SYSCALL(SYS_GET_ERRNO, get_errno)

#define SYS_SET_ERRNO (OTHERS_BASE + 1)
SYSCALL(SYS_SET_ERRNO, set_errno)

#define SYS_HAL_UART_SEND (OTHERS_BASE + 2)
SYSCALL(SYS_HAL_UART_SEND, hal_uart_send)

#define SYS_VPRINTF (OTHERS_BASE + 3)
SYSCALL(SYS_VPRINTF, aos_vprintf)

#define SYS_FFLUSH (OTHERS_BASE + 4)
SYSCALL(SYS_FFLUSH, aos_fflush)



