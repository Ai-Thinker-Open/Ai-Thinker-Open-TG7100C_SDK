# Component Makefile
#
## These include paths would be exported to project level
NAME := hal_drv

## This component's src

$(NAME)_SOURCES += tg7100c_hal/bl_uart.c \
                  tg7100c_hal/bl_chip.c \
                  tg7100c_hal/bl_cks.c \
                  tg7100c_hal/bl_sys.c \
                  tg7100c_hal/bl_dma.c \
                  tg7100c_hal/bl_irq.c \
                  tg7100c_hal/bl_sec.c \
                  tg7100c_hal/bl_boot2.c \
                  tg7100c_hal/bl_timer.c \
                  tg7100c_hal/bl_gpio.c \
                  tg7100c_hal/bl_hbn.c \
                  tg7100c_hal/bl_efuse.c \
                  tg7100c_hal/bl_flash.c \
                  tg7100c_hal/bl_pwm.c \
                  tg7100c_hal/bl_sec_aes.c \
                  tg7100c_hal/bl_sec_sha.c \
                  tg7100c_hal/bl_wifi.c \
                  tg7100c_hal/bl_wdt.c \
                  tg7100c_hal/hal_boot2.c \
                  tg7100c_hal/hal_board.c \
                  tg7100c_hal/hal_sys.c \
                  tg7100c_hal/bl_adc.c \
                  tg7100c_hal/bl_rtc.c \
                  tg7100c_hal/hal_adc.c \
                  tg7100c_hal/hal_spi.c \
                  tg7100c_hal/bl_i2c.c \
                  tg7100c_hal/hal_hwtimer.c \
                  tg7100c_hal/hal_hbn.c \
                  tg7100c_hal/hosal_timer.c \
                  #tg7100c_hal/hal_uart.c \
                  tg7100c_hal/hal_gpio.c \
                  tg7100c_hal/hal_hbn.c \
                  tg7100c_hal/hal_pwm.c \
                  tg7100c_hal/hal_ir.c \
                  tg7100c_hal/bl_ir.c \
                  tg7100c_hal/bl_dac_audio.c \
                  tg7100c_hal/hal_button.c \
                  tg7100c_hal/hal_hbnram.c \

GLOBAL_INCLUDES += tg7100c_hal
$(NAME)_DEFINES += ARCH_RISCV

ifndef CONFIG_USE_STD_DRIVER
GLOBAL_DEFINES += TG7100C_USE_HAL_DRIVER
endif
