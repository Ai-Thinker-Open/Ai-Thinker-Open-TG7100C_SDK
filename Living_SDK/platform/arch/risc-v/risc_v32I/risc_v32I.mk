NAME := risc_v32I

$(NAME)_SOURCES := panic/panic_c.c

$(NAME)_SOURCES += misaligned/fp_asm.S
$(NAME)_SOURCES += misaligned/misaligned_ldst.c
GLOBAL_INCLUDES += misaligned/

$(NAME)_SOURCES += gcc/port_s.S
$(NAME)_SOURCES += gcc/port_c.c
GLOBAL_INCLUDES += gcc/

