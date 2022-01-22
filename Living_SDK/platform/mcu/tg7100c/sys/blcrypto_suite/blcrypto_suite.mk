NAME := tg7100c_blcrypto_suite

GLOBAL_INCLUDES += inc priv_inc

## This component's src
$(NAME)_SOURCES += src/blcrypto_suite_bignum.c             \
                  src/blcrypto_suite_ecp.c                \
                  src/blcrypto_suite_ecp_curves.c         \
                  src/blcrypto_suite_platform_util.c      \
                  src/blcrypto_suite_porting.c            \
                  src/blcrypto_suite_hacc.c               \
                  src/blcrypto_suite_hacc_glue.c          \
                  src/blcrypto_suite_hacc_secp256r1_mul.c \
                  src/blcrypto_suite_fw_api.c             \
                  src/blcrypto_suite_export_fw.c          \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := .

