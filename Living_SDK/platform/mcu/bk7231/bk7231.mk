NAME := bk7231

HOST_OPENOCD := bk7231

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS := platform/arch/arm/armv5
$(NAME)_COMPONENTS += libc rhino hal yloop modules.fs.kv alicrypto digest_algorithm
$(NAME)_COMPONENTS += protocols.net
$(NAME)_COMPONENTS += platform/mcu/bk7231/hal_init
$(NAME)_COMPONENTS += platform/mcu/bk7231/aos/framework_runtime
$(NAME)_COMPONENTS += platform/mcu/bk7231/aos/app_runtime

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += CONFIG_AOS_CLI_BOARD
GLOBAL_DEFINES += CONFIG_AOS_FOTA_BREAKPOINT
GLOBAL_DEFINES += CONFIG_AOS_CLI_STACK_SIZE=4096

GLOBAL_CFLAGS += -mcpu=arm968e-s \
                 -march=armv5te \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing


GLOBAL_INCLUDES += beken/lwip-2.0.2/port \
                   beken/common \
                   beken/app/config \
                   beken/func/include \
                   beken/os/include \
                   beken/driver/include \
                   beken/driver/common \
                   beken/ip/common \
                   beken

GLOBAL_LDFLAGS += -mcpu=arm968e-s \
                 -march=armv5te \
                 -mthumb -mthumb-interwork\
                 -mlittle-endian \
                 --specs=nosys.specs \
                 -nostartfiles \
                 $(CLIB_LDFLAGS_NANO_FLOAT)

BINS ?=

GLOBAL_LDS_FILES += platform/mcu/bk7231/bk7231.ld.S

$(NAME)_INCLUDES += aos

$(NAME)_SOURCES :=  aos/aos_main.c
$(NAME)_SOURCES +=  aos/soc_impl.c \
                    aos/trace_impl.c
                    
$(NAME)_SOURCES	 += hal/gpio.c \
                    hal/wdg.c \
                    hal/hw.c \
                    hal/flash.c \
					hal/uart.c \
					hal/ringbuf.c \
                    hal/StringUtils.c \
					hal/wifi_port.c \
					hal/beken_rhino.c \

#ifneq (,$(filter protocols.mesh,$(COMPONENTS)))
$(NAME)_SOURCES +=  hal/mesh_wifi_hal.c
#endif

$(NAME)_PREBUILT_LIBRARY := beken.a
