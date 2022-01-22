NAME := blmtd

GLOBAL_INCLUDES += include

## This component's src
$(NAME)_SOURCES := bl_mtd.c \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := .


##
#CPPFLAGS +=
