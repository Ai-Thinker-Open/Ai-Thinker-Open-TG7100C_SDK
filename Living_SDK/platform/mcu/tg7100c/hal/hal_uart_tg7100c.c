/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "hal/soc/uart.h"
#include <stdarg.h>
#include <stdio.h>
#include "bl_uart.h"
#include <utils_log.h>
#include <k_api.h>
#include <tg7100c_uart.h>
#include <app_uart.h>
#include <bl_timer.h>

typedef struct {
    uint8_t tx_pin;
    uint8_t rx_pin;
} uart_pin_t;

static const uint32_t uart_base[2] = {UART0_BASE, UART1_BASE};
static const int uart_irq[2] = {UART0_IRQn, UART1_IRQn};

static void __uart_irq(void *p_arg)
{
    uint32_t tmpVal = 0;
    uint32_t maskVal = 0;
    uart_dev_t *p_uart = (uart_dev_t *)p_arg;
    uint32_t UARTx = uart_base[p_uart->port];
    uint8_t ch;

    tmpVal = BL_RD_REG(UARTx,UART_INT_STS);
    maskVal = BL_RD_REG(UARTx,UART_INT_MASK);

    if(BL_IS_REG_BIT_SET(tmpVal,UART_URX_FIFO_INT) && !BL_IS_REG_BIT_SET(maskVal,UART_URX_FIFO_INT)){
        while (UART_GetRxFifoCount(p_uart->port) > 0) {
            ch  = BL_RD_BYTE(UARTx + UART_FIFO_RDATA_OFFSET);
            uart_push_to_recv_buff(p_uart->port, &ch, 1);
        }
    }

    /* Length of uart rx data transfer arrived interrupt */
    if(BL_IS_REG_BIT_SET(tmpVal,UART_URX_END_INT) && !BL_IS_REG_BIT_SET(maskVal,UART_CR_URX_END_MASK)){
        while (UART_GetRxFifoCount(p_uart->port) > 0) {
            ch  = BL_RD_BYTE(UARTx + UART_FIFO_RDATA_OFFSET);
            uart_push_to_recv_buff(p_uart->port, &ch, 1);
        }
        if (UART_GetRxFifoCount(p_uart->port) == 0) {
            BL_WR_REG(UARTx,UART_INT_CLEAR,0x2);
        }
    }
}

int32_t hal_uart_init(uart_dev_t* uart)
{
    uart_pin_t *pins = (uart_pin_t *)uart->priv;

    if (pins == NULL) {
        return -1;
    }

    bl_uart_init(uart->port, pins->tx_pin, pins->rx_pin, 255, 255, uart->config.baud_rate);
    uart_recv_buff_all_init();
    bl_irq_register_with_ctx(uart_irq[uart->port], __uart_irq, uart);
    bl_irq_enable(uart_irq[uart->port]);
    UART_IntMask((UART_ID_Type)uart->port, UART_INT_ALL, MASK);
    UART_IntMask((UART_ID_Type)uart->port, UART_INT_RX_FIFO_REQ, UNMASK);
    UART_IntMask((UART_ID_Type)uart->port, UART_INT_RX_END, UNMASK);
    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    if (NULL == uart) {
        return -1;
    }

    bl_irq_disable(uart_irq[uart->port]);
    // todo
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    const uint8_t *ptr = data;
    int i = 0;

    if ((NULL == uart) || (NULL == ptr)) {
        printf("arg err!!!\r\n");
        return -1;
    }

    while (i < size) {
        bl_uart_data_send(uart->port, ptr[i]);
        i++;
    }

    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    uint32_t len = 0;

    if ((NULL == uart) || (NULL == data)) {
        printf("arg err!!!\r\n");
        return -1;
    }

    len = uart_how_many_valid_data_recv_buf(uart->port);
    len = (len > expect_size)?expect_size:len;

    uart_pop_from_recv_buff(uart->port, data, len);

    return len > 0 ? 0 : -1;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    int64_t startms = 0, endms = 0;
    uint32_t real_size = 0;
#if 0
    uint32_t len = 0;

    if ((NULL == uart) || (NULL == data)) {
        printf("arg err!!!\r\n");
        return len;
    }

    len = uart_how_many_valid_data_recv_buf(uart->port);
    while (len < expect_size) {
        len = uart_how_many_valid_data_recv_buf(uart->port);
        aos_msleep(10);
    }

    uart_pop_from_recv_buff(uart->port, data, expect_size);
#else
    uint32_t len = 0;

    if ((NULL == uart) || (NULL == data)) {
        printf("arg err!!!\r\n");
        return -1;
    }

    startms = bl2_timer_now_ms();
    len = uart_how_many_valid_data_recv_buf(uart->port);
    if (len < expect_size) {
        if (0xFFFFFFFF == timeout) {
            while (1) {
                len = uart_how_many_valid_data_recv_buf(uart->port);
                if (len >= expect_size) {
                    break;
                }
                aos_msleep(2);
            }
        } else {
            while (1) {
                endms = bl2_timer_now_ms();
                len = uart_how_many_valid_data_recv_buf(uart->port);
                if (len >= expect_size) {
                    break;
                }
                if ((endms - startms) >= timeout) {
                    break;
                }
                aos_msleep(2);
            }
        }
    }

    if (len < expect_size) {
        real_size = len;
    } else {
        real_size = expect_size;
    }
    if (NULL != recv_size) {
        *recv_size = real_size;
    }
    if (real_size > 0) {
        uart_pop_from_recv_buff(uart->port, data, real_size);
    }
#endif

    return len > 0 ? 0 : -1;
}

