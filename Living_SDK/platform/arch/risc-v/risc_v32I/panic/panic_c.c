/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_dbg_api.h>
#include <stdarg.h>
#include <stdlib.h>
#include "bl_uart.h"

#if (RHINO_CONFIG_BACKTRACE > 0)

/** WARNING: to use call trace facilities, enable
 *  compiler's frame pointer feature:
 *  -fno-omit-frame-pointer
 */

/* function call stack graph
 *
 *                                   low addr
 *                                  |___..___|
 *                               ---|___fp___|<--- previous fp
 * func_A stack start -->        |  |___lr___|<--- current fp
 *                               |  |        |
 *                               |  |   ..   |
 *                               |  |________|
 *                             --|--|___fp___|
 * func_B stack start -->      | -->|___lr___|
 *                             |    |        |
 *                             |    |        |
 *                             |    |___..___|
 *                           --|----|___fp___|
 * func_C stack start -->    |  --->|___lr___|
 *                           |      |        |
 *                           |       high addr
 *                           ------>
 *
 *                   func_C () {
 *                      ...
 *                      func_B();
 *                      ...
 *                   }
 *
 *                   func_B() {
 *                      ...
 *                      func_A();
 *                      ...
 *                   }
 */

#define VALID_PC_START_XIP (0x23000000)
#define VALID_FP_START_XIP (0x42000000)

static inline void backtrace_stack(int (*print_func)(const char *fmt, ...),
                            unsigned long *fp, int depth) {
  uintptr_t *pc;

  while (depth--) {
    if ((((uintptr_t)fp & 0xff000000ul) != VALID_PC_START_XIP) && (((uintptr_t)fp & 0xff000000ul) != VALID_FP_START_XIP)) {
      print_func("!!");
      return;
    }

    pc = (uintptr_t *)fp[-1];
    
    if ((((uintptr_t)pc & 0xff000000ul) != VALID_PC_START_XIP) && (((uintptr_t)pc & 0xff000000ul) != VALID_FP_START_XIP)) {
      print_func("!!");
      return;
    }
    
    if ((uintptr_t)pc > VALID_FP_START_XIP) {
      /* there is a function that does not saved ra,
             * skip!
             * this value is the next fp
             */
      fp = (unsigned long *)pc;
    } else if ((uintptr_t)pc > VALID_PC_START_XIP) {
      print_func(" %p", pc);
      fp = (unsigned long *)fp[-2];

      if (pc == (uintptr_t *)krhino_task_deathbed) {
        //print_func("");
        break;
      }
    }
  }
}

char __global_print_buf[64];

void printStr(const char *str) {
  int i = 0;

  while (str[i] != '\0') {
    bl_uart_data_send(0, str[i++]);
  }
}

void printAddr(uintptr_t addr) {
  itoa((unsigned int)addr, __global_print_buf, 16);
  printStr(__global_print_buf);
}

int printf_no_malloc(const char *s, ...) {
  int i = 0;
  va_list va_ptr;
  va_start(va_ptr, s);

  while (s[i] != '\0') {
    if (s[i] != '%') {
      bl_uart_data_send(0, s[i++]);
      continue;
    }

    switch (s[++i]) {
    case 'p':
      printAddr(va_arg(va_ptr, uintptr_t));
      break;
    case 's':
      printStr(va_arg(va_ptr, char *));
      break;
    default:
      break;
    }

    i++; // 下一个字符
  }

  /* 可变参最后一步 */
  va_end(va_ptr);
  bl_uart_flush(0);

  return 0;
}

int backtrace_now(int (*print_func)(const char *fmt, ...))
{
  static int processing_backtrace = 0;
  unsigned long *fp;
  CPSR_ALLOC();

  RHINO_CPU_INTRPT_DISABLE();

  if (processing_backtrace == 0) {
    processing_backtrace = 1;
  } else {
    print_func("backtrace nested...\r\n");
    return 0;
  }

#if defined(__GNUC__)
  __asm__("add %0, x0, fp"
          : "=r"(fp));
#else
#error "Compiler is not gcc!"
#endif

  print_func(">> ");
  backtrace_stack(print_func, fp, 256);
  print_func(" <<\r\n");

  processing_backtrace = 0;

  RHINO_CPU_INTRPT_ENABLE();
  
  return 0;
}

void backtrace_trap(int (*print_func)(const char *fmt, ...), uintptr_t *regs) {
  static int processing_backtrace_trap = 0;
  uintptr_t *fp;
  uintptr_t pc;

  /* in trap, MIE is disabled already */

  if (processing_backtrace_trap == 0) {
    processing_backtrace_trap = 1;
  } else {
    print_func("backtrace_trap nested...\r\n");
    while(1);
    return;
  }

  fp = (uintptr_t *)regs[6]; /* s0/fp */
  pc = regs[1];              /* mepc */

  print_func("=== backtrace_trap start ===\r\n");

  print_func("-f -p %p", pc);
  pc = regs[2]; /* ra */
  print_func("  %p", pc);

  while (1) {
    pc = fp[-1];

    if (pc > VALID_FP_START_XIP) {
      /* there is a function that does not saved ra,
             * skip!
             * this value is the next fp
             */
      fp = (uintptr_t *)pc;
    } else if ((uintptr_t)pc > VALID_PC_START_XIP) {
      print_func(" %p", pc);
      fp = (uintptr_t *)fp[-2];

      if (pc == (uintptr_t)krhino_task_deathbed) {
        print_func("\r\nbacktrace_trap: reached task deathbed!!!\r\n");
        break;
      }
    }
  }

  print_func("=== backtrace_trap end ===\r\n\r\n");
}

#endif
