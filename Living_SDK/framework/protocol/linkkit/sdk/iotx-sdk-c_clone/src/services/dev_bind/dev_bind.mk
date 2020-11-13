NAME := libdev_bind

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := Bind embedded devices with user account

$(NAME)_DEFINES := USE_LPTHREAD

$(NAME)_COMPONENTS := digest_algorithm

$(NAME)_INCLUDES += . ./os/ ./os/product/ ./os/platform/ ../linkkit/dev_reset/

ifeq (y,$(FEATURE_WIFI_PROVISION_ENABLED))
$(NAME)_INCLUDES += ../awss
endif

$(NAME)_SOURCES :=
$(NAME)_SOURCES += awss_bind.c      awss_cmp_mqtt.c  awss_report.c
$(NAME)_SOURCES += awss_cmp_coap.c  awss_notify.c    awss_timer.c
$(NAME)_SOURCES += passwd.c         awss_packet.c    os/os_misc.c
$(NAME)_SOURCES += sha256.c         awss_event.c     awss_bind_statis.c

$(NAME)_DEFINES += DEBUG

ifeq ($(COMPILER),)
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing
endif
