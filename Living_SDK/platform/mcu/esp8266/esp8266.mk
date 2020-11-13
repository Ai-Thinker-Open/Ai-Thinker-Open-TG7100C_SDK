HOST_OPENOCD := esp8266

NAME := esp8266

$(NAME)_TYPE := kernel 

$(NAME)_COMPONENTS := framework.common yloop modules.fs.kv libc
$(NAME)_COMPONENTS += protocols.net alicrypto hal

use_private_lwip := 1

ESP_INC_PATH     := bsp/include
GLOBAL_INCLUDES  += $(ESP_INC_PATH)
GLOBAL_INCLUDES  += $(ESP_INC_PATH)/xtensa $(ESP_INC_PATH)/espressif $(ESP_INC_PATH)/espressif/esp8266
GLOBAL_INCLUDES  += $(ESP_INC_PATH)/lwip $(ESP_INC_PATH)/lwip/ipv4 $(ESP_INC_PATH)/lwip/ipv6

# $(NAME)_INCLUDES := $(ESP_INC_PATH)/driver
#GLOBAL_INCLUDES  += $(ESP_INC_PATH)/driver

GLOBAL_INCLUDES  += common hal/rec

GLOBAL_INCLUDES += ../include
GLOBAL_INCLUDES += ../../../include
#GLOBAL_INCLUDES += ../../../include/hal/soc

$(NAME)_INCLUDES += .
$(NAME)_INCLUDES += ../include
$(NAME)_INCLUDES += ../../../include
#$(NAME)_INCLUDES += ../../../include/hal/soc

GLOBAL_CFLAGS    += -u call_user_start \
				    -fno-inline-functions \
					-ffunction-sections \
					-fdata-sections \
					-mlongcalls \
					-DESPOS_FOR_ESP8266 \
					-Wl,-static

GLOBAL_LDFLAGS   += -nostdlib \
                    -Lplatform/mcu/esp8266/bsp/lib \
				    -Wl,--no-check-sections \
				    -Wl,--gc-sections \
				    -mtext-section-literals \
				    -fno-builtin-printf \
				    -Wl,-static \
				    -u call_user_start \
					-Wl,-EL \
					-mlongcalls
ifeq ($(SUPPORT_ESP8285),yes)
GLOBAL_LDS_FILES += platform/mcu/esp8266/bsp/ld/eagle.app.v6.new_8285.1024.app1.ld
else
GLOBAL_LDS_FILES += platform/mcu/esp8266/bsp/ld/eagle.app.v6.new.1024.app1.ld
endif
GLOBAL_LDFLAGS   += -Lplatform/mcu/esp8266/bsp/ld

GLOBAL_DEFINES   += CONFIG_AOS_KV_BUFFER_SIZE=8192 CONFIG_ESP_LWIP COAP_WITH_YLOOP
#GLOBAL_DEFINES   += CONFIG_AOS_CLI_BOARD
GLOBAL_DEFINES   += MBEDTLS_AES_ROM_TABLES

$(NAME)_PREBUILT_LIBRARY := bsp/lib/libhal.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libcrypto.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libmain.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libcirom.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libnet80211.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libpp.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libwpa.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libphy.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libgcc.a
ifeq ($(loopback), 1)
$(NAME)_PREBUILT_LIBRARY += bsp/lib/liblwip_loopback.a
else
$(NAME)_PREBUILT_LIBRARY += bsp/lib/liblwip.a
endif
GLOBAL_CFLAGS    += -DXT_USE_THREAD_SAFE_CLIB=0
$(NAME)_SOURCES  := bsp/entry.c
$(NAME)_SOURCES  += bsp/heap_iram.c
$(NAME)_SOURCES  += bsp/syscall.c
$(NAME)_SOURCES  += bsp/key.c

$(NAME)_SOURCES  += bsp/driver/interrupt.c
$(NAME)_SOURCES  += bsp/driver/uart.c

$(NAME)_SOURCES  += hal/uart.c
$(NAME)_SOURCES  += hal/flash.c
$(NAME)_SOURCES  += hal/wdg.c
$(NAME)_SOURCES  += hal/misc.c
$(NAME)_SOURCES  += hal/gpio.c
$(NAME)_SOURCES  += hal/wifi_port.c
$(NAME)_SOURCES  += hal/upgrade_lib.c
$(NAME)_SOURCES  += hal/rec/rec_wdt.c
$(NAME)_SOURCES  += hal/hal_adc_esp8266.c
$(NAME)_SOURCES  += hal/hal_i2c_esp8266.c
$(NAME)_SOURCES  += hal/hal_timer_esp8266.c
$(NAME)_SOURCES  += bsp/driver/gpio.c
$(NAME)_SOURCES  += bsp/driver/hw_timer.c
$(NAME)_SOURCES  += bsp/driver/i2c_master.c
$(NAME)_SOURCES  += bsp/driver/spi_interface.c

$(NAME)_CFLAGS   := -std=gnu99

ifneq ($(wifi),0)
$(NAME)_CFLAGS   += -DENABLE_WIFI
endif
$(NAME)_CFLAGS	 += -Iplatform/mcu/esp8266/bsp

ifeq (0,1)
libs := $(wildcard platform/mcu/esp8266/lib/*.a)
libs := $(foreach lib,$(libs),lib/$(notdir $(lib)))
$(NAME)_PREBUILT_LIBRARY := $(libs)
endif

ifeq ($(vcall),freertos)
GLOBAL_CFLAGS            += -I $(SDK8266_PATH)/include/espos
GLOBAL_CFLAGS            += -I $(SDK8266_PATH)/include/freertos
$(NAME)_PREBUILT_LIBRARY += lib/libespos.a
$(NAME)_PREBUILT_LIBRARY += lib/libfreertos.a
else
$(NAME)_COMPONENTS       += rhino platform/arch/xtensa/lx106
$(NAME)_SOURCES          += aos/hook_impl.c
$(NAME)_SOURCES          += aos/soc_impl.c
$(NAME)_SOURCES          += aos/trace_impl.c
endif

ifneq ($(mesh),0)
endif
