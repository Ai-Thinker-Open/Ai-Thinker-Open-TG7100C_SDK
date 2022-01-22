# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += inc

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := priv_inc

## This component's src
COMPONENT_SRCS := src/blcrypto_suite_bignum.c             \
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

COMPONENT_SRCDIRS := src

##
#CPPFLAGS +=
