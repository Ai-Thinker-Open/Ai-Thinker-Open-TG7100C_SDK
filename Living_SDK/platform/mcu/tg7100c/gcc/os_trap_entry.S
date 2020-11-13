// See LICENSE for license details

#ifndef OS_TRAP_ENTRY_S
#define OS_TRAP_ENTRY_S

#include "riscv_encoding.h"
#include "typedef.h"

.extern restore_sp
.extern check_mie_is_on

.global except_before_task_running
.global except_from_intrpt

  .section      .text.entry	
  .align 6
  .weak trap_entry
  .global trap_entry
trap_entry:

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

  STORE x1,   2*REGBYTES(sp)
  STORE x5,   3*REGBYTES(sp)
  STORE x6,   4*REGBYTES(sp)
  STORE x7,   5*REGBYTES(sp)
  STORE x8,   6*REGBYTES(sp)
  STORE x9,   7*REGBYTES(sp)
  STORE x10,  8*REGBYTES(sp)
  STORE x11,  9*REGBYTES(sp)
  STORE x12, 10*REGBYTES(sp)
  STORE x13, 11*REGBYTES(sp)
  STORE x14, 12*REGBYTES(sp)
  STORE x15, 13*REGBYTES(sp)
  STORE x16, 14*REGBYTES(sp)
  STORE x17, 15*REGBYTES(sp)
  STORE x18, 16*REGBYTES(sp)
  STORE x19, 17*REGBYTES(sp)
  STORE x20, 18*REGBYTES(sp)
  STORE x21, 19*REGBYTES(sp)
  STORE x22, 20*REGBYTES(sp)
  STORE x23, 21*REGBYTES(sp)
  STORE x24, 22*REGBYTES(sp)
  STORE x25, 23*REGBYTES(sp)
  STORE x26, 24*REGBYTES(sp)
  STORE x27, 25*REGBYTES(sp)
  STORE x28, 26*REGBYTES(sp)
  STORE x29, 27*REGBYTES(sp)
  STORE x30, 28*REGBYTES(sp)
  STORE x31, 29*REGBYTES(sp)

  csrr  t1, mstatus
  STORE t1, 0*REGBYTES(sp)
  csrr a1, mepc
  STORE a1, 1*REGBYTES(sp)

  la      t1, g_sys_stat
  LOAD    t2, (t1)
  li      t3, 3
  bne     t2, t3, except_before_task_running                 // RHINO_RUNNING = 3


  call    krhino_intrpt_enter                               // g_intrpt_nested_level++;

  la      t1, g_intrpt_nested_level
  lb      t2, (t1)
  li      t3, 1
  bgt     t2, t3, except_from_intrpt                         // if (g_intrpt_nested_level > 0)

except_from_task:
  // g_active_task->task_stack = context region
  la      t1, g_active_task                                 // g_active_task->task_stack = SP;
  LOAD    t2, (t1)
  STORE   sp, (t2)

  mv a2, sp
  la   sp, __freertos_irq_stack_top                         // Switch to except stack.

  csrr a0, mcause
  csrr a1, mepc
  call handle_trap_ali
  csrw mepc, a0

  call krhino_intrpt_exit
  call check_mie_is_on

  la    t1, g_active_task
  LOAD  t2, (t1)
  LOAD  sp, (t2)

  j   restore_sp


except_from_intrpt:
  csrr a0, mcause
  csrr a1, mepc
  mv a2, sp
  call handle_trap_ali
  csrw mepc, a0

  la     t1,g_intrpt_nested_level                           // g_intrpt_nested_level--;
  lb     t2, (t1)
  add      t2, t2, -1
  STORE    t2, (t1)

  j   restore_sp


except_before_task_running:
  csrr a0, mcause
  csrr a1, mepc
  mv a2, sp
  call handle_trap_ali
  csrw mepc, a0

  j   restore_sp


.weak handle_trap_ali
handle_trap_ali:
1:
  j 1b
	
#endif
