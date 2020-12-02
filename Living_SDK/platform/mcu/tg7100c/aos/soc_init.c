#include "k_api.h"
#include "hal/soc/uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <riscv_encoding.h>

#include <bl_uart.h>
#include <bl_irq.h>
#include "bflb_platform.h"
#include "clic.h"

#define RTC_FREQ   (10*1000*1000UL)

uart_dev_t   uart_0;
uart_dev_t   uart_1;

#define OS_WAIT_FOREVER		0xffffffffU

#define HAL_WAIT_FOREVER    OS_WAIT_FOREVER

extern uint32_t g_debug_uart_port ;

#if defined (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */

void _putc(char c)// used ?
{
    bl_uart_data_send(0, c);
}

void system_timer_start(void)
{
    *((uint64_t*)(CLIC_CTRL_ADDR + CLIC_MTIMECMP)) =
        (*((uint64_t*)(CLIC_CTRL_ADDR + CLIC_MTIME))) + 1*RTC_FREQ/RHINO_CONFIG_TICKS_PER_SECOND;

    bl_irq_enable(7);
}

void system_timer_update(void)
{
    *((uint64_t*) (CLIC_CTRL_ADDR + CLIC_MTIMECMP)) += 1*RTC_FREQ/RHINO_CONFIG_TICKS_PER_SECOND;
}

/*Entry Point for Machine Timer Interrupt Handler*/
/*called from bsp/env/ventry.s          */
void handle_m_time_interrupt(void)
{
    clear_csr(mie, MIP_MTIP);
    
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
    system_timer_update();

    set_csr(mie, MIP_MTIP);
}

int putchar(int ch)
{
    bl_uart_data_send(0, ch);
    return ch;
}

PUTCHAR_PROTOTYPE
{
    bl_uart_data_send(0, ch);
    return ch;
}

GETCHAR_PROTOTYPE
{
    /* Place your implementation of fgetc here */
    /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
    uint8_t ch = -1; //EOF;
    int32_t ret = -1;

    uint32_t recv_size;
    ret = hal_uart_recv_II(&uart_0, &ch, 1, &recv_size, HAL_WAIT_FOREVER);

    if (ret == 0) {
        return ch;
    } else {
        return -1;
    }
}

static int __dbg_fputc (const char c, void *fil)
{
    bl_uart_data_send(1, c);
    return 1;
}

static int __dbg_fputs (const char *s, void *fil)
{
    const char *ss;
    char        c;

    for (ss = s; (c = *ss) != '\0'; ss++) {
        (void)__dbg_fputc(c, fil);
    }

    return (ss - s);
}

void bl_hw_console_output(const char *s)
{
    __dbg_fputs(s, NULL);
}

void uart0_rbuf_init(uint32_t baud_rate)
{
    uint8_t pins[] = {16, 7}; // {tx_pin, rx_pin}

    uart_0.port = 0;
    uart_0.config.baud_rate = baud_rate;
    uart_0.priv = pins;
    hal_uart_init(&uart_0);

    uint8_t my_pins[] = {21, 12};
    uart_1.port = 1;
    uart_1.config.baud_rate = 921600;
    uart_1.priv = my_pins;

    hal_uart_init(&uart_1);
    
}

