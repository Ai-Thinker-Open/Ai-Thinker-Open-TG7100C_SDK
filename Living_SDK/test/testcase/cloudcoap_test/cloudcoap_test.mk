NAME := cloudcoap_test

$(NAME)_SOURCES := cloud_coap.c

#Add this MACRO for coap testing
GLOBAL_DEFINES += COAP_DTLS_SUPPORT

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-varible
