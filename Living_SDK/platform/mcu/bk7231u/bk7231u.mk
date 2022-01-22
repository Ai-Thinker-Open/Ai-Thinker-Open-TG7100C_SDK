NAME := bk7231u

HOST_OPENOCD := bk7231u

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS := platform/arch/arm/armv5
$(NAME)_COMPONENTS += libc rhino yloop  alicrypto digest_algorithm
$(NAME)_COMPONENTS += protocols.net
$(NAME)_COMPONENTS += platform/mcu/bk7231u/hal_init
#$(NAME)_COMPONENTS += platform/mcu/bk7231u/beken
#$(NAME)_COMPONENTS += platform/mcu/bk7231u/beken/ip
#$(NAME)_COMPONENTS += platform/mcu/bk7231u/beken/driver/ble
#$(NAME)_COMPONENTS += platform/mcu/bk7231u/beken/driver/ble/ble_lib
$(NAME)_COMPONENTS += platform/mcu/bk7231u/beken/alios/entry
$(NAME)_COMPONENTS += platform/mcu/bk7231u/aos/framework_runtime
$(NAME)_COMPONENTS += platform/mcu/bk7231u/aos/app_runtime
$(NAME)_COMPONENTS += hal

GLOBAL_DEFINES += CONFIG_AOS_CLI_BOARD
GLOBAL_DEFINES += CONFIG_AOS_UOTA_BREAKPOINT
GLOBAL_DEFINES += CFG_SUPPORT_ALIOS=1
GLOBAL_DEFINES += CONFIG_AOS_CLI_STACK_SIZE=4096

GLOBAL_CFLAGS += -mcpu=arm968e-s           \
                 -march=armv5te \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_INCLUDES +=  beken/alios/entry \
                    beken/alios/lwip-2.0.2/port \
                    beken/alios/os/include \
					beken/alios \
                    beken/common \
                    beken/app \
                    beken/app/config \
                    beken/func/include \
                    beken/func/uart_debug \
                    beken/driver/include \
                    beken/driver/common \
					beken/driver/i2c \
                    beken/ip/common \
					config 

GLOBAL_LDFLAGS += -mcpu=arm968e-s           \
                  -march=armv5te            \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian           \
                  --specs=nosys.specs       \
                  -nostartfiles             \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

PING_PONG_OTA := 0
ifeq ($(PING_PONG_OTA),1)
GLOBAL_DEFINES += AOS_OTA_BANK_DUAL
GLOBAL_DEFINES += AOS_OTA_DISABLE_MD5
AOS_IMG1_XIP1_LD_FILE += platform/mcu/bk7231u/bk7231u.ld
AOS_IMG2_XIP2_LD_FILE += platform/mcu/bk7231u/bk7231u_ex.ld
else
GLOBAL_LDS_FILES += platform/mcu/bk7231u/bk7231u.ld
endif

$(NAME)_INCLUDES += aos

$(NAME)_SOURCES :=  aos/aos_main.c
$(NAME)_SOURCES +=  aos/soc_impl.c \
                    aos/trace_impl.c

$(NAME)_SOURCES += beken/alios/mac_config.c
$(NAME)_SOURCES += beken/app/ate_app.c

$(NAME)_SOURCES += hal/gpio.c        \
                   hal/wdg.c         \
                   hal/hw.c          \
                   hal/flash.c       \
                   hal/uart.c        \
                   hal/StringUtils.c \
                   hal/wifi_port.c   \
                   hal/beken_rhino.c \
                   hal/pwm.c \
				   hal/i2c.c \
				   hal/spi.c \
				   hal/irda.c \
				   hal/adc.c \
				   hal/trng.c

GLOBAL_DEFINES += CFG_SUPPORT_BLE=1
btstack := vendor
GLOBAL_DEFINES  += BLE_4_2
$(NAME)_SOURCES += hal/breeze_hal/breeze_hal_ble.c
$(NAME)_SOURCES += hal/breeze_hal/breeze_hal_os.c
$(NAME)_SOURCES += hal/breeze_hal/breeze_hal_sec.c

$(NAME)_PREBUILT_LIBRARY := 
$(NAME)_PREBUILT_LIBRARY += beken/beken.a
$(NAME)_PREBUILT_LIBRARY += beken/ip/ip.a
$(NAME)_PREBUILT_LIBRARY += beken/driver/ble/ble.a
$(NAME)_PREBUILT_LIBRARY += beken/driver/ble/ble_lib.a

GLOBAL_LDFLAGS += -Wl,--wrap=boot_undefined
GLOBAL_LDFLAGS += -Wl,--wrap=boot_pabort
GLOBAL_LDFLAGS += -Wl,--wrap=boot_dabort