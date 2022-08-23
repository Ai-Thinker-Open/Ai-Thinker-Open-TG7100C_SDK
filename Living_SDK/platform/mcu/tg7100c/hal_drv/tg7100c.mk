# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += tg7100c_hal

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS := tg7100c_hal/bl_uart.c \
                  tg7100c_hal/bl_chip.c \
                  tg7100c_hal/bl_cks.c \
                  tg7100c_hal/bl_sys.c \
                  tg7100c_hal/bl_dma.c \
                  tg7100c_hal/bl_irq.c \
                  tg7100c_hal/bl_sec.c \
                  tg7100c_hal/bl_boot2.c \
                  tg7100c_hal/bl_timer.c \
                  tg7100c_hal/bl_gpio.c \
                  tg7100c_hal/bl_gpio_cli.c \
                  tg7100c_hal/bl_hbn.c \
                  tg7100c_hal/bl_efuse.c \
                  tg7100c_hal/bl_flash.c \
                  tg7100c_hal/bl_pwm.c \
                  tg7100c_hal/bl_sec_aes.c \
                  tg7100c_hal/bl_sec_sha.c \
                  tg7100c_hal/bl_wifi.c \
                  tg7100c_hal/bl_wdt.c \
                  tg7100c_hal/bl_wdt_cli.c \
                  tg7100c_hal/hal_uart.c \
                  tg7100c_hal/hal_gpio.c \
                  tg7100c_hal/hal_hbn.c \
                  tg7100c_hal/hal_pwm.c \
                  tg7100c_hal/hal_boot2.c \
                  tg7100c_hal/hal_sys.c \
                  tg7100c_hal/hal_board.c \
                  tg7100c_hal/bl_adc.c \
                  tg7100c_hal/hal_ir.c \
                  tg7100c_hal/bl_ir.c \
                  tg7100c_hal/bl_dac_audio.c \
                  tg7100c_hal/hal_button.c \
                  tg7100c_hal/hal_hbnram.c \
                  tg7100c_hal/hosal_timer.c \

COMPONENT_SRCDIRS := tg7100c_hal

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(COMPONENT_OBJS))

##
CPPFLAGS += -DARCH_RISCV
ifndef CONFIG_USE_STD_DRIVER
CPPFLAGS += -DTG7100C_USE_HAL_DRIVER
endif
