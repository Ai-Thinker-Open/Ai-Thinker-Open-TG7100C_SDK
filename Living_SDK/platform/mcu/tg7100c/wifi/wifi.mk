# Component Makefile
#
## These include paths would be exported to project level
NAME := tg7100c_wifi_driver

COMPONENT_ADD_INCLUDEDIRS += tg7100c_wifi_driver/include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := tg7100c_wifi_driver

## This component's src
COMPONENT_SRCS := tg7100c_wifi_driver/ipc_host.c \
				  tg7100c_wifi_driver/os_hal.c \
				  tg7100c_wifi_driver/bl_apis.c \
				  tg7100c_wifi_driver/bl_cmds.c \
				  tg7100c_wifi_driver/bl_irqs.c \
				  tg7100c_wifi_driver/bl_main.c \
				  tg7100c_wifi_driver/bl_mod_params.c \
				  tg7100c_wifi_driver/bl_msg_rx.c \
				  tg7100c_wifi_driver/bl_msg_tx.c \
				  tg7100c_wifi_driver/bl_platform.c \
				  tg7100c_wifi_driver/bl_rx.c \
				  tg7100c_wifi_driver/bl_strs.c \
				  tg7100c_wifi_driver/bl_tx.c \
				  tg7100c_wifi_driver/bl_utils.c \
				  tg7100c_wifi_driver/stateMachine.c \
				  tg7100c_wifi_driver/wifi.c \
				  tg7100c_wifi_driver/wifi_mgmr.c \
				  tg7100c_wifi_driver/wifi_mgmr_api.c \
				  tg7100c_wifi_driver/wifi_mgmr_ext.c \
				  tg7100c_wifi_driver/wifi_mgmr_profile.c \
				  tg7100c_wifi_driver/wifi_netif.c \
				  tg7100c_wifi_driver/wifi_mgmr_event.c \
				  tg7100c_wifi_driver/wifi_mgmr_cli.c \

$(NAME)_COMPONENTS += platform/mcu/tg7100c/wifi/firmware 

COMPONENT_SRCDIRS := tg7100c_wifi_driver

GLOBAL_INCLUDES += $(COMPONENT_ADD_INCLUDEDIRS) $(COMPONENT_PRIV_INCLUDEDIRS)

$(NAME)_SOURCES := $(COMPONENT_SRCS)

ifeq ($(CONFIG_BL_IOT_FW_AP),1)
$(NAME)_DEFINES := CFG_STA_MAX="10"
else
$(NAME)_DEFINES := CFG_STA_MAX="1"
endif

