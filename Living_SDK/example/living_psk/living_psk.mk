NAME := living_psk
$(NAME)_SOURCES :=   app_entry.c

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



$(NAME)_SOURCES += living_example_psk.c

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifeq ($(print_heap),1)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

ifneq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CONFIG_AOS_CLI
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

ifeq (y,$(pvtest))
GLOBAL_DEFINES += PREVALIDATE_TEST
endif

GLOBAL_INCLUDES += ./

include example/linkkitapp/make.settings

SWITCH_VARS := FEATURE_MQTT_COMM_ENABLED   FEATURE_ALCS_ENABLED FEATURE_MQTT_DIRECT FEATURE_DEVICE_MODEL_ENABLED  \
    FEATURE_DEVICE_MODEL_GATEWAY  FEATURE_DEPRECATED_LINKKIT   FEATURE_DEV_BIND_ENABLED \
    FEATURE_DEVICE_MODEL_RAWDATA_SOLO  FEATURE_COAP_COMM_ENABLED FEATURE_HTTP2_COMM_ENABLED \
    FEATURE_HTTP_COMM_ENABLED FEATURE_SAL_ENABLED  FEATURE_WIFI_PROVISION_ENABLED FEATURE_AWSS_SUPPORT_SMARTCONFIG\
    FEATURE_AWSS_SUPPORT_ZEROCONFIG FEATURE_AWSS_SUPPORT_SMARTCONFIG FEATURE_AWSS_SUPPORT_ZEROCONFIG FEATURE_AWSS_SUPPORT_PHONEASAP \
    FEATURE_AWSS_SUPPORT_ROUTER FEATURE_AWSS_SUPPORT_DEV_AP FEATURE_OTA_ENABLED

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval GLOBAL_DEFINES += $(subst FEATURE_,,$(v)))) \
)
