# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS +=

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS := src/utils_hex.c \
                  src/utils_crc.c \
                  src/utils_sha256.c \
                  src/utils_fec.c \
                  src/utils_log.c \
                  src/utils_list.c \
                  src/utils_rbtree.c \
                  src/utils_hexdump.c \
                  src/utils_time.c \
                  src/utils_notifier.c \


COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src


##
#CPPFLAGS +=
