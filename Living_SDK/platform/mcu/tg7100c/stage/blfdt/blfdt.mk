NAME := blfdt


$(NAME)_SOURCES := src/fdt.c src/fdt_ro.c src/fdt_wip.c src/fdt_sw.c src/fdt_rw.c src/fdt_strerror.c\
src/fdt_empty_tree.c src/fdt_addresses.c src/fdt_overlay.c

#test/tc_blfdt_dump.c test/tc_blfdt_wifi.c \
test/blfdt_cli_test.c \

#$(NAME)_INCLUDES :=

GLOBAL_INCLUDES += inc

