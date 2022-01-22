# Component Makefile
#
## These include paths would be exported to project level
NAME := tg7100c_lwip

LWIPDIR := src
include platform/mcu/tg7100c/network/lwip/src/Filelists.mk

#$(NAME)_INCLUDES += src/include lwip-port lwip-port/config lwip-port/Alios-things lwip-port/arch
GLOBAL_INCLUDES += src/include lwip-port lwip-port/config lwip-port/Alios-things lwip-port/arch

$(NAME)_SOURCES := $(COREFILES) $(CORE4FILES) $(APIFILES) $(NETIFFILES)
ifeq ($(CONFIG_AIOT_MDNS_ENABLE), 1)
$(NAME)_SOURCES += $(MDNSFILES)
endif
$(NAME)_SOURCES += lwip-port/Alios-things/ethernetif.c lwip-port/Alios-things/sys_arch.c
