NAME := tg7100c_networks

GLOBAL_DEFINES += WITH_PRIVATE_LWIP LWIP_ERRNO_STDINCLUDE
$(NAME)_COMPONENTS += platform/mcu/tg7100c/network/lwip
$(NAME)_COMPONENTS += platform/mcu/tg7100c/network/dns_server
$(NAME)_COMPONENTS += platform/mcu/tg7100c/network/lwip_dhcpd
$(NAME)_COMPONENTS += platform/mcu/tg7100c/network/ble
$(NAME)_COMPONENTS += platform/mcu/tg7100c/network/netutils
