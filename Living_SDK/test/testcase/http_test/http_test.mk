NAME := http_test

$(NAME)_SOURCES := http.c

#Add this MACRO for http testing
GLOBAL_DEFINES += MBEDTLS_SSL_MAX_CONTENT_LEN=8192


$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-varible
