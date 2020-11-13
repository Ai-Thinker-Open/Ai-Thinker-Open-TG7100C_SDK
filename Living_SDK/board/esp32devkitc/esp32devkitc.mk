NAME := board_esp32devkitc

MODULE              := 1062
HOST_ARCH           := xtensa
HOST_MCU_FAMILY     := esp32
SUPPORT_BINS        := no

# todo: remove these after rhino/lwip ready
vcall               ?= rhino

define get-os-version
"AOS-R"-$(CURRENT_TIME)
endef

CONFIG_SYSINFO_OS_VERSION := $(call get-os-version)
CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ESP32
CONFIG_SYSINFO_DEVICE_NAME := ESP32

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DBOARD_ESP32

GLOBAL_INCLUDES += .
$(NAME)_SOURCES := board.c

EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk

ifeq ($(hci_h4),1)
GLOBAL_CFLAGS += -DCONFIG_BLE_HCI_H4_UART_PORT=1
endif
