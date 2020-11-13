/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_CONFIG_H
#define K_CONFIG_H

/* chip level conf */
#ifndef RHINO_CONFIG_LITTLE_ENDIAN
#define RHINO_CONFIG_LITTLE_ENDIAN           1
#endif
#ifndef RHINO_CONFIG_CPU_STACK_DOWN
#define RHINO_CONFIG_CPU_STACK_DOWN          1
#endif

/* kernel feature conf */
#ifndef RHINO_CONFIG_SEM
#define RHINO_CONFIG_SEM                     1
#endif
#ifndef RHINO_CONFIG_QUEUE
#define RHINO_CONFIG_QUEUE                   1
#endif
#ifndef RHINO_CONFIG_TASK_SEM
#define RHINO_CONFIG_TASK_SEM                1
#endif
#ifndef RHINO_CONFIG_EVENT_FLAG
#define RHINO_CONFIG_EVENT_FLAG              1
#endif
#ifndef RHINO_CONFIG_TIMER
#define RHINO_CONFIG_TIMER                   1
#endif
#ifndef RHINO_CONFIG_BUF_QUEUE
#define RHINO_CONFIG_BUF_QUEUE               1
#endif
#ifndef RHINO_CONFIG_MM_BLK
#define RHINO_CONFIG_MM_BLK                  1
#endif
#ifndef RHINO_CONFIG_MM_DEBUG
#define RHINO_CONFIG_MM_DEBUG                1
#endif
#ifndef RHINO_CONFIG_MM_TLF
#define RHINO_CONFIG_MM_TLF                  1
#endif
#ifndef RHINO_CONFIG_MM_MAXMSIZEBIT
#define RHINO_CONFIG_MM_MAXMSIZEBIT          24
#endif
#ifndef RHINO_CONFIG_GCC_RETADDR
#define RHINO_CONFIG_GCC_RETADDR             1
#endif
#ifndef RHINO_CONFIG_MM_LEAKCHECK
#define RHINO_CONFIG_MM_LEAKCHECK            0
#endif
#define K_MM_STATISTIC                       1
#ifndef RHINO_CONFIG_KOBJ_SET
#define RHINO_CONFIG_KOBJ_SET                1
#endif
#ifndef RHINO_CONFIG_RINGBUF_VENDOR
#define RHINO_CONFIG_RINGBUF_VENDOR          1
#endif

/* kernel task conf */
#ifndef RHINO_CONFIG_TASK_SUSPEND
#define RHINO_CONFIG_TASK_SUSPEND            1
#endif
#ifndef RHINO_CONFIG_TASK_INFO
#define RHINO_CONFIG_TASK_INFO               10
#endif
#ifndef RHINO_CONFIG_TASK_DEL
#define RHINO_CONFIG_TASK_DEL                1
#endif
#ifndef RHINO_CONFIG_TASK_WAIT_ABORT
#define RHINO_CONFIG_TASK_WAIT_ABORT         1
#endif
#ifndef RHINO_CONFIG_TASK_STACK_OVF_CHECK
#define RHINO_CONFIG_TASK_STACK_OVF_CHECK    1
#endif
#ifndef RHINO_CONFIG_SCHED_RR
#define RHINO_CONFIG_SCHED_RR                1
#endif
#ifndef RHINO_CONFIG_TIME_SLICE_DEFAULT
#define RHINO_CONFIG_TIME_SLICE_DEFAULT      10
#endif
#ifndef RHINO_CONFIG_PRI_MAX
#define RHINO_CONFIG_PRI_MAX                 62
#endif
#ifndef RHINO_CONFIG_USER_PRI_MAX
#define RHINO_CONFIG_USER_PRI_MAX            (RHINO_CONFIG_PRI_MAX - 2)
#endif

/* kernel workqueue conf */
#ifndef RHINO_CONFIG_WORKQUEUE
#define RHINO_CONFIG_WORKQUEUE               1
#endif

/* kernel mm_region conf */
#ifndef RHINO_CONFIG_MM_REGION_MUTEX
#define RHINO_CONFIG_MM_REGION_MUTEX         0
#endif

/* kernel timer&tick conf */
#ifndef RHINO_CONFIG_HW_COUNT
#define RHINO_CONFIG_HW_COUNT                1
#endif
#ifndef RHINO_CONFIG_TICK_TASK
#define RHINO_CONFIG_TICK_TASK               1
#endif
#if (RHINO_CONFIG_TICK_TASK > 0)
#ifndef RHINO_CONFIG_TICK_TASK_STACK_SIZE
#define RHINO_CONFIG_TICK_TASK_STACK_SIZE    256
#endif
#ifndef RHINO_CONFIG_TICK_TASK_PRI
#define RHINO_CONFIG_TICK_TASK_PRI           1
#endif
#endif
#ifndef RHINO_CONFIG_TICKLESS
#define RHINO_CONFIG_TICKLESS                0
#endif
#ifndef RHINO_CONFIG_TICKS_PER_SECOND
#define RHINO_CONFIG_TICKS_PER_SECOND        100
#endif
/* must be 2^n size!, such as 1, 2, 4, 8, 16,32, etc....... */
#ifndef RHINO_CONFIG_TICK_HEAD_ARRAY
#define RHINO_CONFIG_TICK_HEAD_ARRAY         8
#endif
#ifndef RHINO_CONFIG_TIMER_TASK_STACK_SIZE
#define RHINO_CONFIG_TIMER_TASK_STACK_SIZE   200
#endif
#ifndef RHINO_CONFIG_TIMER_RATE
#define RHINO_CONFIG_TIMER_RATE              1
#endif
#ifndef RHINO_CONFIG_TIMER_TASK_PRI
#define RHINO_CONFIG_TIMER_TASK_PRI          5
#endif

/* kernel intrpt conf */
#ifndef RHINO_CONFIG_INTRPT_STACK_REMAIN_GET
#define RHINO_CONFIG_INTRPT_STACK_REMAIN_GET 1
#endif
#ifndef RHINO_CONFIG_INTRPT_STACK_OVF_CHECK
#define RHINO_CONFIG_INTRPT_STACK_OVF_CHECK  0
#endif
#ifndef RHINO_CONFIG_INTRPT_MAX_NESTED_LEVEL
#define RHINO_CONFIG_INTRPT_MAX_NESTED_LEVEL 188u
#endif
#ifndef RHINO_CONFIG_INTRPT_GUARD
#define RHINO_CONFIG_INTRPT_GUARD            0
#endif

/* kernel dyn alloc conf */
#ifndef RHINO_CONFIG_KOBJ_DYN_ALLOC
#define RHINO_CONFIG_KOBJ_DYN_ALLOC          1
#endif
#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
#ifndef RHINO_CONFIG_K_DYN_QUEUE_MSG
#define RHINO_CONFIG_K_DYN_QUEUE_MSG         30
#endif
#ifndef RHINO_CONFIG_K_DYN_TASK_STACK
#define RHINO_CONFIG_K_DYN_TASK_STACK        256
#endif
#ifndef RHINO_CONFIG_K_DYN_MEM_TASK_PRI
#define RHINO_CONFIG_K_DYN_MEM_TASK_PRI      5
#endif
#endif

/* kernel idle conf */
#ifndef RHINO_CONFIG_IDLE_TASK_STACK_SIZE
#define RHINO_CONFIG_IDLE_TASK_STACK_SIZE    100
#endif

/* kernel hook conf */
#ifndef RHINO_CONFIG_USER_HOOK
#define RHINO_CONFIG_USER_HOOK               0
#endif

/* kernel stats conf */
#ifndef RHINO_CONFIG_SYSTEM_STATS
#define RHINO_CONFIG_SYSTEM_STATS            1
#endif
#ifndef RHINO_CONFIG_DISABLE_SCHED_STATS
#define RHINO_CONFIG_DISABLE_SCHED_STATS     1
#endif
#ifndef RHINO_CONFIG_DISABLE_INTRPT_STATS
#define RHINO_CONFIG_DISABLE_INTRPT_STATS    1
#endif
#ifndef RHINO_CONFIG_CPU_USAGE_STATS
#define RHINO_CONFIG_CPU_USAGE_STATS         0
#endif
#ifndef RHINO_CONFIG_CPU_USAGE_TASK_PRI
#define RHINO_CONFIG_CPU_USAGE_TASK_PRI      (RHINO_CONFIG_PRI_MAX - 2)
#endif
#ifndef RHINO_CONFIG_TASK_SCHED_STATS
#define RHINO_CONFIG_TASK_SCHED_STATS        0
#endif
#ifndef RHINO_CONFIG_CPU_USAGE_TASK_STACK
#define RHINO_CONFIG_CPU_USAGE_TASK_STACK    256
#endif

/* kernel trace conf */
#ifndef RHINO_CONFIG_TRACE
#define RHINO_CONFIG_TRACE                   1
#endif

#ifndef RHINO_CONFIG_CPU_NUM
#define RHINO_CONFIG_CPU_NUM                 1
#endif

#ifndef RHINO_CONFIG_TASK_STACK_CUR_CHECK
#define RHINO_CONFIG_TASK_STACK_CUR_CHECK    1
#endif

#endif /* K_CONFIG_H */

