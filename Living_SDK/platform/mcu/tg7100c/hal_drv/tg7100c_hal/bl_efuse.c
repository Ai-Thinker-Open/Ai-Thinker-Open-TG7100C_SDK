#include "tg7100c_glb.h"
#include <tg7100c_ef_ctrl.h>
#include "bl_efuse.h"

int bl_efuse_read_mac(uint8_t mac[6])
{
    EF_Ctrl_Read_MAC_Address(mac);
    return 0;
}

int bl_efuse_read_mac_factory(uint8_t mac[6])
{
    if (0 == mfg_media_read_macaddr(mac, 1)) {
        return 0;
    }
    return -1;
}


int bl_efuse_read_capcode(uint8_t *capcode)
{
    if (0 == mfg_media_read_xtal_capcode(capcode, 1)) {
        return 0;
    }
    return -1;
}

int bl_efuse_read_pwroft(int8_t poweroffset[14])
{
    if (0 == mfg_media_read_poweroffset(poweroffset, 1)) {
        return 0;
    }
    return -1;
}


// #include <sys/types.h>
#include "regex.h"
#include "iot_export.h"
#include "bl_flash.h"

//检查MAC地址是否合规,使用正则表达式检查
// static int is_valid_mac(char *mac)
// {
//     char * pattern = "^([A-Fa-f0-9]{2}){5}[A-Fa-f0-9]{2}$";
//     int status;
//     char ebuf[128];
//     const int cflags = 0;
//     regmatch_t pmatch[10];
//     const size_t nmatch = 10;
//     regex_t reg;


//     status = regcomp(&reg, pattern, cflags);//编译正则模式
// 	if(status != 0) {
// 		regerror(status, &reg, ebuf, sizeof(ebuf));
// 		fprintf(stderr, "regcomp fail: %s , pattern '%s' \n",ebuf, pattern);
// 		goto failed;
// 	}

// 	status = regexec(&reg, mac, nmatch, pmatch,0);//执行正则表达式和缓存的比较,
// 	if(status != 0) {
// 		regerror(status, &reg, ebuf, sizeof(ebuf));
// 		fprintf(stderr, "regexec fail: %s , mac:\"%s\" \n", ebuf, mac);
// 		goto failed;
// 	}
 
// 	printf("[%s] match success.\n", __FUNCTION__);
// 	regfree(&reg);
// 	return 0;
 
// failed:
// 	regfree(&reg);
// 	return -1;
// }

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
}//change by johhn

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
