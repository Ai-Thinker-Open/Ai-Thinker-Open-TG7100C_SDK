#ifndef _MAC_CONFIG_H_
#define _MAC_CONFIG_H_

#define CONFIG_ROLE_NULL        0
#define CONFIG_ROLE_AP          1
#define CONFIG_ROLE_STA         2
#define CONFIG_ROLE_COEXIST     3

extern uint8_t system_mac[];

void cfg_load_mac(u8 *mac);
uint32_t cfg_param_init(void);

void wifi_get_mac_address(char *mac, u8 type);
int wifi_set_mac_address(char *mac);
int wifi_set_mac_address_to_efuse(UINT8 *mac);
int wifi_get_mac_address_from_efuse(UINT8 *mac);

int wifi_write_efuse(UINT8 addr, UINT8 data);
UINT8 wifi_read_efuse(UINT8 addr);

#endif /*_MAC_CONFIG_H_*/
