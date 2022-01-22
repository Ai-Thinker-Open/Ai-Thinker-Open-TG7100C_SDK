NAME := board_uno-91h

JTAG := jlink

$(NAME)_TYPE         := kernel
MODULE               := UNO-91H
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := rda5981x

$(NAME)_SOURCES := board.c    \
                   hal_pwm.c \
                   startup_uno-91h.s

GLOBAL_INCLUDES += .
GLOBAL_DEFINES  += STDIO_UART=0
GLOBAL_DEFINES  += RHINO_CONFIG_TICK_TASK=0 RHINO_CONFIG_WORKQUEUE=0
GLOBAL_DEFINES  += AOS_CLI_MINI_SIZE=1
GLOBAL_DEFINES  += DELETE_HFILOP_CODE

ifeq ($(shell uname -o), Msys)
    CURRENT_TIME = $(shell ${DATE} +%Y%m%d.%H%M)
endif

#CONFIG_SYSINFO_KERNEL_VERSION = AOS-R-1.3.4

CONFIG_SYSINFO_OS_VERSION := $(call get-os-version)

$(warning $(CONFIG_SYSINFO_OS_VERSION))

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RDA5981
CONFIG_SYSINFO_DEVICE_NAME := UNO-91H

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_KERNEL_VERSION=\"$(CONFIG_SYSINFO_KERNEL_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"

GLOBAL_LDFLAGS += -L $(SOURCE_ROOT)/board/uno-91h

$(NAME)_COMPONENTS += framework.uOTA.hal

# Global defines
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_CFLAGS  += -DRDA5981X -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
GLOBAL_CFLAGS += -DRDA5981A

GLOBAL_LDFLAGS += -T uno-91h.ld

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES  +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
