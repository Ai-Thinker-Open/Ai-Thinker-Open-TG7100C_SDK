/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include <aos/kernel.h>
#include <aos/aos.h>
#include <k_api.h>
#include <hal/soc/spi.h>
#include <hal/soc/gpio.h>
#include <hal/soc/soc.h>
#include "hal_test_entry.h"

#define SPI1_PORT_NUM  1
#define SPI_BUF_SIZE   5
#define SPI_BUF_SIZE2   1
#define SPI_TX_TIMEOUT 10000
#define SPI_RX_TIMEOUT 10000

#define TAG "TEST_HAL_SPI"
#define SPI_PIN_CS              (2)

/* define dev */
static spi_dev_t spi1;
//gpio_pin_handle_t gpio;
gpio_dev_t gpio = {
    .config = OUTPUT_PUSH_PULL,
    .port = SPI_PIN_CS, 
};

#define csi_gpio_pin_write(gpio, val) (val)?hal_gpio_output_high(&gpio):hal_gpio_output_low(&gpio)

spi_dev_t test_spi;
static void led_test_task(void *args);

void show_heap()
{
    extern k_mm_head *g_kmm_head;
    int free = g_kmm_head->free_size;
    LOG("============free heap size =%d==========", free);
}

static void led_test_write_cb(uint32_t event, void *data, uint32_t size)
{
    /* check interrupt event */
    switch (event) {
        case SPI_INT_DMA_TH:    /* DMA transmit half */
            break;
        case SPI_INT_DMA_TC:    /* DMA transmit complete */
            break;
        case SPI_INT_DMA_TE:    /* DMA transmit error */
        default:
            printf("ERR: led write\r\n");
            break;
    }
    printf("----------------------%s %ld---------------------\r\n", __func__, event);
}

int led_test_init(void)
{
  test_spi.port = 0;
  test_spi.config.mode = HAL_SPI_MODE_MASTER;
  test_spi.config.data_width = HAL_SPI_DATA_WIDTH_8B;
  test_spi.config.data_shift = HAL_SPI_DATA_SHIFT_MSB;
  test_spi.config.clock_mode = HAL_SPI_CLK_POL_LOW_PHA_1E;
  test_spi.config.freq = 2500000;
  hal_spi_init(&test_spi);
  hal_spi_send_recv_cb_reg(&test_spi, (hal_spi_cb_t)(&led_test_write_cb));
  led_test_task(NULL);
  hal_spi_finalize(&test_spi);
  return 0;
}
#define TEST_BUF_SIZE 3000
static void led_test_task(void *args)
{
    int i = 0;
    uint8_t *send_buffer = aos_malloc(TEST_BUF_SIZE);
    uint8_t *recv_buffer = aos_malloc(TEST_BUF_SIZE);

    for (i = 0; i < 255; i++) {
        send_buffer[i] = i;
        recv_buffer[i] = 0;
    }
    for (i = 0; i < 100; i++) {
        memset(recv_buffer, 0, TEST_BUF_SIZE);
        hal_spi_send_recv(&test_spi, send_buffer, recv_buffer, TEST_BUF_SIZE, 0);
        aos_msleep(50);
        if (memcmp(send_buffer, recv_buffer, TEST_BUF_SIZE) != 0) {
            printf("check failed!!!!!!\r\n");
        } else {
            printf("check ok\r\n");
        }
    }
    aos_free(send_buffer);
    aos_free(recv_buffer);
}
void aos_spi_cb(spi_event_t event, void *data, uint32_t size)
{
    int i;
    uint8_t *recv_data = (uint8_t *)data;

    printf("event:%d\r\n", event);
    
    if (data != NULL) {
        printf("data:%p\r\n", (uint8_t *)data);
        for (i = 0; i < size; i++) {
            printf("spi_data:%d\r\n",recv_data[i]);
        }
    }
}

static void aos_hal_spi_master_send(uint16_t size, uint32_t frequency, uint32_t time)
{
    int ret1     = -1;
    int ret2     = -1;
    int ret3     = -1;
    int i       = 0;
    int n        = 0;
    char spi_data_buf[SPI_BUF_SIZE];
    spi1.config.freq = frequency;

    ret1 = hal_spi_init(&spi1);
    if (ret1 != 0) {
        printf("spi1 init error !\n");
    }

    for (i = 0; i < size; i++) {
        spi_data_buf[i] = i + 2;
    }

    while(n<10) {
        csi_gpio_pin_write(gpio, 0);
        ret2 = hal_spi_send(&spi1, spi_data_buf, size, time);
        csi_gpio_pin_write(gpio, 1);
        aos_msleep(50);
        for (int i = 0; i < size; i++)
        {
            printf("master send is %d\n",spi_data_buf[i]);
        }
        if (ret2 == 0) {
            printf("spi1 data send succeed !\n");
        }
        n++;
    };

    ret3 = hal_spi_finalize(&spi1);
    if (ret3 != 0) {
        printf("spi1 finalize error !\n");
    }

    if (ret1 == 0 && ret2 == 0 && ret3 == 0) {
        AT_BACK_OK();
    }
    else{
        AT_BACK_ERR();
    }
}

static void aos_hal_spi_master_recv(uint16_t size)
{
    int ret1     = -1;
    int ret2     = -1;
    int ret2_result     = 0;
    int ret3     = -1;
    int n        = 0;
    char spi_data_buf1[SPI_BUF_SIZE]={0};
    spi1.config.freq = 100000;

    ret1 = hal_spi_init(&spi1);
    if (ret1 != 0) {
        printf("spi1 init error !\n");
    }

    hal_spi_send_recv_cb_reg(&spi1, aos_spi_cb);

    while(n<10) {
        csi_gpio_pin_write(gpio, 0);
        ret2 = hal_spi_recv(&spi1, spi_data_buf1, size, SPI_RX_TIMEOUT);
        csi_gpio_pin_write(gpio, 1);
        aos_msleep(50);
        for (int i = 0; i < size; i++)
        {
            printf("master recv is %d\n",spi_data_buf1[i]);
            if(spi_data_buf1[i] != i + 3){
                ret2_result += 1;
            }
        }
        
        if (ret2 == 0) {
            printf("spi1 data recv succeed !\n");
        }
        else
        {
            printf("spi1 data recv error !\n");
        }
        n++;
    };

    ret3 = hal_spi_finalize(&spi1);
    if (ret3 != 0) {
        printf("spi1 finalize error !\n");
    }

    if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0) {
        AT_BACK_OK();
    }
    else{
        AT_BACK_ERR();
    }
}

static void aos_hal_spi_master_send_recv(uint16_t size)
{
    int ret1     = -1;
    int ret2     = -1;
    int ret2_result     = 0;
    int ret3     = -1;
    int n        = 0;
    int i       = 0;
    char spi_data_buf[SPI_BUF_SIZE];
    char spi_data_buf1[SPI_BUF_SIZE]={0};
    spi1.config.freq = 100000;

    ret1 = hal_spi_init(&spi1);
    if (ret1 != 0) {
        printf("spi1 init error !\n");
    }

    for (i = 0; i < size; i++) {
        spi_data_buf[i] = i + 2;
    }
    hal_spi_send_recv_cb_reg(&spi1, aos_spi_cb);

    while(n<10) {
        csi_gpio_pin_write(gpio, 0);
        ret2 = hal_spi_send_recv(&spi1,spi_data_buf,spi_data_buf1,size, SPI_RX_TIMEOUT);
        csi_gpio_pin_write(gpio, 1);
        aos_msleep(50);
        for (int i = 0; i < size; i++)
        {
            printf("master send is %d\n",spi_data_buf[i]);
            printf("master recv is %d\n",spi_data_buf1[i]);
            if(spi_data_buf1[i] != i + 3){
                ret2_result += 1;
            }
        }
        
        if (ret2 == 0) {
            printf("spi1 data recv succeed !\n");
        }
        else
        {
            printf("spi1 data recv error !\n");
        }
        n++;
    };

    ret3 = hal_spi_finalize(&spi1);
    if (ret3 != 0) {
        printf("spi1 finalize error !\n");
    }

    if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0) {
        AT_BACK_OK();
    }
    else{
        AT_BACK_ERR();
    }
}

// static void aos_hal_spi_master_send_and_recv(uint16_t size, uint32_t frequency, uint32_t time)
// {
//     int ret1     = -1;
//     int ret2     = -1;
//     int ret2_result     = 0;
//     int ret3     = -1;
//     int n        = 0;
//     int i       = 0;
//     char spi_data_buf[size];
//     char spi_data_buf1[SPI_BUF_SIZE]={0};
//     spi1.config.freq = frequency;

//     ret1 = hal_spi_init(&spi1);
//     if (ret1 != 0) {
//         printf("spi1 init error !\n");
//     }

//     for (i = 0; i < size; i++) {
//         spi_data_buf[i] = i + 2;
//     }
//     while(n<10) {
//         csi_gpio_pin_write(gpio, 0);
//         ret2 = hal_spi_send_and_recv(&spi1, spi_data_buf, size, spi_data_buf1, size, time);
//         csi_gpio_pin_write(gpio, 1);
//         aos_msleep(50);
//         for (int i = 0; i < size; i++)
//         {
//             printf("master send is %d\n",spi_data_buf[i]);
//         }
        
//         if (ret2 == 0) {
//             printf("spi1 data send succeed !\n");
//         }
//         else
//         {
//             printf("spi1 data send error !\n");
//         }
//         n++;
//     };
//     n = 0;

//     while(n<10) {
//         csi_gpio_pin_write(gpio, 0);
//         ret2 = hal_spi_send_and_recv(&spi1, spi_data_buf, size, spi_data_buf1, size, time);
//         csi_gpio_pin_write(gpio, 1);
//         aos_msleep(50);
//         for (int i = 0; i < size; i++)
//         {
//             printf("master recv is %d\n",spi_data_buf1[i]);
//             if(spi_data_buf1[i] != i + 3){
//                 ret2_result += 1;
//             }
//         }
        
//         if (ret2 == 0) {
//             printf("spi1 data recv succeed !\n");
//         }
//         else
//         {
//             printf("spi1 data recv error !\n");
//         }
//         n++;
//     };

//     ret3 = hal_spi_finalize(&spi1);
//     if (ret3 != 0) {
//         printf("spi1 finalize error !\n");
//     }

//     if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0) {
//         AT_BACK_OK();
//     }
//     else{
//         AT_BACK_ERR();
//     }
// }

// static void aos_hal_spi_master_send_and_send(uint16_t size)
// {
//     int ret1     = -1;
//     int ret2     = -1;
//     int ret3     = -1;
//     int n        = 0;
//     int i       = 0;
//     char spi_data_buf[size];
//     spi1.config.freq = 100000;

//     ret1 = hal_spi_init(&spi1);
//     if (ret1 != 0) {
//         printf("spi1 init error !\n");
//     }

//     for (i = 0; i < size; i++) {
//         spi_data_buf[i] = i + 2;
//     }
//     while(n<10) {
//         csi_gpio_pin_write(gpio, 0);
//         ret2 = hal_spi_send_and_send(&spi1, spi_data_buf, size, spi_data_buf, size, SPI_TX_TIMEOUT);
//         csi_gpio_pin_write(gpio, 1);
//         aos_msleep(50);
//         if (ret2 == 0) {
//             printf("spi1 data send succeed !\n");
//             for (int i = 0; i < size; i++)
//             {
//                 printf("master send is %d\n",spi_data_buf[i]);
//             }
//         }
//         else
//         {
//             printf("spi1 data send error !\n");
//         }
//         n++;
//     };

//     ret3 = hal_spi_finalize(&spi1);
//     if (ret3 != 0) {
//         printf("spi1 finalize error !\n");
//     }

//     if (ret1 == 0 && ret2 == 0 && ret3 == 0) {
//         AT_BACK_OK();
//     }
//     else{
//         AT_BACK_ERR();
//     }
// }

void test_hal_spi_master(char *cmd, int type, char *data)
{
   // gpio = csi_gpio_pin_initialize(PA18, NULL);
   // csi_gpio_pin_config_mode(gpio, GPIO_MODE_PUSH_PULL);
   // csi_gpio_pin_config_direction(gpio, GPIO_DIRECTION_OUTPUT);
   // csi_gpio_pin_write(gpio, 1);

   // drv_pinmux_config(PA18,PIN_FUNC_GPIO);
   // drv_pinmux_config(PA21,0);
   // drv_pinmux_config(PA22,0);
   // drv_pinmux_config(PA23,0);

    hal_gpio_init(&gpio);
  
    spi1.port = SPI1_PORT_NUM;
    spi1.config.mode  = HAL_SPI_MODE_MASTER;
    spi1.config.data_width = HAL_SPI_DATA_WIDTH_8B;
    spi1.config.data_shift = HAL_SPI_DATA_SHIFT_MSB;
    spi1.config.clock_mode = HAL_SPI_CLK_POL_LOW_PHA_1E;

    if (strcmp((const char *)data, "send_multi") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 100000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "send_single") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE2, 100000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "recv_multi") == 0) {
        aos_hal_spi_master_recv(SPI_BUF_SIZE);
    }else if (strcmp((const char *)data, "recv_single") == 0) {
        aos_hal_spi_master_recv(SPI_BUF_SIZE2);
    }else if (strcmp((const char *)data, "multi_send_recv") == 0) {
        aos_hal_spi_master_send_recv(SPI_BUF_SIZE);
    }else if (strcmp((const char *)data, "single_send_recv") == 0) {
        aos_hal_spi_master_send_recv(SPI_BUF_SIZE2);
    }
    // else if (strcmp((const char *)data, "'multi_send_and_recv'\0") == 0) {
    //     aos_hal_spi_master_send_and_recv(SPI_BUF_SIZE, 100000, SPI_TX_TIMEOUT);
    // }else if (strcmp((const char *)data, "'single_send_and_recv'\0") == 0) {
    //     aos_hal_spi_master_send_and_recv(SPI_BUF_SIZE2, 100000, SPI_TX_TIMEOUT);
    // }else if (strcmp((const char *)data, "'multi_send_and_send'\0") == 0) {
    //     aos_hal_spi_master_send_and_send(SPI_BUF_SIZE);
    // }else if (strcmp((const char *)data, "'single_send_and_send'\0") == 0) {
    //     aos_hal_spi_master_send_and_send(SPI_BUF_SIZE2);
    // }
    else if (strcmp((const char *)data, "'freq_100000'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 100000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "'freq_1M'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 1000000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "'freq_10M'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 10000000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "'freq_3M'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 3000000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "'timeout_HAL_WAIT_FOREVER'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 100000, HAL_WAIT_FOREVER);
    } else if (strcmp((const char *)data, "test") == 0) {
    	led_test_init();
    }
}
