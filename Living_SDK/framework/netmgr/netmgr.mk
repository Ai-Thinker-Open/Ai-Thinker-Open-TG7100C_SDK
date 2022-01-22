NAME := netmgr

$(NAME)_TYPE := framework
ifneq (,$(ssid))
$(NAME)_DEFINES += WIFI_SSID=\"$(ssid)\"
$(NAME)_DEFINES += WIFI_PWD=\"$(pwd)\"
endif

$(NAME)_SOURCES := netmgr.c

ifeq ($(CONFIG_BOARD_NAME), TG7100CEVB)
$(NAME)_CFLAGS      += -Wno-error=incompatible-pointer-types
else
#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif
endif

$(NAME)_COMPONENTS += yloop kernel.hal

GLOBAL_INCLUDES += include ../protocol/alink/os/platform/

GLOBAL_DEFINES += AOS_NETMGR
