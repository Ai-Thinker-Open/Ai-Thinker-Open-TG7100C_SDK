NAME := basic_test

$(NAME)_SOURCES := aos_test.c cutest/cut.c
$(NAME)_COMPONENTS := rhino
$(NAME)_COMPONENTS += 
$(NAME)_COMPONENTS += yloop

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-varible
