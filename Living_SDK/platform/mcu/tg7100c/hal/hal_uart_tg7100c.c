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
#include <tg7100c_dma.h>
#include <app_uart.h>
#include <bl_timer.h>

#define UART0_DEFAULT_TX_PIN 16                                                                                                                                                           
#define UART0_DEFAULT_RX_PIN 7
#define UART1_DEFAULT_TX_PIN 4
#define UART1_DEFAULT_RX_PIN 2

typedef struct {
    uint8_t tx_pin;
    uint8_t rx_pin;
} uart_pin_t;

static const uint32_t uart_base[2] = {UART0_BASE, UART1_BASE};
static const int uart_irq[2] = {UART0_IRQn, UART1_IRQn};
static uart_rx_cb g_uart_cb[2] = {NULL}; 
static const uart_pin_t g_default_pins[2] = {
    {UART0_DEFAULT_TX_PIN, UART0_DEFAULT_RX_PIN},
    {UART1_DEFAULT_TX_PIN, UART1_DEFAULT_RX_PIN},
};

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
        if (g_uart_cb[p_uart->port]) {
            g_uart_cb[p_uart->port](p_uart);
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
        if (g_uart_cb[p_uart->port]) {
            g_uart_cb[p_uart->port](p_uart);
        }
    }
}

static int g_uart_tx_dma_chan = -1;

/**
 * @description:
 * @param {*}
 * @return {*}
 */
static void hal_uart_dma_int_handler_tx(void)
{
    if (g_uart_tx_dma_chan >= 0)
    {
        bl_dma_int_clear(g_uart_tx_dma_chan);

        /* Stop */
        DMA_Channel_Disable(g_uart_tx_dma_chan);
    }
}

int32_t hal_uart_init(uart_dev_t* uart)
{
    UART_FifoCfg_Type fifocfg;

    if (uart == NULL || uart->port >= 2) {
        return -1;
    }

    uart_pin_t *pins = (uart_pin_t *)uart->priv;
    UART_StopBits_Type stop_bits;
    
    if (pins == NULL) {
        pins = (uart_pin_t *)&g_default_pins[uart->port];
    }
    if (uart->config.data_width > DATA_WIDTH_8BIT) {
        return -1;
    }
    if (uart->config.stop_bits == STOP_BITS_1) {
        stop_bits = UART_STOPBITS_1;
    } else if (uart->config.stop_bits == STOP_BITS_2) {
        stop_bits = UART_STOPBITS_2;
    } else {
        return -1;
    }
    bl_uart_init(uart->port, pins->tx_pin, pins->rx_pin, 255, 255, uart->config.baud_rate);
    bl_uart_setconfig(uart->port,
                      uart->config.baud_rate,
                      uart->config.data_width,
                      stop_bits,
                      uart->config.parity);
    if (uart->port == UART1_ID)
    {
        /* disable uart before config */
        UART_Disable(uart->port, UART_TXRX);

        fifocfg.txFifoDmaThreshold = 16;
        fifocfg.rxFifoDmaThreshold = 16;
        fifocfg.txFifoDmaEnable = ENABLE;
        fifocfg.rxFifoDmaEnable = DISABLE;
        UART_FifoConfig(uart->port, &fifocfg);

        /* Enable tx free run mode */
        UART_TxFreeRun(uart->port, ENABLE);

        /* Enable uart */
        UART_Enable(uart->port, UART_TXRX);

        /* DMA */
        DMA_Enable();
        g_uart_tx_dma_chan = DMA_CH1;
        DMA_IntMask(g_uart_tx_dma_chan, DMA_INT_ALL, MASK);
        DMA_IntMask(g_uart_tx_dma_chan, DMA_INT_TCOMPLETED, UNMASK);
        DMA_IntMask(g_uart_tx_dma_chan, DMA_INT_ERR, UNMASK);
        bl_irq_enable(DMA_ALL_IRQn);
        bl_dma_irq_register(g_uart_tx_dma_chan, &hal_uart_dma_int_handler_tx, NULL, NULL);
        //DMA_Int_Callback_Install(g_uart_tx_dma_chan, DMA_INT_TCOMPLETED, &hal_uart_dma_int_handler_tx);
    }

    /* UART irq */
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
    if (g_uart_tx_dma_chan >= 0)
    {
        DMA_Channel_Disable(g_uart_tx_dma_chan);
        g_uart_tx_dma_chan = -1;
    }
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

    if (g_uart_tx_dma_chan < 0 || uart->port == 0)
    {
        while (i < size) {
            bl_uart_data_send(uart->port, ptr[i]);
            i++;
        }
    }
    else
    {
        /* Config DMA */
        DMA_LLI_Ctrl_Type uart_lli_tx = {0};
        struct DMA_Control_Reg uart_tx_dmaCtrlRegVal = {0};
        DMA_LLI_Cfg_Type uart_tx_lliCfg = {0};

        DMA_Channel_Disable(g_uart_tx_dma_chan);

        DMA_IntMask(g_uart_tx_dma_chan, DMA_INT_ALL, MASK);
        DMA_IntMask(g_uart_tx_dma_chan, DMA_INT_TCOMPLETED, UNMASK);
        DMA_IntMask(g_uart_tx_dma_chan, DMA_INT_ERR, UNMASK);
        bl_irq_enable(DMA_ALL_IRQn);

        uart_tx_dmaCtrlRegVal.TransferSize = size,
        uart_tx_dmaCtrlRegVal.SBSize = DMA_BURST_SIZE_4,
        uart_tx_dmaCtrlRegVal.DBSize = DMA_BURST_SIZE_4,
        uart_tx_dmaCtrlRegVal.SWidth = DMA_TRNS_WIDTH_8BITS,
        uart_tx_dmaCtrlRegVal.DWidth = DMA_TRNS_WIDTH_8BITS,
        uart_tx_dmaCtrlRegVal.SI = DMA_MINC_ENABLE,
        uart_tx_dmaCtrlRegVal.DI = DMA_PINC_DISABLE,
        uart_tx_dmaCtrlRegVal.I = 1,

        uart_lli_tx.srcDmaAddr = (uint32_t)&ptr[0];
        uart_lli_tx.destDmaAddr = (uint32_t)(UART1_BASE + UART_FIFO_WDATA_OFFSET);
        uart_lli_tx.nextLLI = 0;
        uart_lli_tx.dmaCtrl = uart_tx_dmaCtrlRegVal;

        uart_tx_lliCfg.dir = DMA_TRNS_M2P;
        uart_tx_lliCfg.srcPeriph = DMA_REQ_NONE;
        uart_tx_lliCfg.dstPeriph = DMA_REQ_UART1_TX;

        DMA_LLI_Init(g_uart_tx_dma_chan, &uart_tx_lliCfg);
        DMA_LLI_Update(g_uart_tx_dma_chan, (uint32_t)&uart_lli_tx);
        DMA_Channel_Enable(g_uart_tx_dma_chan);
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

    hal_uart_recv_II(uart, data, expect_size, &len, timeout);

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

int32_t hal_uart_send_poll(uart_dev_t *uart, const void *data, uint32_t size)
{
    return hal_uart_send(uart, data, size, 0xFFFFFFFF);
}

int32_t hal_uart_recv_poll(uart_dev_t *uart, void *data, uint32_t expect_size)
{
    return hal_uart_recv(uart, data, expect_size, 0xFFFFFFFF);
}

int32_t hal_uart_recv_cb_reg(uart_dev_t *uart, uart_rx_cb cb)
{
    if (uart == NULL) {
        return -1;
    }
    if (uart->port >= 2) {
        return -1;
    }
    g_uart_cb[uart->port] = cb;
    return 0;
}

