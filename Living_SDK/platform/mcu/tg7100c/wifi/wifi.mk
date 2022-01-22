# Component Makefile
#
## These include paths would be exported to project level
NAME := tg7100c_wifi_driver

COMPONENT_ADD_INCLUDEDIRS += drv_wifi_tg7100c/tg7100c_wifi_driver/include \
                             drv_wifi_tg7100c/os_port/aos \
                             drv_wifi_tg7100c/inc/aos \

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := drv_wifi_tg7100c/tg7100c_wifi_driver

## This component's src
COMPONENT_SRCS := drv_wifi_tg7100c/tg7100c_wifi_driver/ipc_host.c \
                  drv_wifi_tg7100c/os_port/aos/os_hal.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_apis.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_cmds.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_irqs.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_main.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_mod_params.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_msg_rx.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_msg_tx.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_platform.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_rx.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_strs.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_tx.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/bl_utils.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/stateMachine.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/wifi.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/wifi_mgmr.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/wifi_mgmr_api.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/wifi_mgmr_ext.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/wifi_mgmr_profile.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/wifi_netif.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/wifi_mgmr_event.c \
				  drv_wifi_tg7100c/tg7100c_wifi_driver/wifi_mgmr_cli.c \
				  version.c \

$(NAME)_COMPONENTS += platform/mcu/tg7100c/wifi/firmware 

COMPONENT_SRCDIRS := drv_wifi_tg7100c/tg7100c_wifi_driver

GLOBAL_INCLUDES += $(COMPONENT_ADD_INCLUDEDIRS) $(COMPONENT_PRIV_INCLUDEDIRS)

$(NAME)_SOURCES := $(COMPONENT_SRCS)

ifeq ($(CONFIG_BL_IOT_FW_AP),1)
$(NAME)_DEFINES := CFG_STA_MAX="3"
else
$(NAME)_DEFINES := CFG_STA_MAX="1"
endif

$(NAME)_DEFINES += WIFI_MGMR_TASK_SIZE_BYTES="640*4"
$(NAME)_DEFINES += CFG_TXDESC="2"
$(NAME)_DEFINES += BL_CHIP_NAME="\"tg7100c\""

