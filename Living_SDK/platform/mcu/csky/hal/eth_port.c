/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include <csi_kernel.h>

#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"

extern k_status_t csi_kernel_delay_ms(uint32_t ms);
extern int yoc_net_get_link_status(void);

static struct netif *pnetif = NULL;

int dhcpc_connect(struct netif *netif)
{
    int i, cnt;
    int ret = -1;
    ip_addr_t ip4gw;
    ip_addr_t netmask;

    if (netif == NULL) {
        LOG("netif err");
        return -1;
    }

    LOG("dhcp start");

#if LWIP_IPV6
    extern void netif_set_rs_count(struct netif * netif, uint8_t value);
    netif_set_rs_count(netif, LWIP_ND6_MAX_MULTICAST_SOLICIT);
#endif

#if LWIP_IPV4
    ip_addr_t oldip4addr;

    memset(&oldip4addr, 0, sizeof(oldip4addr));
    ip_addr_copy(oldip4addr, *netif_ip_addr4(netif));
    ip_addr_t ip4addr = IPADDR4_INIT(IPADDR_ANY);
    netifapi_netif_set_addr(netif, ip_2_ip4(&ip4addr), ip_2_ip4(&netif->netmask), ip_2_ip4(&netif->gw));
#endif

    for (i = 0; i < 5; i++) {
        ret = netifapi_dhcp_start(netif);

        if (ret < 0) {
            LOG("dhcp_start err,ret=%d", ret);
            netifapi_dhcp_release(netif);
            netifapi_dhcp_stop(netif);
            break;
        }

#if LWIP_IPV4
        ip_addr_copy(ip4addr, *netif_ip_addr4(netif));

        for (cnt = 0; (cnt < 200) && ip_addr_isany(&ip4addr); cnt ++) {
            ip_addr_copy(ip4addr, *netif_ip_addr4(netif));
            csi_kernel_delay_ms(20);
        }

        ip_addr_copy(netmask, *netif_ip_netmask4(netif));
        ip_addr_copy(ip4gw, *netif_ip_gw4(netif));

        if (!ip_addr_isany(&ip4addr)) {
            netifapi_netif_set_addr(netif, ip_2_ip4(&ip4addr), ip_2_ip4(&netmask), ip_2_ip4(&ip4gw));
            LOG("ipv4:%s", ipaddr_ntoa(&ip4addr));
            return ret;
        }

#endif
        netifapi_dhcp_release(netif);
        netifapi_dhcp_stop(netif);
        ret = -1;
        LOG("dhcp try");
    }

    if (ret < 0) {
#if LWIP_IPV4
        netifapi_netif_set_addr(netif, ip_2_ip4(&ip4addr), ip_2_ip4(&netif->netmask), ip_2_ip4(&netif->gw));
#endif
        LOG("dhcp err");
    }

    return ret;
}

static int eth_init(hal_wifi_module_t *m)
{
    return 0;
};

static void eth_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    (void)m;

    memcpy(mac, pnetif->hwaddr, pnetif->hwaddr_len);
    printf("eth_get_mac_addr!!\n");

};

static int eth_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    (void)init_para;

    hal_wifi_ip_stat_t ip_stat;

    pnetif = netif_find("en0");

    if (pnetif != NULL) {
        printf("eth_start success!!\n");
    }

    dhcpc_connect(pnetif);

    memcpy(ip_stat.ip, ipaddr_ntoa(&pnetif->ip_addr), 16);
    memcpy(ip_stat.mask, ipaddr_ntoa(&pnetif->netmask), 16);
    memcpy(ip_stat.gate, ipaddr_ntoa(&pnetif->gw), 16);
    memcpy(ip_stat.mac, pnetif->hwaddr, pnetif->hwaddr_len);

    if (m->ev_cb->stat_chg != NULL) {
        m->ev_cb->stat_chg(m, NOTIFY_STATION_UP, NULL);
    }

    if (m->ev_cb->ip_got != NULL) {
        m->ev_cb->ip_got(m, &ip_stat, NULL);
    }

    return 0;
}

static int eth_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    (void)init_para_adv;

    return 0;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    (void)wifi_type;

    dhcpc_connect(pnetif);

    memcpy(out_net_para->ip, ipaddr_ntoa(&pnetif->ip_addr), 16);
    memcpy(out_net_para->mask, ipaddr_ntoa(&pnetif->netmask), 16);
    memcpy(out_net_para->gate, ipaddr_ntoa(&pnetif->gw), 16);
    memcpy(out_net_para->mac, pnetif->hwaddr, pnetif->hwaddr_len);

    return 0;
}

static int get_link_stat(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat)
{
    return yoc_net_get_link_status();
}

static void start_scan(hal_wifi_module_t *m)
{

}

static void start_scan_adv(hal_wifi_module_t *m)
{
}

static int power_off(hal_wifi_module_t *m)
{
    return 0;
}

static int power_on(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_station(hal_wifi_module_t *m)
{
    return 0;
}

static int suspend_soft_ap(hal_wifi_module_t *m)
{

    return 0;
}

static int set_channel(hal_wifi_module_t *m, int ch)
{
    return 0;
}

static void start_monitor(hal_wifi_module_t *m)
{

}

static void stop_monitor(hal_wifi_module_t *m)
{

}

static void register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{

}

static void register_wlan_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{

}

static int wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return 0;
}

hal_wifi_module_t hobbit_eth_enc28j60 = {
    .base.name           = "csky_hobbit_eth",
    .init                =  eth_init,
    .get_mac_addr        =  eth_get_mac_addr,
    .start               =  eth_start,
    .start_adv           =  eth_start_adv,
    .get_ip_stat         =  get_ip_stat,
    .get_link_stat       =  get_link_stat,
    .start_scan          =  start_scan,
    .start_scan_adv      =  start_scan_adv,
    .power_off           =  power_off,
    .power_on            =  power_on,
    .suspend             =  suspend,
    .suspend_station     =  suspend_station,
    .suspend_soft_ap     =  suspend_soft_ap,
    .set_channel         =  set_channel,
    .start_monitor       =  start_monitor,
    .stop_monitor        =  stop_monitor,
    .register_monitor_cb =  register_monitor_cb,
    .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
    .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame
};

