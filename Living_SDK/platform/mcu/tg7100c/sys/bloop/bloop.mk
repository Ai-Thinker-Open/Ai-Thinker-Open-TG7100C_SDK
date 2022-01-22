NAME := bloop

$(NAME)_SOURCES += bloop/src/bloop_base.c	\
	bloop/src/bloop_handler_sys.c	\
	looprt/src/looprt.c	\
	loopset/src/loopset_ir.c	\
	loopset/src/loopset_led.c	\
	loopset/src/loopset_pwm.c	\
	loopset/src/loopset_led_cli.c

GLOBAL_INCLUDES += bloop/include looprt/include loopset/include
