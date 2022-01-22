NAME := cli

$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES := cli.c dumpsys.c

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
ifeq ($(CONFIG_BOARD_NAME), TG7100CEVB)
$(NAME)_CFLAGS += -Wno-error=incompatible-pointer-types
else
$(NAME)_CFLAGS  += -Wall -Werror
endif
endif

$(NAME)_COMPONENTS += hal

GLOBAL_INCLUDES += include
GLOBAL_DEFINES  += HAVE_NOT_ADVANCED_FORMATE
