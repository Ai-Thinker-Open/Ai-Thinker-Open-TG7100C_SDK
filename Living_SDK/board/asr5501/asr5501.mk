NAME := board_asr5501

MODULE              := ASR5501
HOST_ARCH           := Cortex-M4
HOST_MCU_FAMILY     := asr5501
SUPPORT_BINS        := no

define get-os-version
"AOS-R"-$(CURRENT_TIME)
endef

CONFIG_SYSINFO_OS_VERSION := $(call get-os-version)

$(NAME)_SOURCES := config/partition_conf.c  \
                config/k_config.c   \
                startup/startup.c   \
                startup/startup_cm4.S   \
                startup/board.c

GLOBAL_INCLUDES += . \
        ./config    \
        ./drivers/include \
        ./drivers/include/lwip_if   \

$(NAME)_CFLAGS += -DLEGA_CM4 -DALIOS_SUPPORT -DWIFI_DEVICE -D_SPI_FLASH_ENABLE_ -DDCDC_PFMMODE_CLOSE -DCFG_MIMO_UF
$(NAME)_CFLAGS += -DCFG_BATX=1 -DCFG_BARX=1  -DCFG_REORD_BUF=4  -DCFG_SPC=4  -DCFG_TXDESC0=4 -DCFG_TXDESC1=4 -DCFG_TXDESC2=4 -DCFG_TXDESC3=4 -DCFG_TXDESC4=4 -DCFG_CMON -DCFG_MDM_VER_V21 -DCFG_SOFTAP_SUPPORT -DCFG_SNIFFER_SUPPORT -DCFG_DBG=2 -D__FPU_PRESENT=1 -DDX_CC_TEE -DHASH_SHA_512_SUPPORTED -DCC_HW_VERSION=0xF0 -DDLLI_MAX_BUFF_SIZE=0x10000 -DSSI_CONFIG_TRNG_MODE=0

#default a0v2 config
ifeq ($(buildsoc),a0v1)
$(NAME)_CFLAGS += -DLEGA_A0V1
GLOBAL_LDS_FILES += $(SOURCE_ROOT)/board/asr5501/gcc.ld
else
$(NAME)_CFLAGS += -DLEGA_A0V2
GLOBAL_LDS_FILES += $(SOURCE_ROOT)/board/asr5501/gcc_a0v2.ld
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_LEGAWIFI
CONFIG_SYSINFO_DEVICE_NAME := 5501A0V240A

GLOBAL_DEFINES += STDIO_UART=1

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DLEGA_CM4
#GLOBAL_CFLAGS += -DAWSS_REGISTRAR_LOWPOWER_EN

EXTRA_TARGET_MAKEFILES += $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_ota_bin.mk

#GLOBAL_DEFINES += CONFIG_SOCKET_ACCESS_CONTROL
GLOBAL_CFLAGS += -DCONFIG_TCP_SOCKET_ACCESS_CONTROL
