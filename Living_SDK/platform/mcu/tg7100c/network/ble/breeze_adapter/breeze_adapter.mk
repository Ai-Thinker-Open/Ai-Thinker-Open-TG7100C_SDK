NAME := breeze_adapter

$(NAME)_SOURCES := src/breeze_hal_ble.c  \
				   src/breeze_hal_os.c	 \
				   src/breeze_hal_sec.c

GLOBAL_INCLUDES += ../../../../../../framework/bluetooth/breeze/hal/include	