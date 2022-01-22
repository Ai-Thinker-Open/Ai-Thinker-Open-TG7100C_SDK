/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include "hal_test_entry.h"
#include <hal/soc/uart.h>
#include <hal/soc/soc.h>
//#include <yoc/atserver.h>

//#define AT_BACK_OK()       printf("\r\nOK\r\n")
//#define AT_BACK_ERR()      printf("\r\nERROR\r\n")
//#define AT_BACK_READY()    printf("\r\nREADY\r\n")

#define UART1_PORT_NUM  1
#define UART_BUF_SIZE   10
#define UART_BUF_SIZE_1   1
#define UART_RX_TIMEOUT 10000
#define SEND_COUNT 20

#define TAG "TEST_AOS_HAL"

static uint8_t pins[] = {4, 3};

/* define dev */
static uart_dev_t uart1 = {
    .priv = pins,
};

/* data buffer */
uint8_t uart_data_buf[UART_BUF_SIZE];

int aos_hal_uart_slaver_test(int baud,int width,int parity,int stop,int size,int timeout)
{
    uint32_t count   = 0;
    uint32_t ret     = -1;
    uint32_t i    = 0;
    uint32_t j  = 0;
    uint32_t rx_size = 0;


    /* uart port set */
    uart1.port = UART1_PORT_NUM;

    
    /* uart attr config */
    uart1.config.baud_rate    = baud;
    uart1.config.data_width   = width;
    uart1.config.parity       = parity;
    uart1.config.stop_bits    = stop;
    uart1.config.flow_control = FLOW_CONTROL_DISABLED;
    uart1.config.mode         = MODE_TX_RX;

    printf("start test \n");

    /* init uart1 with the given settings */
    ret = hal_uart_init(&uart1);
    if (ret != 0)
        LOGE(TAG, "uart1 init error ! err_id=%d", ret);

    for (i = 0; i < size; i++) {
        uart_data_buf[i] = i + '0';
    }
    ret = hal_uart_send(&uart1, uart_data_buf, size, timeout);
    if (ret == 0) {
        printf("uart1 data send succeed !\n");
    }

    // ret = hal_uart_recv(&uart1, uart_data_buf, size,timeout);
    // if (ret == 0) {
    //     printf("uart1 data received succeed !\n");
    // }else {
    //     AT_BACK_ERR();
    // }

    /* scan uart1 every 100ms, if data received send it back */
    i = 0;
    AT_BACK_READY();
    while(1) {
        ret = hal_uart_recv_II(&uart1, uart_data_buf, size,
                               &rx_size, timeout);
        //ret = hal_uart_recv(&uart1, uart_data_buf, size,timeout);
        if (ret == 0){
            for (int j = 0; j < rx_size; j++)
            {
                printf("recv is %c\n",uart_data_buf[j]);
                if(uart_data_buf[j] != j + 1){
                    hal_uart_finalize(&uart1);
                    AT_BACK_ERR();
                    return -1;
                }
            }
        }
        i++;
        if (i == SEND_COUNT){
            ret = hal_uart_finalize(&uart1);
            if (ret != 0){
                AT_BACK_ERR();
                return -1;
            }
            AT_BACK_OK();
            break;
        }

    };
}
    
void test_hal_uart_slaver(char *cmd, int type, char *data)
{
    if (strcmp((const char *)data, "'RECV_II_115200'\0") == 0) {
        aos_hal_uart_slaver_test(115200,DATA_WIDTH_8BIT,NO_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_9600'\0") == 0) {
        aos_hal_uart_slaver_test(9600,DATA_WIDTH_8BIT,NO_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_38400'\0") == 0) {
        aos_hal_uart_slaver_test(38400,DATA_WIDTH_8BIT,NO_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_8BIT'\0") == 0) {
        aos_hal_uart_slaver_test(38400,DATA_WIDTH_8BIT,NO_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_7BIT'\0") == 0) {
        aos_hal_uart_slaver_test(38400,DATA_WIDTH_7BIT,NO_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_6BIT'\0") == 0) {
        aos_hal_uart_slaver_test(115200,DATA_WIDTH_6BIT,NO_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_5BIT'\0") == 0) {
        aos_hal_uart_slaver_test(9600,DATA_WIDTH_5BIT,NO_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_NOPARITY'\0") == 0) {
        aos_hal_uart_slaver_test(9600,DATA_WIDTH_8BIT,NO_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_EVENPARITY'\0") == 0) {
        aos_hal_uart_slaver_test(9600,DATA_WIDTH_8BIT,EVEN_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_ODDPARITY'\0") == 0) {
        aos_hal_uart_slaver_test(115200,DATA_WIDTH_8BIT,ODD_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_STOP_BITS_1'\0") == 0) {
        aos_hal_uart_slaver_test(115200,DATA_WIDTH_7BIT,ODD_PARITY,STOP_BITS_1,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_STOP_BITS_2'\0") == 0) {
        aos_hal_uart_slaver_test(115200,DATA_WIDTH_7BIT,ODD_PARITY,STOP_BITS_2,UART_BUF_SIZE,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'SINGLE_RECV_II'\0") == 0) {
        aos_hal_uart_slaver_test(115200,DATA_WIDTH_8BIT,NO_PARITY,STOP_BITS_1,UART_BUF_SIZE_1,UART_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'RECV_II_HAL_WAIT_FOREVER'\0") == 0) {
        aos_hal_uart_slaver_test(115200,DATA_WIDTH_8BIT,NO_PARITY,STOP_BITS_1,UART_BUF_SIZE,HAL_WAIT_FOREVER);
    }
}



