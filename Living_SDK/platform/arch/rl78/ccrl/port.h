/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H

void     cpu_intrpt_switch(void);
void     cpu_first_task_start(void);
void    *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry);

RHINO_INLINE uint8_t cpu_cur_get(void)
{
    return 0;
}

#define cpu_task_switch()  __brk() 

#define cpu_intrpt_save()           __get_psw();__DI()
#define cpu_intrpt_restore(cpsr)    __set_psw(cpsr)

#define CPSR_ALLOC() uint8_t cpsr
#define RHINO_CPU_INTRPT_DISABLE() { cpsr = cpu_intrpt_save(); }
#define RHINO_CPU_INTRPT_ENABLE()  { cpu_intrpt_restore(cpsr); }

#endif /* PORT_H */

