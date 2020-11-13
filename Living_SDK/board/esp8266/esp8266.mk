NAME := board_esp8266

MODULE              := 1062
HOST_ARCH           := xtensa
HOST_MCU_FAMILY     := esp8266
SUPPORT_BINS        := no

# todo: remove these after rhino/lwip ready
vcall               ?= rhino

define get-os-version
"AOS-R"-$(CURRENT_TIME)
endef

CONFIG_SYSINFO_OS_VERSION := $(call get-os-version)
CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ESP8266
CONFIG_SYSINFO_DEVICE_NAME := ESP8266

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

#for activation handle
GLOBAL_CFLAGS += -DBOARD_ESP8266

GLOBAL_INCLUDES += .
$(NAME)_SOURCES := board.c

EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk

GLOBAL_DEFINES += LOCAL_PORT_ENHANCED_RAND
