NAME := asr5502
HOST_OPENOCD := asr5502

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino hal netmgr framework.common cjson cli digest_algorithm protocols.net

$(NAME)_CFLAGS += -DLEGA_CM4 -DALIOS_SUPPORT -DWIFI_DEVICE -D_SPI_FLASH_ENABLE_ -DDCDC_PFMMODE_CLOSE -D_SPI_FLASH_120MHz_ -DCFG_MIMO_UF -DCFG_OS_VERSION_CHECK
$(NAME)_CFLAGS += -DCFG_BATX=1 -DCFG_BARX=1  -DCFG_REORD_BUF=4  -DCFG_SPC=4  -DCFG_TXDESC0=4 -DCFG_TXDESC1=4 -DCFG_TXDESC2=4 -DCFG_TXDESC3=4 -DCFG_TXDESC4=4 -DCFG_CMON -DCFG_MDM_VER_V21 -DCFG_SOFTAP_SUPPORT -DCFG_SNIFFER_SUPPORT -DCFG_DBG=2 -D__FPU_PRESENT=1 -DDX_CC_TEE -DHASH_SHA_512_SUPPORTED -DCC_HW_VERSION=0xF0 -DDLLI_MAX_BUFF_SIZE=0x10000 -DSSI_CONFIG_TRNG_MODE=0
$(NAME)_CFLAGS += -DCFG_SNIFFER_UNICAST_SUPPORT -DCFG_BATCH_CONFIG_NETWORK_SUPPORT
#$(NAME)_CFLAGS += -DXO_FREQ_BIAS_CONFIG
$(NAME)_CFLAGS += -D_LOWER_CCA_THRESHOLD_

ifneq ($(mcu_highfreq),true)
$(NAME)_CFLAGS += -DPS_CLOSE_APLL
endif

#default a0v2 config
ifeq ($(buildsoc),a0v1)
$(NAME)_CFLAGS += -DLEGA_A0V1
else
$(NAME)_CFLAGS += -DLEGA_A0V2
endif

$(NAME)_CFLAGS += -Wall -Wno-error -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing

BINS ?=

GLOBAL_CFLAGS   += -mcpu=cortex-m4 -mthumb -mthumb-interwork -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -w -std=gnu99 -ffunction-sections -fdata-sections
GLOBAL_ASMFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += KV_CONFIG_TOTAL_SIZE=8192
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += CONFIG_AOS_CLI_STACK_SIZE=4096
GLOBAL_DEFINES += MBEDTLS_AES_ROM_TABLES

ifeq ($(wifips),true)
GLOBAL_DEFINES += WIFI_CONFIG_SUPPORT_LOWPOWER=1
endif

ifeq ($(mcu_highfreq),true)
GLOBAL_DEFINES += HIGHFREQ_MCU160_SUPPORT
endif

GLOBAL_LDFLAGS += -mcpu=cortex-m4 -mthumb -Wl,-gc-sections

GLOBAL_INCLUDES += drivers/driver/inc \
            drivers/platform/CMSIS/Include \
            drivers/common \
            drivers/platform/misc \
            drivers/platform/system/include \
            hal/inc \
            ../../../framework/uOTA/hal\
            ../../../framework/uOTA/inc

#soc
$(NAME)_SOURCES += drivers/platform/system/systick_delay.c \
            drivers/platform/system/system_cm4.c \
            drivers/platform/misc/misc.c \
            drivers/common/lega_mac_addr.c

#hal
$(NAME)_SOURCES += hal/src/hw.c \
            hal/src/gpio.c \
            hal/src/uart.c \
            hal/src/flash.c \
            hal/src/timer.c \
            hal/src/pwm.c \
            hal/src/wdg.c   \
            hal/src/adc.c   \
            hal/src/wifi_port.c \
            hal/src/pwrmgmt_hal/board_cpu_pwr.c \
            hal/src/pwrmgmt_hal/board_cpu_pwr_rtc.c \
            hal/src/pwrmgmt_hal/board_cpu_pwr_systick.c

ifeq ($(mcu_highfreq),true)
$(NAME)_PREBUILT_LIBRARY := drivers/libs/$(HOST_ARCH)/mcu_160m/libasr_wifi.a
else
$(NAME)_PREBUILT_LIBRARY := drivers/libs/$(HOST_ARCH)/mcu_52m/libasr_wifi.a
endif

ifeq ($(VENDOR_MXCHIP),1)
$(NAME)_SOURCES += hal/src/ota.c
else
$(NAME)_PREBUILT_LIBRARY += drivers/libs/$(HOST_ARCH)/libota_port.a
endif

$(NAME)_PREBUILT_LIBRARY += drivers/libs/$(HOST_ARCH)/libarm_cortexM4l_math.a

#driver
$(NAME)_SOURCES += drivers/driver/src/lega_flash_alg.c \
            drivers/driver/src/lega_efuse.c \
            drivers/driver/src/lega_board.c \
            drivers/driver/src/lega_flash.c \
            drivers/driver/src/lega_gpio.c \
            drivers/driver/src/lega_uart.c \
            drivers/driver/src/lega_wdg.c \
            drivers/driver/src/lega_pwm.c \
            drivers/driver/src/lega_adc.c \
            drivers/driver/src/lega_timer.c
