NAME := entry

$(NAME)_TYPE := kernel

$(NAME)_CFLAGS += -marm
                   
$(NAME)_SOURCES	 := boot_handlers.S \
                    boot_vectors.S \
                    ll.S 