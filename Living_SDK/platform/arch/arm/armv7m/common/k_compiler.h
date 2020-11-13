/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_COMPILER_H
#define K_COMPILER_H

#if defined(__CC_ARM)
#define RHINO_INLINE                static __inline
/* get the return address of the current function
   unsigned int __return_address(void) */
#define RHINO_GET_RA()              (void *)__return_address()
/* get the  the value of the stack pointer
   unsigned int __current_sp(void) */
#define RHINO_GET_SP()              (void *)__current_sp()
/* get the number of leading 0-bits in x
   unsigned char __clz(unsigned int val) */
#define RHINO_BIT_CLZ(x)            __clz(x)

#ifndef __WEAK
#define __WEAK                      __weak
#endif

/* Instruction Synchronization Barrier */
#define OS_ISB()    __isb(15)   /* Full system  Any-Any */

/* Data Memory Barrier */
#define OS_DMB()    __dmb(15)   /* Full system  Any-Any */

/* Data Synchronization Barrier */
#define OS_DSB()    __dsb(15)   /* Full system  Any-Any */

#elif defined(__ICCARM__)
#define RHINO_INLINE                static inline
/* get the return address of the current function
   unsigned int __get_LR(void) */
#define RHINO_GET_RA()              (void *)__get_LR()
/* get the  the value of the stack pointer
   unsigned int __get_SP(void) */
#define RHINO_GET_SP()              (void *)__get_SP()
/* get the value of the stack pointer register
   unsigned int __CLZ(unsigned int) */
//#define RHINO_BIT_CLZ(x)            __CLZ(x)

#ifndef __WEAK
#define __WEAK                      __weak
#endif

/* Instruction Synchronization Barrier */
#define OS_ISB()    __isb(15)   /* Full system  Any-Any */

/* Data Memory Barrier */
#define OS_DMB()    __dmb(15)   /* Full system  Any-Any */

/* Data Synchronization Barrier */
#define OS_DSB()    __dsb(15)   /* Full system  Any-Any */

#elif defined(__GNUC__)
#define RHINO_INLINE                static inline
/* get the return address of the current function
   void * __builtin_return_address (unsigned int level) */
#define RHINO_GET_RA()              __builtin_return_address(0)
/* get the return address of the current function */
__attribute__((always_inline)) RHINO_INLINE void *RHINO_GET_SP(void)
{
    void *sp;
    asm volatile("mov %0, SP\n":"=r"(sp));
    return sp;
}
__attribute__((always_inline)) RHINO_INLINE unsigned char RHINO_BIT_CLZ_(unsigned int bitmap)
{
    unsigned char cnt;
    asm volatile("clz %0, %1":"=r"(cnt):"r"(bitmap));
    return cnt;
}
/* get the number of leading 0-bits in x
   int __builtin_clz (unsigned int x) */
#define RHINO_BIT_CLZ(x)            RHINO_BIT_CLZ_(x)

#ifndef __WEAK
#define __WEAK                      __attribute__((weak))
#endif

/* Instruction Synchronization Barrier */
#define OS_ISB() \
    __asm volatile ("isb sy":::"memory")

/* Data Memory Barrier */
#define OS_DMB() \
    __asm volatile ("dmb sy":::"memory")

/* Data Synchronization Barrier */
#define OS_DSB() \
    __asm volatile ("dsb sy":::"memory")

#else
#error "Unsupported compiler"
#endif

#endif /* K_COMPILER_H */

