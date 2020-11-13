NAME := tls_client

DTLS := no

$(NAME)_SOURCES     := tls_client.c

GLOBAL_DEFINES += DISABLE_SECURE_STORAGE

ifeq ($(DTLS), yes)
$(NAME)_DEFINES     += DTLS_ENABLED
$(NAME)_SOURCES     += dtls_client.c
endif

$(NAME)_COMPONENTS  := imbedtls alicrypto netmgr

