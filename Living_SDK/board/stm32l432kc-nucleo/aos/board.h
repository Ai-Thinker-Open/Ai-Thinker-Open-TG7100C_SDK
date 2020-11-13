#define HARDWARE_REVISION   "V1.0"
#define MODEL               "STM32L4"

#ifdef BOOTLOADER
#define STDIO_UART 0
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART 2
#define STDIO_UART_BUADRATE 115200
#endif
