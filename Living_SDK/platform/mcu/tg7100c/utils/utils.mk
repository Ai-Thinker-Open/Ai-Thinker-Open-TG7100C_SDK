NAME := tg7100c_utils

## This component's src
COMPONENT_SRCS := src/utils_hex.c \
                  src/utils_fec.c \
                  src/utils_log.c \
                  src/utils_list.c \
                  src/utils_rbtree.c \
                  src/utils_hexdump.c \
                  src/utils_time.c \
                  src/utils_notifier.c \

GLOBAL_INCLUDES += include

$(NAME)_SOURCES := $(COMPONENT_SRCS)

