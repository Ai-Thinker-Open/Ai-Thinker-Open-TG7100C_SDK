NAME := yts

$(NAME)_SOURCES := main.c
$(NAME)_COMPONENTS := testcase rhino.test log vfs yloop hal

GLOBAL_DEFINES += DISABLE_SECURE_STORAGE

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable

ifneq (,$(findstring linux, $(BUILD_STRING)))
$(NAME)_COMPONENTS += protocols.net netmgr modules.fs.fatfs framework.common

GLOBAL_DEFINES += YTS_LINUX
else
$(NAME)_COMPONENTS += cli
endif

GLOBAL_DEFINES += CONFIG_YTS_TEST
