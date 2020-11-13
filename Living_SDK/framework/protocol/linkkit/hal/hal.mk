NAME := iotx-hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := HAL_OS_rhino.c \
                   HAL_TCP_rhino.c \
                   HAL_PRODUCT_rhino.c \
                   HAL_UDP_rhino.c \
                   HAL_Crypt_rhino.c \
                   HAL_AWSS_rhino.c

$(NAME)_COMPONENTS += \
                      digest_algorithm \
                      utility/log \
                      activation \
                      chip_code \
                      framework/netmgr

ifneq (,$(filter ywss4linkkit,$(COMPONENTS)))
$(NAME)_SOURCES  +=  HAL_AWSS_rhino.c
endif

ifeq (y,$(strip $(FEATURE_SUPPORT_ITLS)))
$(info FEATURE_SUPPORT_ITLS = y, so using iTLS)
$(NAME)_SOURCES += HAL_TLS_itls.c
$(NAME)_COMPONENTS += security/itls
else
$(info FEATURE_SUPPORT_ITLS != y, so using normal TLS)
$(NAME)_SOURCES += HAL_TLS_mbedtls.c HAL_DTLS_mbedtls.c
$(NAME)_COMPONENTS += security/imbedtls
$(NAME)_DEFINES += TLS_SAVE_TICKET
endif
