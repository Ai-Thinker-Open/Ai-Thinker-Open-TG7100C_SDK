/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include "platform_peripheral.h"
#include "RingBufferUtils.h"
#include "board.h"

extern platform_uart_driver_t platform_uart_drivers[MICO_UART_MAX];
extern const platform_uart_t platform_uart_peripherals[];

#define UART_FIFO_SIZE 1024

static ring_buffer_t rx_ringbuf[MICO_UART_MAX];

int32_t hal_uart_init(uart_dev_t *uart)
{
    uint8_t *rx_buf;
    platform_uart_config_t config;
    
    if (uart->port > MICO_UART_MAX)
        return -1;

    rx_buf = (uint8_t *)malloc(UART_FIFO_SIZE);
    ring_buffer_init(&rx_ringbuf[uart->port], rx_buf, UART_FIFO_SIZE);
    config.baud_rate = uart->config.baud_rate;
    config.data_width = uart->config.data_width;
    config.parity = uart->config.parity;
    config.stop_bits = uart->config.stop_bits;
    config.flow_control = uart->config.flow_control;
    platform_uart_init(&platform_uart_drivers[uart->port], &platform_uart_peripherals[uart->port],
        &config, &rx_ringbuf[uart->port]);
    return 0;
}


int32_t hal_uart_finalize(uart_dev_t *uart)
{
    if (uart->port > MICO_UART_MAX)
        return -1;

    platform_uart_deinit(&platform_uart_drivers[uart->port]);
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    if (uart->port > MICO_UART_MAX)
        return -1;

    platform_uart_transmit_bytes(&platform_uart_drivers[uart->port], data, size);
    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    if (uart->port > MICO_UART_MAX)
        return -1;

    platform_uart_receive_bytes(&platform_uart_drivers[uart->port], data, expect_size, timeout);
    *recv_size = expect_size;
    return 0;
}


