NAME := netmgrapp

$(NAME)_SOURCES := netmgrapp.c

$(NAME)_COMPONENTS += netmgr yloop cli

GLOBAL_CFLAGS += -DDISABLE_SECURE_STORAGE

ifeq ($(test_wifi_hal),1)
GLOBAL_DEFINES += TEST_WIFI_HAL_ONLY
endif
