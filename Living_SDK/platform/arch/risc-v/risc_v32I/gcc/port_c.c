/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "typedef.h"
#include "riscv_encoding.h"

extern int fpu_reg_count(void);


/* ARMCC || IAR || GNU */
#define FPU_AVL                                          \
    ((defined(__CC_ARM) && defined(__TARGET_FPU_VFP)) || \
     (defined(__ICCARM__) && defined(__ARMVFP__)) ||     \
     (defined(__GNUC__) && defined(__VFP_FP__) && !defined(__SOFTFP__)))


void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size, void *arg,
                          task_entry_t entry)
{
    cpu_stack_t *stk;
    //cpu_stack_t  task_addr;
    uint32_t     temp = (uint32_t)(stack_base + stack_size);

    /* stack aligned by 16 byte */
    temp &= 0xfffffff0;
    stk = (cpu_stack_t *)temp;

#if __riscv_float_abi_single
    /*     MEM HIGH (32 + 1 + 1)
in_stk->
           ^ padding
           | fcsr
           | f31         (ft11)
           | f30         (ft10)
           | f29         (ft9)
           | f28         (ft8)
           | f27         (fs11)
           | f26         (fs10)
           | f25         (fs9)
           | f24         (fs8)
           | f23         (fs7)
           | f22         (fs6)
           | f21         (fs5)
           | f20         (fs4)
           | f19         (fs3)
           | f18         (fs2)
           | f17         (fa7)
           | f16         (fa6)
           | f15         (fa5)
           | f14         (fa4)
           | f13         (fa3)
           | f12         (fa2)
           | f11         (fa1)
           | f10         (fa0)
           | f9          (fs1)
           | f8          (fs0)
           | f7          (ft7)
           | f6          (ft6)
           | f5          (ft5)
           | f4          (ft4)
           | f3          (ft3)
           | f2          (ft2)
           | f1          (ft1)
           | f0          (ft0)
           MEM LOW */
    *(stk - 2) = 0; /* fcsr: Round to Nearest, ties to Even */
    stk -= 34;
#endif

    /*     MEM HIGH ( 28 + 1 + 1)
         29^ x31         (t6)
         28| x30         (t5)
         27| x29         (t4)
         26| x28         (t3)
         25| x27         (s11)
         24| x26         (s10)
         23| x25         (s9)
         22| x24         (s8)
         21| x23         (s7)
         20| x22         (s6)
         19| x21         (s5)
         18| x20         (s4)
         17| x19         (s3)
         16| x18         (s2)
         15| x17         (a7)
         14| x16         (a6)
         13| x15         (a5)
         12| x14         (a4)
         11| x13         (a3)
         10| x12         (a2)
          9| x11         (a1)
          8| x10         (a0)
          7| x9          (s1)
          6| x8          (s0/fp)
          5| x7          (t2)
          4| x6          (t1)
          3| x5          (t0)
          2| x1          (ra)
          1| mepc
out_stk-> 0| mstatus
           MEM LOW
    */

    /* for debugging purpose */
    *(stk -  1) = (cpu_stack_t)0x31313131;           /* Reg x31, t6 */
    *(stk - 26) = (cpu_stack_t)0x06060606;           /* Reg  x6, t1 */
    *(stk - 27) = (cpu_stack_t)0x05050505;           /* Reg  x5, t0 */

    /* init values */
    *(stk - 22) = (cpu_stack_t)arg;                  /* Reg x10, a0 : argument */
    *(stk - 28) = (cpu_stack_t)krhino_task_deathbed; /* Reg  x1, ra */
    *(stk - 29) = (cpu_stack_t)entry;                /* epc */

    /* for mstatus, better to:
     * 1. read mstatus back
     * 2. mask some bits
     * 3. modify some bits
     * for those WPRI bits
     */
#if __riscv_float_abi_single
    *(stk - 30) = (cpu_stack_t)(RISCV_MSTATUS_FS_INIT | MSTATUS_MPP | MSTATUS_MPIE); /* mstatus SET FS(init), MIE and MPP */
#else
    *(stk - 30) = (cpu_stack_t)(MSTATUS_MPP | MSTATUS_MPIE); /* mstatus SET MIE and MPP */
#endif

    stk -= 30;

    return stk;
}
