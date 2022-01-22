#include <stdint.h>
#include <stdio.h>

#include <tg7100c.h>

#include <clic.h>
#include <blog.h>
#include "bl_irq.h"
#include <k_api.h>

#ifdef TG7100C_COREDUMP
#include <bl_coredump.h>
#endif

void bl_irq_enable(unsigned int source)
{
    *(volatile uint8_t*)(CLIC_HART0_ADDR + CLIC_INTIE + source) = 1;
}

void bl_irq_disable(unsigned int source)
{
    *(volatile uint8_t*)(CLIC_HART0_ADDR + CLIC_INTIE + source) = 0;
}

void bl_irq_pending_set(unsigned int source)
{
    *(volatile uint8_t*)(CLIC_HART0_ADDR + CLIC_INTIP + source) = 1;
}

void bl_irq_pending_clear(unsigned int source)
{
    *(volatile uint8_t*)(CLIC_HART0_ADDR + CLIC_INTIP + source) = 0;
}

void bl_irq_exception_trigger(BL_IRQ_EXCEPTION_TYPE_T type, void *ptr)
{
    uint32_t val = 0x12345678;;

    switch (type) {
        case BL_IRQ_EXCEPTION_TYPE_LOAD_MISALIGN:
        {
            val = *(uint32_t*)ptr;
        }
        break;
        case BL_IRQ_EXCEPTION_TYPE_STORE_MISALIGN:
        {
            *(uint32_t*)ptr = val;
        }
        break;
        case BL_IRQ_EXCEPTION_TYPE_ACCESS_ILLEGAL:
        {
            *(uint32_t*)ptr = val;
        }
        break;
        case BL_IRQ_EXCEPTION_TYPE_ILLEGAL_INSTRUCTION:
        {
            uint32_t fun_val = 0;
            typedef void (*ins_ptr_t)(void);
            ins_ptr_t func = (ins_ptr_t)&fun_val;

            func();
        }
        break;
        default:
        {
            /*nothing here*/
        }
    }
    printf("Trigger exception val is %08lx\r\n", val);
}

void bl_irq_default(void)
{
    while (1) {
        /*dead loop*/
    }
}

static void (*handler_list[2][16 + 64])(void) = {
    
};


static inline void _irq_num_check(int irqnum)
{
    if (irqnum < 0 || irqnum >= sizeof(handler_list[0])/sizeof(handler_list[0][0])) {
        blog_error("illegal irqnum %d\r\n", irqnum);
        while (1) {
            /*Deap loop here, TODO ass blog_assert*/
        }
    }
}

void bl_irq_register_with_ctx(int irqnum, void *handler, void *ctx)
{
    _irq_num_check(irqnum);
    if (handler_list[0][irqnum] && handler_list[0][irqnum] != handler) {
        blog_warn("IRQ %d already registered with %p \r\n",
             irqnum,
             handler_list[0][irqnum]
        );
    }
   
    if (handler == NULL) {
        blog_error("handler is NULL pointer! \r\n");
        return;
    }

    if (NULL == ctx) {
        handler_list[0][irqnum] = handler;
        handler_list[1][irqnum] = NULL;
    }
    else {
        handler_list[0][irqnum] = handler;
        handler_list[1][irqnum] = ctx;
    }

    return;
    
}

void bl_irq_ctx_get(int irqnum, void **ctx)
{
    _irq_num_check(irqnum);
    *ctx = handler_list[1][irqnum];

    return;
}

void bl_irq_register(int irqnum, void *handler)
{
    bl_irq_register_with_ctx(irqnum, handler, NULL);
}

void bl_irq_unregister(int irqnum, void *handler)
{
    _irq_num_check(irqnum);
    if (handler_list[0][irqnum] != handler) {
        blog_warn("IRQ %d:%p Not match with registered %p\r\n",
            irqnum,
            handler,
            handler_list[0][irqnum]
        );
    }
    handler_list[0][irqnum] = handler;
}

void interrupt_entry(uint32_t mcause, uint32_t mepc) 
{
    void *handler = NULL;
    mcause &= 0x7FFFFFF;
    if (mcause < sizeof(handler_list[0])/sizeof(handler_list[0][0])) {
        handler = handler_list[0][mcause];
    }
    if (handler) {
        if (handler_list[1][mcause]) {
           ((void (*)(void *))handler)(handler_list[1][mcause]);//handler(ctx)
        }
        else {
            ((void (*)(void))handler)();
        }
    } else {
        printf("mepc: %lx, %s\r\n", mepc, g_active_task[0]->task_name);
        printf("Cannot handle mcause 0x%lx:%lu, adjust to externel(0x%lx:%lu)\r\n",
                mcause,
                mcause,
                mcause - 16,
                mcause - 16
        );
        while (1) {
            /*dead loop now*/
        }
    }
}

void __dump_exception_code_str(uint32_t code)
{
    printf("Exception code: %lu\r\n", code);
    switch (code) {
        case 0x00:
        /*Instruction address misaligned*/
        {
            puts("  msg: Instruction address misaligned\r\n");
        }
        break;
        case 0x01:
        /*Instruction access fault*/
        {
            puts("  msg: Instruction access fault\r\n");
        }
        break;
        case 0x02:
        /*Illegal instruction*/
        {
            puts("  msg: Illegal instruction\r\n");
        }
        break;
        case 0x03:
        /*Breakpoint*/
        {
            puts("  msg: Breakpoint\r\n");
        }
        break;
        case 0x04:
        /*Load address misaligned*/
        {
            puts("  msg: Load address misaligned\r\n");
        }
        break;
        case 0x05:
        /*Load access fault*/
        {
            puts("  msg: Load access fault\r\n");
        }
        break;
        case 0x06:
        /*Store/AMO access misaligned*/
        {
            puts("  msg: Store/AMO access misaligned\r\n");
        }
        break;
        case 0x07:
        /*Store/AMO access fault*/
        {
            puts("  msg: Store/AMO access fault\r\n");
        }
        break;
        case 0x08:
        /*Environment call from U-mode*/
        {
            puts("  msg: Environment call from U-mode\r\n");
        }
        break;
        case 0x09:
        /*Environment call from S-mode*/
        {
            puts("  msg: Environment call from S-mode\r\n");
        }
        break;
        case 0x0a:
        case 0x0e:
        /*Reserved*/
        {
            puts("  msg: Reserved\r\n");
        }
        break;
        case 0x0b:
        /*Environment call from M-mode*/
        {
            puts("  msg: Environment call from M-mode\r\n");
        }
        break;
        case 0x0c:
        /*Instruction page fault*/
        {
            puts("  msg: Instruction page fault\r\n");
        }
        break;
        case 0x0d:
        /*Load page fault*/
        {
            puts("  msg: Load page fault\r\n");
        }
        break;
        case 0x0f:
        /*Store/AMO page fault*/
        {
            puts("  msg: Store/AMO page fault\r\n");
        }
        break;
        default:{
            puts("  msg: Reserved default exception\r\n");
        }
    }
}

extern void misaligned_load_trap(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc);
extern void misaligned_store_trap(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc);

#define EXCPT_LOAD_MISALIGNED        4
#define EXCPT_STORE_MISALIGNED       6
#define MCAUSE_INT         0x80000000

#define VALID_PC_START_XIP (0x23000000)
#define VALID_FP_START_XIP (0x42000000)

extern void backtrace_trap(int (*print_func)(const char *fmt, ...), uintptr_t *regs);

static void check_valid_addr(uintptr_t addr, uintptr_t *regs) {
  addr &= 0xff000000ul;
  
  if ((addr != VALID_PC_START_XIP) && (addr != VALID_FP_START_XIP)) {
      printf("exception invalid addr = 0x%08x.\r\n", addr);
      backtrace_trap(printf, regs);
      while(1);
  }

  return;
}

void check_mie_is_on(void)
{
    uint32_t mstatus = read_csr(mstatus);
    if (mstatus & MSTATUS_MIE) {
        printf("check_mie_is_on\r\n");
        printf("%d %d\r\n", (int)read_csr(mepc), (int)mstatus);
        __asm volatile ("csrci mstatus, 8":::"memory");// di
        //__asm volatile ("csrci mstatus, MSTATUS_MIE");
        while(1);
    }
}
void exception_entry(uint32_t mcause, uint32_t mepc, uint32_t mtval, uintptr_t *regs)
{
    if ((mcause & 0x3ff) == EXCPT_LOAD_MISALIGNED){
        uintptr_t addr = read_csr(mbadaddr);
        check_valid_addr(addr, regs);
        misaligned_load_trap(regs, mcause, mepc);
    } else if ((mcause & 0x3ff) == EXCPT_STORE_MISALIGNED){
        uintptr_t addr = read_csr(mbadaddr);
        check_valid_addr(addr, regs);
        misaligned_store_trap(regs, mcause, mepc);
    } else {
        puts("Exception Entry--->>>\r\n");
        printf("mcause %08lx, mepc %08lx, mtval %08lx\r\n",
            mcause,
            mepc,
            mtval
        );

        backtrace_trap(printf, regs);
        //__dump_exception_code_str(mcause & 0xFFFF);
        //backtrace_now((int (*)(const char *fmt, ...))printf, regs);
        while (1) {
            /*Deap loop now*/
#ifdef TG7100C_COREDUMP
    bl_coredump_run();
#endif
        }
    }
}

uintptr_t handle_trap_ali(uintptr_t mcause, uintptr_t mepc, uintptr_t* regs)
{
    uint32_t mtval;

    if (mcause & MCAUSE_INT) {
        interrupt_entry(mcause, mepc); 
        //check_mie_is_on();
    } else {
        /* exception */
        mtval = read_csr(mtval);
        exception_entry(mcause, mepc, mtval, regs);
    }

    return mepc;
}

void bl_irq_init(void)
{
    uint32_t ptr;

    /*clear mask*/
    for (ptr = 0x02800400; ptr < 0x02800400 + 128; ptr++) {
        *(uint8_t*)ptr = 0;
    }
    /*clear pending*/
    for (ptr = 0x02800000; ptr < 0x02800000 + 128; ptr++) {
        *(uint8_t*)ptr = 0;
    }
}
