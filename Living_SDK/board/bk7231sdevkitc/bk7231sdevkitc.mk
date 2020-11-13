NAME := board_bk7231s

JTAG := jlink

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board bk7231s
MODULE             := BK7231S
HOST_ARCH          := ARM968E-S
HOST_MCU_FAMILY    := bk7231s
SUPPORT_MBINS      := no

$(NAME)_SOURCES := board.c

GLOBAL_INCLUDES += .
GLOBAL_DEFINES += CLI_CONFIG_SUPPORT_BOARD_CMD=1

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_BK7231S
CONFIG_SYSINFO_DEVICE_NAME   := BK7231S


GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDS_INCLUDES += $(SOURCE_ROOT)/board/bk7231sdevkitc/bk7231sdevkitc.ld

# Extra build target include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
ifeq ($(PING_PONG_OTA),1)
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_pingpong_bin.mk
else
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
endif

# Define default component testcase for certification
ifneq (, $(findstring yts, $(BUILD_STRING)))
TEST_COMPONENTS += basic rhino yloop api kv vfs 
TEST_COMPONENTS += cjson list digest_algorithm hashtable 
TEST_COMPONENTS += netmgr wifi_hal 
TEST_COMPONENTS += http cloudcoap
endif
