/**
 ****************************************************************************************
 *
 * @file gnuarm/compiler.h
 *
 * @brief Definitions of compiler specific directives.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

/// define the force inlining attribute for this compiler  gcc: __attribute__((always_inline))
#define __INLINE                     static inline

/// function returns struct in registers (4 words max, var with gnuarm)
#if !defined(__VIR)
#define __VIR                           __value_in_regs
#endif

/// function has no side effect and return depends only on arguments
#if !defined(__PURE)
#define __PURE                         __pure
#endif
 
/// Align instantiated lvalue or struct member on 4 bytes
#if !defined(__ALIGN4)
#define __ALIGN4                     __attribute__((aligned(4)))
#endif

/// Pack a structure field
#define __PACKED16                __attribute__( ( packed ) )
#if !defined(__PACKED)
#define __PACKED                  __attribute__( ( packed ) )
#endif

#endif // _COMPILER_H_

