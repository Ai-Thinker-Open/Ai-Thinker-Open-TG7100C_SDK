NAME := tg7100c_blcrypto_suite_tg

## This component's src
$(NAME)_SOURCES += src/blcrypto_suite_bignum.c             \
                  src/blcrypto_suite_ecp.c                \
                  src/blcrypto_suite_ecp_curves.c         \
                  src/blcrypto_suite_platform_util.c      \
                  src/blcrypto_suite_porting.c            \
                  src/blcrypto_suite_hacc.c               \
                  src/blcrypto_suite_aes.c                \
                  src/blcrypto_suite_hacc_glue.c          \
                  src/blcrypto_suite_hacc_secp256r1_mul.c \
                  src/blcrypto_suite_supplicant_api.c     \
                  src/blcrypto_suite_export_fw.c          \


COMPONENT_SRCDIRS += src \



GLOBAL_INCLUDES += priv_inc \
                   inc \
