/*
*
* @file port_s.S
*
*/

#include "typedef.h"
#include "riscv_encoding.h"

/* TODO:
 * 1. handle fp registers ONLY when needed
 * 2. use macro
 */

/******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************/

.extern  g_active_task
.extern  g_preferred_ready_task
.extern  krhino_stack_ovf_check
.extern check_mie_is_on

/******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************/

.global  cpu_intrpt_save
.global  cpu_intrpt_restore
.global  cpu_task_switch
.global  cpu_intrpt_switch
.global  cpu_first_task_start
.global  restore_sp


/******************************************************************************
@                                 EQUATES
@******************************************************************************/

//.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/
//.equ RISCV_MSTATUS_MPIE,       (1<<7)       /*machine-level pre interrupt bit*/
//.equ RISCV_MSTATUS_MPP,        (0x3<<11)    /*machine-level MPP bit*/

//.equ LOG_REGBYTES,              2    
//.equ REGBYTES,                 (1<<LOG_REGBYTES)    /*machine-level MPP bit*/

/******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************/

.text
.align 2

/******************************************************************************
@                        MACRO DEFINED
@******************************************************************************/

.macro POP_FP_REG reg


.endm

.macro PUSH_FP_REG reg


.endm


/******************************************************************************
@ Functions:
@     size_t cpu_intrpt_save(void);
@     void cpu_intrpt_restore(size_t cpsr);
@******************************************************************************/
cpu_intrpt_save:
    csrrci a0, mstatus, RISCV_MSTATUS_MIE
    ret


cpu_intrpt_restore:
    csrw mstatus, a0
    ret


/******************************************************************************
@ Functions:
@     void   cpu_first_task_start(void);
@******************************************************************************/
cpu_first_task_start:
    la    t1,  g_active_task
    lw    t2, (t1)
    lw    sp, (t2)

    lw    t1, (sp)
    csrw  mstatus, t1
    lw    t1, 1*REGBYTES(sp)
    csrw  mepc, t1
    lw     x1,  2*REGBYTES(sp)
    lw     x5,  3*REGBYTES(sp)
    lw     x6,  4*REGBYTES(sp)
    lw     x7,  5*REGBYTES(sp)
    lw     x8,  6*REGBYTES(sp)
    lw     x9,  7*REGBYTES(sp)
    lw    x10,  8*REGBYTES(sp)
    lw    x11,  9*REGBYTES(sp)
    lw    x12, 10*REGBYTES(sp)
    lw    x13, 11*REGBYTES(sp)
    lw    x14, 12*REGBYTES(sp)
    lw    x15, 13*REGBYTES(sp)
    lw    x16, 14*REGBYTES(sp)
    lw    x17, 15*REGBYTES(sp)
    lw    x18, 16*REGBYTES(sp)
    lw    x19, 17*REGBYTES(sp)
    lw    x20, 18*REGBYTES(sp)
    lw    x21, 19*REGBYTES(sp)
    lw    x22, 20*REGBYTES(sp)
    lw    x23, 21*REGBYTES(sp)
    lw    x24, 22*REGBYTES(sp)
    lw    x25, 23*REGBYTES(sp)
    lw    x26, 24*REGBYTES(sp)
    lw    x27, 25*REGBYTES(sp)
    lw    x28, 26*REGBYTES(sp)
    lw    x29, 27*REGBYTES(sp)
    lw    x30, 28*REGBYTES(sp)
    lw    x31, 29*REGBYTES(sp)

    addi  sp, sp, 30*REGBYTES

#if __riscv_float_abi_single
    flw   f0,   0*REGBYTES(sp)
    flw   f1,   1*REGBYTES(sp)
    flw   f2,   2*REGBYTES(sp)
    flw   f3,   3*REGBYTES(sp)
    flw   f4,   4*REGBYTES(sp)
    flw   f5,   5*REGBYTES(sp)
    flw   f6,   6*REGBYTES(sp)
    flw   f7,   7*REGBYTES(sp)
    flw   f8,   8*REGBYTES(sp)
    flw   f9,   9*REGBYTES(sp)
    flw   f10, 10*REGBYTES(sp)
    flw   f11, 11*REGBYTES(sp)
    flw   f12, 12*REGBYTES(sp)
    flw   f13, 13*REGBYTES(sp)
    flw   f14, 14*REGBYTES(sp)
    flw   f15, 15*REGBYTES(sp)
    flw   f16, 16*REGBYTES(sp)
    flw   f17, 17*REGBYTES(sp)
    flw   f18, 18*REGBYTES(sp)
    flw   f19, 19*REGBYTES(sp)
    flw   f20, 20*REGBYTES(sp)
    flw   f21, 21*REGBYTES(sp)
    flw   f22, 22*REGBYTES(sp)
    flw   f23, 23*REGBYTES(sp)
    flw   f24, 24*REGBYTES(sp)
    flw   f25, 25*REGBYTES(sp)
    flw   f26, 26*REGBYTES(sp)
    flw   f27, 27*REGBYTES(sp)
    flw   f28, 28*REGBYTES(sp)
    flw   f29, 29*REGBYTES(sp)
    flw   f30, 30*REGBYTES(sp)
    flw   f31, 31*REGBYTES(sp)

    csrw  mscratch, t1
    lw    t1,  32*REGBYTES(sp)
    fscsr t1
    csrr t1, mscratch

    addi  sp, sp, 34*REGBYTES
#endif

    mret


/******************************************************************************
@ Functions:
@     void cpu_task_switch(void);
@******************************************************************************/
cpu_task_switch:
    // save current task context:

#if __riscv_float_abi_single
    addi  sp, sp, -34*REGBYTES

    fsw   f0,   0*REGBYTES(sp)
    fsw   f1,   1*REGBYTES(sp)
    fsw   f2,   2*REGBYTES(sp)
    fsw   f3,   3*REGBYTES(sp)
    fsw   f4,   4*REGBYTES(sp)
    fsw   f5,   5*REGBYTES(sp)
    fsw   f6,   6*REGBYTES(sp)
    fsw   f7,   7*REGBYTES(sp)
    fsw   f8,   8*REGBYTES(sp)
    fsw   f9,   9*REGBYTES(sp)
    fsw   f10, 10*REGBYTES(sp)
    fsw   f11, 11*REGBYTES(sp)
    fsw   f12, 12*REGBYTES(sp)
    fsw   f13, 13*REGBYTES(sp)
    fsw   f14, 14*REGBYTES(sp)
    fsw   f15, 15*REGBYTES(sp)
    fsw   f16, 16*REGBYTES(sp)
    fsw   f17, 17*REGBYTES(sp)
    fsw   f18, 18*REGBYTES(sp)
    fsw   f19, 19*REGBYTES(sp)
    fsw   f20, 20*REGBYTES(sp)
    fsw   f21, 21*REGBYTES(sp)
    fsw   f22, 22*REGBYTES(sp)
    fsw   f23, 23*REGBYTES(sp)
    fsw   f24, 24*REGBYTES(sp)
    fsw   f25, 25*REGBYTES(sp)
    fsw   f26, 26*REGBYTES(sp)
    fsw   f27, 27*REGBYTES(sp)
    fsw   f28, 28*REGBYTES(sp)
    fsw   f29, 29*REGBYTES(sp)
    fsw   f30, 30*REGBYTES(sp)
    fsw   f31, 31*REGBYTES(sp)

    csrw  mscratch, t1
    frcsr t1
    sw    t1,  32*REGBYTES(sp)
    csrr  t1, mscratch
#endif

    addi sp, sp, -30*REGBYTES

    sw   x1, 1*REGBYTES(sp)
    sw   x1, 2*REGBYTES(sp)
    sw   x5, 3*REGBYTES(sp)
    sw   x6, 4*REGBYTES(sp)
    sw   x7, 5*REGBYTES(sp)

    li   t0, MSTATUS_MPIE
 /* csrrci t1, mstatus, MSTATUS_MIE */
    csrr t1, mstatus
    /*MPIE is set to 1 after last mret*/
    andi t2, t1, MSTATUS_MIE
    bnez t2, store_mstatus
    /* mask MPIE since MIE==0 */
    not  t0, t0
    and  t1, t1, t0
store_mstatus:
    sw   t1, (sp)     /*mstatus*/
    
    sw   x8,   6*REGBYTES(sp)
    sw   x9,   7*REGBYTES(sp)
    sw   x10,  8*REGBYTES(sp)
    sw   x11,  9*REGBYTES(sp)
    sw   x12, 10*REGBYTES(sp)
    sw   x13, 11*REGBYTES(sp)
    sw   x14, 12*REGBYTES(sp)
    sw   x15, 13*REGBYTES(sp)
    sw   x16, 14*REGBYTES(sp)
    sw   x17, 15*REGBYTES(sp)
    sw   x18, 16*REGBYTES(sp)
    sw   x19, 17*REGBYTES(sp)
    sw   x20, 18*REGBYTES(sp)
    sw   x21, 19*REGBYTES(sp)
    sw   x22, 20*REGBYTES(sp)
    sw   x23, 21*REGBYTES(sp)
    sw   x24, 22*REGBYTES(sp)
    sw   x25, 23*REGBYTES(sp)
    sw   x26, 24*REGBYTES(sp)
    sw   x27, 25*REGBYTES(sp)
    sw   x28, 26*REGBYTES(sp)
    sw   x29, 27*REGBYTES(sp)
    sw   x30, 28*REGBYTES(sp)
    sw   x31, 29*REGBYTES(sp)

    la    t1,  g_active_task
    lw    t1, (t1)
    sw    sp, (t1) 
        
    call    krhino_stack_ovf_check

    la    t0, g_active_task                         // g_active_task = g_preferred_ready_task;
    la    t1, g_preferred_ready_task
    lw    t2, (t1)
    sw    t2, (t0)
    
    lw    sp, (t2)
    
restore_sp:
    lw    t1, (sp)
    csrw  mstatus, t1
    lw    t1,   1*REGBYTES(sp)
    csrw  mepc, t1
    lw    ra,   2*REGBYTES(sp)
    lw    x5,   3*REGBYTES(sp)
    lw    x6,   4*REGBYTES(sp)
    lw    x7,   5*REGBYTES(sp)
    lw    x8,   6*REGBYTES(sp)
    lw    x9,   7*REGBYTES(sp)
    lw    x10,  8*REGBYTES(sp)
    lw    x11,  9*REGBYTES(sp)
    lw    x12, 10*REGBYTES(sp)
    lw    x13, 11*REGBYTES(sp)
    lw    x14, 12*REGBYTES(sp)
    lw    x15, 13*REGBYTES(sp)
    lw    x16, 14*REGBYTES(sp)
    lw    x17, 15*REGBYTES(sp)
    lw    x18, 16*REGBYTES(sp)
    lw    x19, 17*REGBYTES(sp)
    lw    x20, 18*REGBYTES(sp)
    lw    x21, 19*REGBYTES(sp)
    lw    x22, 20*REGBYTES(sp)
    lw    x23, 21*REGBYTES(sp)
    lw    x24, 22*REGBYTES(sp)
    lw    x25, 23*REGBYTES(sp)
    lw    x26, 24*REGBYTES(sp)
    lw    x27, 25*REGBYTES(sp)
    lw    x28, 26*REGBYTES(sp)
    lw    x29, 27*REGBYTES(sp)
    lw    x30, 28*REGBYTES(sp)
    lw    x31, 29*REGBYTES(sp)

    addi  sp, sp, 30*REGBYTES

#if __riscv_float_abi_single
    flw   f0,   0*REGBYTES(sp)
    flw   f1,   1*REGBYTES(sp)
    flw   f2,   2*REGBYTES(sp)
    flw   f3,   3*REGBYTES(sp)
    flw   f4,   4*REGBYTES(sp)
    flw   f5,   5*REGBYTES(sp)
    flw   f6,   6*REGBYTES(sp)
    flw   f7,   7*REGBYTES(sp)
    flw   f8,   8*REGBYTES(sp)
    flw   f9,   9*REGBYTES(sp)
    flw   f10, 10*REGBYTES(sp)
    flw   f11, 11*REGBYTES(sp)
    flw   f12, 12*REGBYTES(sp)
    flw   f13, 13*REGBYTES(sp)
    flw   f14, 14*REGBYTES(sp)
    flw   f15, 15*REGBYTES(sp)
    flw   f16, 16*REGBYTES(sp)
    flw   f17, 17*REGBYTES(sp)
    flw   f18, 18*REGBYTES(sp)
    flw   f19, 19*REGBYTES(sp)
    flw   f20, 20*REGBYTES(sp)
    flw   f21, 21*REGBYTES(sp)
    flw   f22, 22*REGBYTES(sp)
    flw   f23, 23*REGBYTES(sp)
    flw   f24, 24*REGBYTES(sp)
    flw   f25, 25*REGBYTES(sp)
    flw   f26, 26*REGBYTES(sp)
    flw   f27, 27*REGBYTES(sp)
    flw   f28, 28*REGBYTES(sp)
    flw   f29, 29*REGBYTES(sp)
    flw   f30, 30*REGBYTES(sp)
    flw   f31, 31*REGBYTES(sp)

    csrw  mscratch, t1
    lw    t1,  32*REGBYTES(sp)
    fscsr t1
    csrr t1, mscratch

    addi  sp, sp, 34*REGBYTES
#endif

    mret

/******************************************************************************
@ Functions:
@     void   cpu_intrpt_switch(void);
@******************************************************************************/
cpu_intrpt_switch:
    call check_mie_is_on
    la     t1, g_active_task                                  // g_active_task = g_preferred_ready_task;
    la     t2, g_preferred_ready_task
    lw     t3, (t2)
    sw     t3, (t1)                                             // R0 = g_active_task->task_stack = context region

    lw     sp, (t3)

    j   restore_sp

