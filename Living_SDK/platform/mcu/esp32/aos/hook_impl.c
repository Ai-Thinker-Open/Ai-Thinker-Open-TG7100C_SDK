/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "frxt/xtensa_config.h"

void soc_hw_timer_init()
{
}

#if (RHINO_CONFIG_USER_HOOK > 0)
void krhino_idle_pre_hook(void)
{

}
void krhino_idle_hook(void)
{
}

void krhino_init_hook(void)
{
#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif
}
#endif

void krhino_start_hook(void)
{
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    krhino_task_sched_stats_reset();
#endif
}

void krhino_task_create_hook(ktask_t *task)
{
#if XCHAL_CP_NUM > 0
    krhino_task_info_set(task, 0, (void *)((uint32_t)task->task_stack + XT_STK_FRMSZ));
#endif
}

void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
}

void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest)
{
    (void)orgin;
    (void)dest;
}

void krhino_tick_hook(void)
{
}

void krhino_task_abort_hook(ktask_t *task)
{
    (void)task;
}

void krhino_mm_alloc_hook(void *mem, size_t size)
{
    (void)mem;
    (void)size;
}

