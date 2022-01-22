NAME := smart_outlet

# Cloud server region: MAINLAND or SINGAPORE, GERMANY
CONFIG_SERVER_REGION ?= SINGAPORE

# Cloud server env: ON_DAILY, ON_PRE, ONLINE
CONFIG_SERVER_ENV ?= ONLINE
# AWSS MODE: AWSS_ONESHOT_MODE, AWSS_DEV_AP_MODE, AWSS_BT_MODE
CONFIG_AWSS_MODE ?= AWSS_DEV_AP_MODE

CONFIG_FIRMWARE_VERSION = app-1.6.6

#1:support multi module ota test 0:not support
CONFIG_SUPPORT_MULTI_MODULE_OTA := 0

#1:support module certification test 0:not support
CONFIG_CERTIFICATION_TEST := 0

# Compile date and time
ifeq ("${DATE}", "")
	DATE=date
endif
CONFIG_COMPILE_DATE ?= $(shell ${DATE} +%Y%m%d.%H%M%S)

# Firmware type: 0 - release, 1 -  debug
CONFIG_DEBUG ?= 0

# Manufacturer self-defined AP auto find, default close
CONFIG_MANUFACT_AP_FIND ?= FALSE

CONFIG_BOARD_NAME = $(shell echo $(PLATFORM) | tr a-z A-Z | tr '-' '_')

$(NAME)_SOURCES := app_entry.c \
                   smart_outlet_main.c \
                   vendor.c \
                   device_state_manger.c \
                   factory.c \
                   property_report.c \
                   msg_process_center.c

ifeq ($(CONFIG_SUPPORT_MULTI_MODULE_OTA), 1)
$(NAME)_SOURCES += ota_hal_module1.c \
	           ota_hal_module2.c
GLOBAL_CFLAGS += -DOTA_MULTI_MODULE_DEBUG
endif

ifeq ($(CONFIG_BOARD_NAME), BK7231UDEVKITC)
$(NAME)_SOURCES += combo_net.c
CONFIG_COMBOAPP := 1
CONFIG_AWSS_MODE = AWSS_BT_MODE
endif

ifeq ($(CONFIG_BOARD_NAME), TG7100CEVB)
$(NAME)_SOURCES += combo_net.c
CONFIG_COMBOAPP := 1
CONFIG_AWSS_MODE = AWSS_BT_MODE
GLOBAL_CFLAGS += -DTG7100C_POWERSAVE_ENABLE
endif

ifeq ($(CONFIG_CERTIFICATION_TEST), 1)
$(NAME)_SOURCES += certification/ct_ut.c \
                    certification/ct_main.c \
                    certification/ct_cmds.c \
                    certification/ct_simulate.c \
                    certification/ct_ota.c

GLOBAL_CFLAGS += -DCERTIFICATION_TEST_MODE
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

GLOBAL_CFLAGS += -DCONFIG_SDK_THREAD_COST=1
GLOBAL_CFLAGS += -DCONFIG_YWSS \
                 -DBUILD_AOS \
                 -DAWSS_SUPPORT_STATIS \
                 -DDEV_ERRCODE_ENABLE \
                 -DALCS_GROUP_COMM_ENABLE \
                 -DDM_UNIFIED_SERVICE_POST \

#GLOBAL_CFLAGS += -DDEV_OFFLINE_SECURE_OTA_ENABLE
ifeq ($(CONFIG_AWSS_MODE), AWSS_DEV_AP_MODE)
GLOBAL_CFLAGS += -DAWSS_BATCH_DEVAP_ENABLE
GLOBAL_CFLAGS += -DDEV_OFFLINE_OTA_ENABLE
endif

ifeq ($(CONFIG_MANUFACT_AP_FIND), ENABLE)
GLOBAL_CFLAGS += -DMANUFACT_AP_FIND_ENABLE
endif

ifeq ($(CONFIG_SUPPORT_MCU_OTA), ENABLE)
GLOBAL_CFLAGS += -DSUPPORT_MCU_OTA
endif

GLOBAL_CFLAGS += -DREGION_${CONFIG_SERVER_REGION}

GLOBAL_CFLAGS += -D${CONFIG_AWSS_MODE}

GLOBAL_CFLAGS += -D${CONFIG_SERVER_ENV}

CONFIG_TIMER_SERVICE_ENABLE ?= 0
ifeq ($(CONFIG_TIMER_SERVICE_ENABLE), 1)
    CONFIG_DEVICE_TIMER_ENABLE := 0
endif

CONFIG_DEVICE_TIMER_ENABLE ?= 1
# 新的定时方案（新物模型，猫精APP支持），最多可支持13个定时
$(info -----:$(CONFIG_DEVICE_TIMER_ENABLE); $(CONFIG_TIMER_SERVICE_ENABLE))
ifeq ($(CONFIG_DEVICE_TIMER_ENABLE), 1)
    GLOBAL_CFLAGS += -DAIOT_DEVICE_TIMER_ENABLE
    CONFIG_TIMER_SERVICE_ENABLE := 0
endif

# 原飞燕定时方案
ifeq ($(CONFIG_TIMER_SERVICE_ENABLE), 1)
    GLOBAL_CFLAGS += -DAOS_TIMER_SERVICE
    GLOBAL_CFLAGS += -DENABLE_COUNTDOWN_LIST
    GLOBAL_CFLAGS += -DENABLE_LOCALTIMER
    # GLOBAL_CFLAGS += -DENABLE_PERIOD_TIMER
    # GLOBAL_CFLAGS += -DENABLE_RANDOM_TIMER
endif

GLOBAL_CFLAGS += -DREPORT_MULTHREAD

GLOBAL_CFLAGS += -DTSL_FY_SUPPORT

ifeq ($(CONFIG_MANUFACT_AP_FIND), ENABLE)
GLOBAL_CFLAGS += -DMANUFACT_AP_FIND_ENABLE
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

$(info CONFIG_BOARD_NAME : $(CONFIG_BOARD_NAME))
$(info server region: ${CONFIG_SERVER_REGION})
$(info server env: ${CONFIG_SERVER_ENV})
$(info APP: ${APP} Board: ${PLATFORM})
$(info host user: ${COMPILE_USER})
$(info branch: ${BRANCH})
$(info hash: ${COMMIT_HASH})
$(info app_version_new:${CONFIG_SYSINFO_APP_VERSION})

ifeq ($(CONFIG_DEBUG), 1)
GLOBAL_CFLAGS += -DDEFAULT_LOG_LEVEL_DEBUG
GLOBAL_CFLAGS += -DCONFIG_BLDTIME_MUTE_DBGLOG=0
$(info firmware type: DEBUG)
else
GLOBAL_CFLAGS += -DCONFIG_BLDTIME_MUTE_DBGLOG=1
$(info firmware type: RELEASE)
endif

ifeq ($(CONFIG_COMBOAPP), 1)
$(NAME)_COMPONENTS += framework.bluetooth.breeze
GLOBAL_CFLAGS += -DEN_COMBO_NET
GLOBAL_CFLAGS += -DAWSS_DISABLE_REGISTRAR
bz_en_auth := 1
bz_en_awss := 1
bz_long_mtu := 1
ble := 1
endif

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifeq ($(print_heap),1)
GLOBAL_CFLAGS += -DCONFIG_PRINT_HEAP
endif

ifneq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_CFLAGS += -DCONFIG_AOS_CLI
GLOBAL_CFLAGS += -D${CONFIG_BOARD_NAME}
else
GLOBAL_CFLAGS += -DESP8266_CHIPSET
endif

ifeq (y,$(pvtest))
GLOBAL_CFLAGS += -DPREVALIDATE_TEST
endif

GLOBAL_INCLUDES += ./

include ./make.settings

SWITCH_VARS := FEATURE_MQTT_COMM_ENABLED FEATURE_ALCS_ENABLED FEATURE_DEVICE_MODEL_ENABLED  \
    FEATURE_DEVICE_MODEL_GATEWAY FEATURE_DEV_BIND_ENABLED FEATURE_MQTT_SHADOW FEATURE_CLOUD_OFFLINE_RESET \
    FEATURE_DEVICE_MODEL_RAWDATA_SOLO  FEATURE_COAP_COMM_ENABLED FEATURE_HTTP2_COMM_ENABLED \
    FEATURE_HTTP_COMM_ENABLED FEATURE_SAL_ENABLED  FEATURE_WIFI_PROVISION_ENABLED FEATURE_AWSS_SUPPORT_SMARTCONFIG\
    FEATURE_AWSS_SUPPORT_ZEROCONFIG FEATURE_AWSS_SUPPORT_SMARTCONFIG FEATURE_AWSS_SUPPORT_ZEROCONFIG FEATURE_AWSS_SUPPORT_PHONEASAP \
    FEATURE_AWSS_SUPPORT_ROUTER FEATURE_AWSS_SUPPORT_DEV_AP FEATURE_OTA_ENABLED FEATURE_MQTT_AUTO_SUBSCRIBE FEATURE_MQTT_PREAUTH_SUPPORT_HTTPS_CDN

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval GLOBAL_CFLAGS += -D$(subst FEATURE_,,$(v)))) \
)

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval SDK_DEFINES += $(subst FEATURE_,-D,$(v)))) \
)
