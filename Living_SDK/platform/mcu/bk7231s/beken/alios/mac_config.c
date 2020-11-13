#include "bk7011_cal_pub.h"
#include "mac_config.h"
#include "drv_model_pub.h"
#include "uart_pub.h"
#include "net_param_pub.h"

#define MAC_EFUSE           0
#define MAC_ITEM            1
#define MAC_RF_OTP_FLASH    2
#define WIFI_MAC_POS        MAC_ITEM

#if ((CFG_SOC_NAME == SOC_BK7231) && (WIFI_MAC_POS == MAC_EFUSE))
#error "BK7231 not support efuse!"
#endif

#define DEFAULT_MAC_ADDR "\xC8\x47\x8C\x00\x00\x18"
uint8_t system_mac[] = DEFAULT_MAC_ADDR;

void cfg_load_mac(u8 *mac)
{
#if (WIFI_MAC_POS == MAC_EFUSE)
    if(!wifi_get_mac_address_from_efuse((UINT8 *)mac))
#elif (WIFI_MAC_POS == MAC_RF_OTP_FLASH)
    if(!manual_cal_get_macaddr_from_flash((UINT8 *)mac))
#elif (WIFI_MAC_POS == MAC_ITEM)
    if(!get_info_item(WIFI_MAC_ITEM, (UINT8 *)mac, NULL, NULL))
#endif
    {
        os_memcpy(mac, DEFAULT_MAC_ADDR, 6);
        if(mac[0] & 0x01)
        {
            os_printf("cfg_load_mac failed, MAC[0]&0x1 == 1\r\n");
        }
    }
}

void wifi_get_mac_address(char *mac, u8 type)
{
    static int mac_inited = 0;

    if (mac_inited == 0)
    {
        cfg_load_mac(system_mac);
        mac_inited = 1;
    }

    if(type == CONFIG_ROLE_AP)
    {
        u8 mac_mask = (0xff & (2/*NX_VIRT_DEV_MAX*/ - 1));
        u8 mac_low;

        os_memcpy(mac, system_mac, 6);
        mac_low = mac[5];

        // if  NX_VIRT_DEV_MAX == 4.
        // if support AP+STA, mac addr should be equal with each other in byte0-4 & byte5[7:2],
        // byte5[1:0] can be different
        // ie: mac[5]= 0xf7,  so mac[5] can be 0xf4, f5, f6. here wre chose 0xf4
        mac[5] &= ~mac_mask;
        mac_low = ((mac_low & mac_mask) ^ mac_mask );
        mac[5] |= mac_low;
    }
    else if(type == CONFIG_ROLE_STA)
    {
        os_memcpy(mac, system_mac, 6);
    }
}

uint8_t wifi_get_vif_index_by_mac(char *mac)
{
    return rwm_mgmt_vif_mac2idx(mac);
}

int wifi_set_mac_address(char *mac)
{

    if(mac[0]&0x01)
    {
        os_printf("set failed,can be a bc/mc address\r\n");
        return 0;
    }

   os_memcpy(system_mac, mac, 6);

#if (WIFI_MAC_POS == MAC_EFUSE)
    //wifi_set_mac_address_to_efuse((UINT8 *)system_mac);
#elif (WIFI_MAC_POS == MAC_RF_OTP_FLASH)
    manual_cal_write_macaddr_to_flash((UINT8 *)system_mac);
#elif (WIFI_MAC_POS == MAC_ITEM)
    save_info_item(WIFI_MAC_ITEM, (UINT8 *)system_mac, NULL, NULL);
#endif

    return 0;
}

#if (CFG_SOC_NAME != SOC_BK7231)
#include "sys_ctrl_pub.h"
int wifi_set_mac_address_to_efuse(UINT8 *mac)
{
    EFUSE_OPER_ST efuse;
    int i = 0, ret;

    if(!mac)
        return 0;
    
    for(i=0; i<EFUSE_MAC_LEN; i++) {
        efuse.addr = EFUSE_MAC_START_ADDR + i;
        efuse.data = mac[i];

        if(i == 0) {
            // ensure mac[0]-bit0 in efuse not '1'
            efuse.data &= ~(0x01);
        }
        
        ret = sddev_control(SCTRL_DEV_NAME, CMD_EFUSE_WRITE_BYTE, &efuse);
        if(ret != 0) {
            os_printf("efuse set MAC failed\r\n");
            return 0;
        } 
    }

    os_printf("efuse set MAC ok\r\n");
    return 1;
}

int wifi_get_mac_address_from_efuse(UINT8 *mac)
{
    EFUSE_OPER_ST efuse;
    int i = 0, ret;
    
    if(!mac)
        return 0;
    
    for(i=0; i<EFUSE_MAC_LEN; i++) {
        efuse.addr = EFUSE_MAC_START_ADDR + i;
        efuse.data = 0;
        
        ret = sddev_control(SCTRL_DEV_NAME, CMD_EFUSE_READ_BYTE, &efuse);
        if(ret == 0) {
            mac[i] = efuse.data;
        } else {
            mac[i] = 0;
            os_printf("efuse get MAC -1\r\n");
            return 0;
        }
    }

    os_printf("efuse get MAC:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if((mac[0] == 0) && (mac[1] == 0) && (mac[2] == 0) && 
        (mac[3] == 0) && (mac[4] == 0) && (mac[5] == 0)){
        os_printf("efuse MAC all zero, see as error\r\n");
        return 0;
    }

    return 1;
}

int wifi_write_efuse(UINT8 addr, UINT8 data)
{
    EFUSE_OPER_ST efuse;
    int i = 0, ret;

    if(addr > EFUSE_CTRL_ADDR) {
        os_printf("efuse addr:0x%x out of range(0-0x1F)\r\n", addr);
        return 0;
    }
    
    efuse.addr = addr;
    efuse.data = data;
        
    ret = sddev_control(SCTRL_DEV_NAME, CMD_EFUSE_WRITE_BYTE, &efuse);
    if(ret != 0) {
        os_printf("efuse write failed, aready write this addr 0x%x\r\n", addr);
        return 0;
    } 

    return 1;
}

UINT8 wifi_read_efuse(UINT8 addr)
{
    EFUSE_OPER_ST efuse;
    int i = 0, ret;

    if(addr > EFUSE_CTRL_ADDR) {
        os_printf("efuse addr:0x%x out of range(0-0x1F)\r\n", addr);
        return 0;
    }
    
    efuse.addr = addr;
    efuse.data = 0;
        
    ret = sddev_control(SCTRL_DEV_NAME, CMD_EFUSE_READ_BYTE, &efuse);
    if(ret == 0) {
        return efuse.data;
    } else {
        os_printf("efuse get MAC -1\r\n");
        return 0xff;
    }
}
#endif // #if (CFG_SOC_NAME != SOC_BK7231)

