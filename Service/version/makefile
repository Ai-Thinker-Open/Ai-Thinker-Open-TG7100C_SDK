CURRENT_PATH = $(PWD)
BUILD_ROOT_PATH = $(CURRENT_PATH)/../..
APP_SOURCE_PATH = $(BUILD_ROOT_PATH)/Products/$(APP_TYPE)/$(APP)
APP_MAKEFILE_PATH = $(APP_SOURCE_PATH)
README_OUT_PATH = $(BUILD_ROOT_PATH)/out/$(APP)@$(BOARD)/
APP_MAKEFILE = $(APP_MAKEFILE_PATH)/$(APP).mk

target = fw_app_version


INCLUDE       = -I$(CURRENT_PATH) \
				-I$(BUILD_ROOT_PATH)/prebuild/include \
				-I$(BUILD_ROOT_PATH)/prebuild/include/kernel/vfs/include

$(target)_sources = $(wildcard ./*.c)
$(target)_objs = $(patsubst %.c,%.o,$($(target)_sources))

## ARMV7 M4 GCC
CROSS_PREFIX?="arm-none-eabi-"
CC=$(CROSS_PREFIX)gcc
LD=$(CROSS_PREFIX)ld

# Compile date
CONFIG_COMPILE_DATE ?= $(shell date +%Y%m%d.%H%M%S)
ifneq ("$(BOARD)", "")
VERSION_CONFIG_KEY = "CONFIG_FIRMWARE_VERSION "
VERSION_STRING = $(shell grep ${VERSION_CONFIG_KEY} ${APP_MAKEFILE})
$(foreach v, \
    $(VERSION_STRING), \
        $(eval CONFIG_FIRMWARE_VERSION := $(v)) \
)
endif
CONFIG_FIRMWARE_VERSION ?= app-1.6.6
CONFIG_SYSINFO_APP_VERSION = ${CONFIG_FIRMWARE_VERSION}-${CONFIG_COMPILE_DATE}
CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"
ifneq ("$(BOARD)", "")
$(info fw_app_version: ${CONFIG_SYSINFO_APP_VERSION})
$(shell echo "version : ${CONFIG_SYSINFO_APP_VERSION}" > $(README_OUT_PATH)/readme.txt)
endif

ifeq ("$(BOARD)", "hf-lpt230")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RDA5981
	CONFIG_SYSINFO_DEVICE_NAME := HF-LPT230
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w
else ifeq ("$(BOARD)", "hf-lpt130")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RDA5981
	CONFIG_SYSINFO_DEVICE_NAME := HF-LPT130
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w
else ifeq ("$(BOARD)", "hf-lpb130")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RDA5981
	CONFIG_SYSINFO_DEVICE_NAME := HF-LPB130
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w
else ifeq ("$(BOARD)", "hf-lpb135")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RDA5981
	CONFIG_SYSINFO_DEVICE_NAME := HF-LPB135
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w
else ifeq ("$(BOARD)", "uno-91h")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RDA5981
	CONFIG_SYSINFO_DEVICE_NAME := UNO-91H
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w

else ifeq ("$(BOARD)", "mk1101")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MX1101
	CONFIG_SYSINFO_DEVICE_NAME := MX1101
else ifeq ("$(BOARD)", "mk3060")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3060
	CONFIG_SYSINFO_DEVICE_NAME := MK3060
	CFLAGS +=  -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3061
	CONFIG_SYSINFO_DEVICE_NAME := MK3061
	CFLAGS +=  -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11  
else ifeq ("$(BOARD)", "mk3080")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3080
	CONFIG_SYSINFO_DEVICE_NAME := MK3080
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w
else ifeq ("$(BOARD)", "mk3092")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3092
	CONFIG_SYSINFO_DEVICE_NAME := MK3092
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w
else ifeq ("$(BOARD)", "mk3165")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3165
	CONFIG_SYSINFO_DEVICE_NAME := MK3165
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w
else ifeq ("$(BOARD)", "mk3166")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3166
	CONFIG_SYSINFO_DEVICE_NAME := MK3166
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w
else ifeq ("$(BOARD)", "mk3239")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3239
	CONFIG_SYSINFO_DEVICE_NAME := MK3239
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w
else ifeq ("$(BOARD)", "mk5080")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK5080
	CONFIG_SYSINFO_DEVICE_NAME := MK5080
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -march=armv7-m -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -w

else ifeq ("$(BOARD)", "stm32f769i-discovery")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_f769-nucleo
	CONFIG_SYSINFO_DEVICE_NAME := f769-nucleo
else ifeq ("$(BOARD)", "stm32l432kc-nucleo")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_432-nucleo
	CONFIG_SYSINFO_DEVICE_NAME := 432-nucleo
else ifeq ("$(BOARD)", "stm32l433rc-nucleo")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_433-nucleo
	CONFIG_SYSINFO_DEVICE_NAME := 433-nucleo
else ifeq ("$(BOARD)", "stm32l476rg-nucleo")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_476-nucleo
	CONFIG_SYSINFO_DEVICE_NAME := 476-nucleo
else ifeq ("$(BOARD)", "stm32l496g-nucleo")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_496-discovery
	CONFIG_SYSINFO_DEVICE_NAME := 496-discovery

##  toolchains['xtensa-lx106']
else ifeq ("$(BOARD)", "esp8266")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ESP8266
	CONFIG_SYSINFO_DEVICE_NAME := ESP8266
##  toolchains['xtensa-esp32']
else ifeq ("$(BOARD)", "esp32devkitc")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ESP32
	CONFIG_SYSINFO_DEVICE_NAME := ESP32
else ifeq ("$(BOARD)", "asr5502")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_LEGAWIFI
	CONFIG_SYSINFO_DEVICE_NAME := 5502A0V240A
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -mfpu=fpv4-sp-d16 -std=gnu11 -w
else ifeq ("$(BOARD)", "mx1270")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_LEGAWIFI
	CONFIG_SYSINFO_DEVICE_NAME := 5502A0V240A
	CFLAGS += -Os -Werror -fdata-sections -ffunction-sections -mcpu=cortex-m4 -mfloat-abi=soft -mlittle-endian -mthumb -mthumb-interwork -mfpu=fpv4-sp-d16 -std=gnu11 -w

else ifeq ("$(BOARD)", "bk7231devkitc")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_BK7231
	CONFIG_SYSINFO_DEVICE_NAME := BK7231
    CFLAGS +=  -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11
else ifeq ("$(BOARD)", "bk7231sdevkitc")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_BK7231S
	CONFIG_SYSINFO_DEVICE_NAME := BK7231S
    CFLAGS +=  -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11
else ifeq ("$(BOARD)", "bk7231udevkitc")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_BK7231U
	CONFIG_SYSINFO_DEVICE_NAME := BK7231U_MODULE
    CFLAGS +=  -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian -w -Os -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11
else ifeq ("$(BOARD)", "b_l475e")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_B-L475E
	CONFIG_SYSINFO_DEVICE_NAME := B-L475E

else ifeq ("$(BOARD)", "eml3047")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_EML3047
	CONFIG_SYSINFO_DEVICE_NAME := EML3047
else ifeq ("$(BOARD)", "frdmkl27z")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MKL27Z644
	CONFIG_SYSINFO_DEVICE_NAME := MKL27Z644
else ifeq ("$(BOARD)", "lpcxpresso54102")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_LPC54102
	CONFIG_SYSINFO_DEVICE_NAME := LPC54102

else ifeq ("$(BOARD)", "pca10056")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_pca10056
	CONFIG_SYSINFO_DEVICE_NAME := pca10056

##  toolchains['arm-rockchip-linux-gnueabihf']
else ifeq ("$(BOARD)", "rk1108")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RASP
	CONFIG_SYSINFO_DEVICE_NAME := RASP

##  toolchains['nds32le-elf-newlib-v3']
else ifeq ("$(BOARD)", "sv6266_evb")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_SV6266_EVB
	CONFIG_SYSINFO_DEVICE_NAME := SV6266_EVB

else ifeq ("$(BOARD)", "tg7100cevb")
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_TG7100CEVB
	CONFIG_SYSINFO_DEVICE_NAME := TG7100CEVB
	CFLAGS += -std=gnu99 -Os -g -msmall-data-limit=4 -fno-omit-frame-pointer -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -fshort-enums -fno-strict-aliasing -march=rv32imfc -mabi=ilp32f
else
	CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_UNKNOW_MODEL
	CONFIG_SYSINFO_DEVICE_NAME := UNKNOW_MODEL
endif
	CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
	CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"


all : $(target)

$(target):$($(target)_objs) 
	@mkdir -p obj
	@mv *.o obj
	@-$(RM)  *.d
	@$(AR) -rcs $(PWD)/../../prebuild/lib/$(target).a obj/*.o
	@$(RM) -rf obj

%.o:%.c
	@$(CC) -c $< -o $@  $(CFLAGS) $(INCLUDE)

clean:
	@-$(RM)  *.d obj/*
