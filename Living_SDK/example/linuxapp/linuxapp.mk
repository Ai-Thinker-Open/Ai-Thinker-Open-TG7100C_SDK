NAME := linuxapp

$(NAME)_SOURCES    := main.c

GLOBAL_DEFINES += DISABLE_SECURE_STORAGE

$(NAME)_COMPONENTS := cli yloop

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS += protocols.net
endif

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS += benchmarks
GLOBAL_DEFINES     += CONFIG_CMD_BENCHMARKS
endif

