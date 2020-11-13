#include <lwip/netifapi.h>

#include "wifi_mgmr.h"
#include "wifi_netif.h"

int wifi_netif_dhcp_start(struct netif *netif)
{
    netifapi_dhcp_start(netif);
    return 0;
}

