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
				  src/utils_tlv_bl.c \
				  src/utils_getopt.c \
				  src/utils_dns.c \
				  src/utils_psk_fast.c \
				  src/utils_hmac_sha1_fast.c \
				  src/tg7100c_utils_base64.c \
				  src/utils_crc.c \
				  #src/utils_string.c
GLOBAL_INCLUDES += include

$(NAME)_SOURCES := $(COMPONENT_SRCS)

ifeq ($(CONFIG_UTIL_BASE64_ENABLE),1)
CPPFLAGS += -DUTILS_BASE64
endif
