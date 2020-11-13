/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
//#include "gpio_api.h"
//#include "wait_api.h"
//#include "toolchain.h"
#include "mbed_interface.h"
#include "critical.h"
#include "serial_api.h"

#if DEVICE_SERIAL
extern int stdio_uart_inited;
extern serial_t stdio_uart;
#endif

__attribute__((weak)) void mbed_die(void) {
    while (1);
}

#if 0
WEAK void mbed_die(void) {
#if !defined (NRF51_H) && !defined(TARGET_EFM32)
    core_util_critical_section_enter();
#endif
#if   (DEVICE_ERROR_RED == 1)
    gpio_t led_red; gpio_init_out(&led_red, LED_RED);
#elif (DEVICE_ERROR_PATTERN == 1)
    gpio_t led_1; gpio_init_out(&led_1, LED1);
    gpio_t led_2; gpio_init_out(&led_2, LED2);
    gpio_t led_3; gpio_init_out(&led_3, LED3);
    gpio_t led_4; gpio_init_out(&led_4, LED4);
#endif

    while (1) {
#if   (DEVICE_ERROR_RED == 1)
        gpio_write(&led_red, 1);

#elif (DEVICE_ERROR_PATTERN == 1)
        gpio_write(&led_1, 1);
        gpio_write(&led_2, 0);
        gpio_write(&led_3, 0);
        gpio_write(&led_4, 1);
#endif

        wait_ms(150);

#if   (DEVICE_ERROR_RED == 1)
        gpio_write(&led_red, 0);

#elif (DEVICE_ERROR_PATTERN == 1)
        gpio_write(&led_1, 0);
        gpio_write(&led_2, 1);
        gpio_write(&led_3, 1);
        gpio_write(&led_4, 0);
#endif

        wait_ms(150);
    }
}
#endif

void mbed_error_printf(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    mbed_error_vfprintf(format, arg);
    va_end(arg);
}

void mbed_error_vfprintf(const char * format, va_list arg) {
#if DEVICE_SERIAL
    core_util_critical_section_enter();
    char buffer[128];
    int size = vsprintf(buffer, format, arg);
    if (size > 0) {
        if (!stdio_uart_inited) {
        serial_init(&stdio_uart, STDIO_UART_TX, STDIO_UART_RX);
        }
        for (int i = 0; i < size; i++) {
            serial_putc(&stdio_uart, buffer[i]);
        }
    }
    core_util_critical_section_exit();
#endif
}
