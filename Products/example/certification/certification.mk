NAME := certification

# Cloud server region: MAINLAND or SINGAPORE, GERMANY
CONFIG_SERVER_REGION ?= MAINLAND

# Cloud server env: ON_DAILY, ON_PRE, ONLINE
CONFIG_SERVER_ENV ?= ONLINE

# AWSS MODE: AWSS_ONESHOT_MODE, AWSS_DEV_AP_MODE, AWSS_BT_MODE
CONFIG_AWSS_MODE ?= AWSS_DEV_AP_MODE

$(NAME)_SOURCES := ct_main.c \
                   ct_entry.c \
                   ct_cmds.c \
                   ct_ut.c \
                   ct_ota.c \
                   ct_simulate.c

# support MCU OTA, default DISABLE
CONFIG_SUPPORT_MCU_OTA ?= DISABLE

CONFIG_FIRMWARE_VERSION = app-1.6.0

# Compile date and time
ifeq ("${DATE}", "")
	DATE=date
endif

CONFIG_COMPILE_DATE ?= $(shell ${DATE} +%Y%m%d.%H%M%S)

CONFIG_BOARD_NAME = $(shell echo $(PLATFORM) | tr a-z A-Z | tr '-' '_')

# Firmware type: 0 - release, 1 -  debug
CONFIG_DEBUG ?= 1

ifeq ($(CONFIG_BOARD_NAME), BK7231UDEVKITC)
$(NAME)_SOURCES += combo_net.c
CONFIG_COMBOAPP := 1
CONFIG_AWSS_MODE = AWSS_BT_MODE
endif

ifeq ($(CONFIG_BOARD_NAME), TG7100CEVB)
$(NAME)_SOURCES += combo_net.c
CONFIG_COMBOAPP := 1
CONFIG_AWSS_MODE = AWSS_BT_MODE
endif

$(NAME)_COMPONENTS += framework/protocol/linkkit/sdk \
                      framework/protocol/linkkit/hal \
                      framework/netmgr \
                      framework/common \
                      utility/cjson \
                      framework/uOTA

GLOBAL_INCLUDES += ../../../../framework/protocol/linkkit/include \
                    ../../../../framework/protocol/linkkit/include/imports \
                    ../../../../framework/protocol/linkkit/include/exports \


GLOBAL_CFLAGS += -DCONFIG_YWSS \
                 -DBUILD_AOS \
                 -DAWSS_SUPPORT_STATIS

ifeq ($(CONFIG_SUPPORT_MCU_OTA), ENABLE)
GLOBAL_CFLAGS += -DSUPPORT_MCU_OTA
endif

GLOBAL_CFLAGS += -DREGION_${CONFIG_SERVER_REGION}

GLOBAL_CFLAGS += -D${CONFIG_SERVER_ENV}

ifeq ($(CONFIG_COMBOAPP), 1)
$(NAME)_COMPONENTS += framework.bluetooth.breeze
GLOBAL_CFLAGS += -DEN_COMBO_NET
GLOBAL_CFLAGS += -DAWSS_DISABLE_REGISTRAR
bz_en_auth := 1
bz_en_awss := 1
bz_long_mtu := 1
ble := 1
endif

ifeq ($(HOST_MCU_FAMILY),esp8266)
GLOBAL_CFLAGS += -DPOWER_CYCLE_AWSS
GLOBAL_DEFINES += ESP8266_CHIPSET
else
GLOBAL_CFLAGS += -D${CONFIG_BOARD_NAME}
$(NAME)_COMPONENTS  += cli
GLOBAL_CFLAGS += -DCONFIG_AOS_CLI

#Create thread of cm_yield in mqtt_client.c
GLOBAL_CFLAGS += -DCONFIG_SDK_THREAD_COST=1

GLOBAL_CFLAGS += -DDEV_ERRCODE_ENABLE
endif

BRANCH := $(shell git rev-parse --abbrev-ref HEAD)
GLOBAL_CFLAGS += -DGIT_BRANCH=\"${BRANCH}\"
COMMIT_HASH := $(shell git rev-parse HEAD)
GLOBAL_CFLAGS += -DGIT_HASH=\"${COMMIT_HASH}\"
COMPILE_USER := ${USER}
GLOBAL_CFLAGS += -DCOMPILE_HOST=\"${COMPILE_USER}\"
SERVER_CONF_STRING := "${CONFIG_SERVER_REGION}-${CONFIG_SERVER_ENV}"
GLOBAL_CFLAGS += -DREGION_ENV_STRING=\"${SERVER_CONF_STRING}\"
GLOBAL_CFLAGS += -DAPP_NAME=\"${APP}\"
GLOBAL_CFLAGS += -DPLATFORM=\"${PLATFORM}\"
CONFIG_SYSINFO_APP_VERSION = ${CONFIG_FIRMWARE_VERSION}-${CONFIG_COMPILE_DATE}
GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"
GLOBAL_CFLAGS += -D${CONFIG_AWSS_MODE}

ifneq ("$(BOARD)", "")
$(info server region: ${CONFIG_SERVER_REGION})
$(info server env: ${CONFIG_SERVER_ENV})
$(info APP: ${APP} Board: ${PLATFORM})
$(info host user: ${COMPILE_USER})
$(info branch: ${BRANCH})
$(info hash: ${COMMIT_HASH})
$(info app_version_new:${CONFIG_SYSINFO_APP_VERSION})
endif

ifeq ($(CONFIG_DEBUG), 1)
GLOBAL_CFLAGS += -DDEFAULT_LOG_LEVEL_DEBUG
GLOBAL_CFLAGS += -DCONFIG_BLDTIME_MUTE_DBGLOG=0
$(info firmware type: DEBUG)
else
GLOBAL_CFLAGS += -DCONFIG_BLDTIME_MUTE_DBGLOG=1
$(info firmware type: RELEASE)
endif

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

GLOBAL_INCLUDES += ./

include ./make.settings

SWITCH_VARS := FEATURE_MQTT_COMM_ENABLED   FEATURE_ALCS_ENABLED FEATURE_DEVICE_MODEL_ENABLED  \
    FEATURE_DEVICE_MODEL_GATEWAY FEATURE_DEV_BIND_ENABLED \
    FEATURE_DEVICE_MODEL_RAWDATA_SOLO  FEATURE_COAP_COMM_ENABLED FEATURE_HTTP2_COMM_ENABLED \
    FEATURE_HTTP_COMM_ENABLED FEATURE_SAL_ENABLED  FEATURE_WIFI_PROVISION_ENABLED FEATURE_AWSS_SUPPORT_SMARTCONFIG\
    FEATURE_AWSS_SUPPORT_ZEROCONFIG FEATURE_AWSS_SUPPORT_SMARTCONFIG FEATURE_AWSS_SUPPORT_ZEROCONFIG FEATURE_AWSS_SUPPORT_PHONEASAP \
    FEATURE_AWSS_SUPPORT_ROUTER FEATURE_AWSS_SUPPORT_DEV_AP FEATURE_OTA_ENABLED FEATURE_MQTT_AUTO_SUBSCRIBE FEATURE_MQTT_PREAUTH_SUPPORT_HTTPS_CDN \
    FEATURE_DM_UNIFIED_SERVICE_POST

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval GLOBAL_DEFINES += $(subst FEATURE_,,$(v)))) \
)

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval SDK_DEFINES += $(subst FEATURE_,-D,$(v)))) \
)
