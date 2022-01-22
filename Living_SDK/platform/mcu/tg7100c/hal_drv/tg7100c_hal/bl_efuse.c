#include "tg7100c_glb.h"
#include <tg7100c_ef_ctrl.h>
#include "tg7100c_mfg_media.h"
#include "bl_efuse.h"

int bl_efuse_read_mac(uint8_t mac[6])
{
    EF_Ctrl_Read_MAC_Address(mac);
    return 0;
}

int bl_efuse_read_mac_factory(uint8_t mac[6])
{
    if (0 == mfg_media_read_macaddr_with_lock(mac, 1)) {
        return 0;
    }
    return -1;
}


int bl_efuse_read_capcode(uint8_t *capcode)
{
    if (0 == mfg_media_read_xtal_capcode_need_lock(capcode, 1)) {
        return 0;
    }
    return -1;
}

int bl_efuse_read_pwroft(int8_t poweroffset[14])
{
    if (0 == mfg_media_read_poweroffset_need_lock(poweroffset, 1)) {
        return 0;
    }
    return -1;
}

int bl_efuse_ctrl_program_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Program_Direct_R0(index, data, len);

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);

    return 0;
}

int bl_efuse_ctrl_read_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Read_Direct_R0(index, data, len);

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);

    return 0;
}

int bl_efuse_read_mac_opt(uint8_t slot, uint8_t mac[6], uint8_t reload)
{
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Read_MAC_Address_Opt(slot, mac, reload);

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);

    return 0;
}


#include "regex.h"
#include "iot_export.h"
#include "bl_flash.h"
static int is_valid_mac(char *mac)
{
    if(strlen(mac) != 12){
        printf("not valied MAC\r\n");
        return -1;
    }
    char *p = mac;
    while(*p != '\0')
    {
        if(*p < '0' || *p > 'f'){
            return -1;
        }
        if(*p >= '0' && *p <= '9')        {}
        else if(*p >= 'A' && *p <= 'F')   {}
        else if(*p >= 'a' && *p <= 'f')   {}
        else{
            return -1;
        }
        p++;
    }
    return 0;
}
/*************************************************
Function: 		StringToHex
Description: 	字符串转HEX字符串
Input: 			pAscStr:原字符数组
                len:字符串长度
Output: 		pHexStr:输出的字符串              
Return: 		Hex字符串长度
*************************************************/
static int StringToHex(const char *pAscStr,unsigned char *pHexStr,int Len)
{
    int i = 0;
    int j = 0;
    if(Len % 2==0){
    }
    for(i=0;i<Len;i++)
    {
        if( ( pAscStr[i] >= '0' ) && ( pAscStr[i] <= '9' ) ){
            pHexStr[j] =  pAscStr[i] - '0';
        }
        else if( ( pAscStr[i] >= 'A' ) && ( pAscStr[i] <= 'F' ) ){
            pHexStr[j] = pAscStr[i] - '7';
        }
        else if( ( pAscStr[i] >= 'a' ) && ( pAscStr[i] <= 'f' ) ){
            pHexStr[j] = pAscStr[i] - 0x57;
        }
        else{
            pHexStr[j] = 0xff;
            return 0;
        }
        i++;
        if(i>=Len){
            return j+1;
        }
        if( ( pAscStr[i] >= '0' ) && ( pAscStr[i] <= '9' ) ){
            pHexStr[j] =pHexStr[j]*0x10+  pAscStr[i] - '0';
            // printf("%x\n",pHexStr[j]);
        }
        else if( ( pAscStr[i] >= 'A' ) && ( pAscStr[i] <= 'F' ) ){
            pHexStr[j] =pHexStr[j]*0x10+  pAscStr[i] - '7';
            // printf("%x\n",pHexStr[j]);
        }
        else if( ( pAscStr[i] >= 'a' ) && ( pAscStr[i] <= 'f' ) ){
            pHexStr[j] = pHexStr[j]*0x10+ pAscStr[i] - 0x57;
            // printf("%x\n",pHexStr[j]);
        }
        else{
            pHexStr[j] = 0xff;
            printf("%x\n",pHexStr[j]);
            return 0;
        }
        j++;
        
    }
    
    return j;
}
char* bl_flash_efuse_read_mac(uint8_t mac[6])
{
    char device_name[DEVICE_NAME_LEN + 1] = { 0 };
    int len = DEVICE_NAME_LEN + 1;
    aos_kv_get(KV_KEY_DN, device_name, &len);
    printf("read devicename is %s\n",device_name);
    int ret = is_valid_mac(device_name);
    
    if( ret == 0){      //flash MAC有效
        StringToHex(device_name, mac, sizeof(device_name));
        printf("Flash MAC :%02X:%02X:%02X:%02X:%02X:%02X\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    }
    else if(ret == -1){ //flash MAC无效，使用Efuse MAC
        bl_efuse_read_mac_factory(mac);
        printf("Efuse MAC:%02X:%02X:%02X:%02X:%02X:%02X\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    }
    return device_name;
}