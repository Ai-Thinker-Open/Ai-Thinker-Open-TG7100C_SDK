#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <riscv_encoding.h>
#include "clic.h"
#include "tg7100c_glb.h"
#include "bflb_platform.h"
#include "platform_device.h"
#include "tg7100c_common.h"

#include "emulation.h"

#define MCAUSE_INT         0x80000000
#define MCAUSE_CAUSE       0x000003FFUL
#define IRQ_M_TIMER        7
#define IRQ_DBG_UART       0x2d

#define EXCPT_LOAD_MISALIGNED        4
#define EXCPT_STORE_MISALIGNED       6

extern int main(int argc, char** argv);
extern void trap_entry();
extern int hal_interpt_init(void);

static unsigned long mtime_lo(void)
{
  return *(volatile unsigned long *)(CLIC_CTRL_ADDR + CLIC_MTIME);
}

#ifdef __riscv32

static uint32_t mtime_hi(void)
{
  return *(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME + 4);
}

uint64_t get_timer_value()
{
  while (1) {
    uint32_t hi = mtime_hi();
    uint32_t lo = mtime_lo();
    if (hi == mtime_hi())
      return ((uint64_t)hi << 32) | lo;
  }
}

#else /* __riscv32 */

uint64_t get_timer_value()
{
    return mtime_lo();
}

#endif

unsigned long get_timer_freq()
{
    return 160 * 1000 * 1000UL;
}

unsigned long get_cpu_freq()
{
    static uint32_t cpu_freq;

    return 160 * 1000 * 1000UL;
}

void uart_init(size_t baud_rate)
{

}

#ifdef USE_M_TIME
extern void handle_m_time_interrupt();
#endif

/* TODO: formalize IRQ handling */
uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc, uintptr_t* regs)
{
    if (mcause & MCAUSE_INT) {
        if ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER){
            handle_m_time_interrupt();
        } else if ((mcause & MCAUSE_CAUSE) == IRQ_DBG_UART){
            UART_IntHandler(UART0_ID);
        } else {
            __asm volatile ("ebreak");
            while(1);
        }
    } else {
        /* exception */
        if ((mcause & MCAUSE_CAUSE) == EXCPT_LOAD_MISALIGNED){
            misaligned_load_trap(regs, mcause, epc);
        } else if ((mcause & MCAUSE_CAUSE) == EXCPT_STORE_MISALIGNED){
            misaligned_store_trap(regs, mcause, epc);
        } else {
            __asm volatile ("ebreak");
            Default_Handler();
            while(1);
        }
    }

    return epc;
}

void _init()
{
    /* disable mtimer irq */
    clear_csr(mie, MIP_MTIP);
    write_csr(mtvec, (int)(trap_entry) | 0x02); /* CLIC direct mode */
}

void _fini()
{
}
