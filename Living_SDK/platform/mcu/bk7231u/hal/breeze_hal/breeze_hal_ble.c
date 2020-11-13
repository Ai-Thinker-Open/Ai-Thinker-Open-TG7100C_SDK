#include "ble_pub.h"
#include "ble_api.h"
#include <stddef.h>
#include <stdint.h>
#include "breeze_hal_ble.h"

extern struct bd_addr common_default_bdaddr;
static ais_bt_init_t ais_bt_init_info;
static void (*g_indication_txdone)(uint8_t res);
breeze_config_t breeze_cfg; 
uint8_t g_connected = 0;

#define BK_ATT_DECL_PRIMARY_SERVICE     {0x00,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DECL_CHARACTERISTIC      {0x03,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DESC_CLIENT_CHAR_CFG     {0x02,0x29,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

#define BK_ATT_FEB3S_IDX_FED4_VAL_VALUE {0xD4,0xFE,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_FEB3S_IDX_FED5_VAL_VALUE {0xD5,0xFE,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_FEB3S_IDX_FED6_VAL_VALUE {0xD6,0xFE,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_FEB3S_IDX_FED7_VAL_VALUE {0xD7,0xFE,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_FEB3S_IDX_FED8_VAL_VALUE {0xD8,0xFE,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

uint8_t uuid_feb3[16] = {0xB3,0xFE,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const bk_attm_desc_t feb3_att_db[FEB3S_IDX_NB] =
{
	// FEB3 Service Declaration
	[FEB3S_IDX_SVC]                  =   {BK_ATT_DECL_PRIMARY_SERVICE, BK_PERM_SET(RD, ENABLE), 0, 0},
	// fed4  Characteristic Declaration
	[FEB3S_IDX_FED4_VAL_CHAR]        =   {BK_ATT_DECL_CHARACTERISTIC, BK_PERM_SET(RD, ENABLE), 0, 0},
	// fed4  Characteristic Value
	[FEB3S_IDX_FED4_VAL_VALUE]       =   {BK_ATT_FEB3S_IDX_FED4_VAL_VALUE, BK_PERM_SET(RD, ENABLE), BK_PERM_SET(RI, ENABLE), FEB3_CHAR_DATA_LEN},
	
	// fed5  Characteristic Declaration
	[FEB3S_IDX_FED5_VAL_CHAR]        =   {BK_ATT_DECL_CHARACTERISTIC, BK_PERM_SET(RD, ENABLE), 0, 0},
	// fed5  Characteristic Value
	[FEB3S_IDX_FED5_VAL_VALUE]       =   {BK_ATT_FEB3S_IDX_FED5_VAL_VALUE, BK_PERM_SET(WRITE_REQ, ENABLE)|BK_PERM_SET(RD, ENABLE), BK_PERM_SET(RI, ENABLE), FEB3_CHAR_DATA_LEN},
	
	// fed6  Characteristic Declaration
	[FEB3S_IDX_FED6_VAL_CHAR]        =   {BK_ATT_DECL_CHARACTERISTIC, BK_PERM_SET(RD, ENABLE), 0, 0},
	// fed6  Characteristic Value
	[FEB3S_IDX_FED6_VAL_VALUE]       =   {BK_ATT_FEB3S_IDX_FED6_VAL_VALUE, BK_PERM_SET(IND, ENABLE)|BK_PERM_SET(RD, ENABLE), BK_PERM_SET(RI, ENABLE), FEB3_CHAR_DATA_LEN},
	[FFB3S_IDX_FED6_VAL_IND_CFG]     =   {BK_ATT_DESC_CLIENT_CHAR_CFG, BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE), 0, 0},
	
	// fed7  Characteristic Declaration
	[FEB3S_IDX_FED7_VAL_CHAR]        =   {BK_ATT_DECL_CHARACTERISTIC, BK_PERM_SET(RD, ENABLE), 0, 0},
	// fed7  Characteristic Value
	[FEB3S_IDX_FED7_VAL_VALUE]       =   {BK_ATT_FEB3S_IDX_FED7_VAL_VALUE, BK_PERM_SET(WRITE_COMMAND, ENABLE)|BK_PERM_SET(RD, ENABLE), BK_PERM_SET(RI, ENABLE), FEB3_CHAR_DATA_LEN},
	
	// fed8  Characteristic Declaration
	[FEB3S_IDX_FED8_VAL_CHAR]        =   {BK_ATT_DECL_CHARACTERISTIC, BK_PERM_SET(RD, ENABLE), 0, 0},
	// fed8  Characteristic Value
	[FEB3S_IDX_FED8_VAL_VALUE]       =   {BK_ATT_FEB3S_IDX_FED8_VAL_VALUE, BK_PERM_SET(NTF, ENABLE)|BK_PERM_SET(RD, ENABLE), BK_PERM_SET(RI, ENABLE), FEB3_CHAR_DATA_LEN},
	[FFB3S_IDX_FED8_VAL_NTF_CFG]     =   {BK_ATT_DESC_CLIENT_CHAR_CFG, BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE), 0, 0},
};/// Macro used to retrieve permission value from access and rights on attribute.

static ble_err_t ble_create_db(void)
{
    ble_err_t status;
    struct bk_ble_db_cfg ble_db_cfg;

    ble_db_cfg.att_db = feb3_att_db;
    ble_db_cfg.att_db_nb = FEB3S_IDX_NB;
    ble_db_cfg.prf_task_id = 0;
    ble_db_cfg.start_hdl = 0;
    ble_db_cfg.svc_perm = 0;
    memcpy(&(ble_db_cfg.uuid[0]), &uuid_feb3[0], 16);

    status = bk_ble_create_db(&ble_db_cfg);

	return status;
}

void ble_event_callback(ble_event_t event, void *param)
{
	switch(event)	  
	{ 	   
		case BLE_STACK_OK:		 
			{	
				bk_printf("BLE_STACK_OK\r\n");
				ble_create_db();
			}		  
			break;		
		case BLE_STACK_FAIL:		
			{	
				bk_printf("BLE_STACK_FAIL\r\n");
			} 	   
			break;
		case BLE_CREATE_DB_OK:
			{
				bk_printf("BLE_CREATE_DB_OK\r\n");			
			} 	   
			break;		 
		case BLE_CONNECT:		  
			{ 		   
				bk_printf("BLE CONNECT\r\n");			
				g_connected = 1;
				ais_bt_init_info.on_connected();	
			}
			break; 	   
		case BLE_DISCONNECT: 	   
			{			
				bk_printf("BLE DISCONNECT\r\n");			
				g_connected = 0;
				ais_bt_init_info.on_disconnected(); 	   
			}		
			break;		  
		case BLE_MTU_CHANGE:		  
			{ 		   
				bk_printf("BLE_MTU_CHANGE:%d\r\n", *(uint16_t *)param);		  
			} 	   
			break;		 
		case BLE_CFG_NOTIFY:		 
			{			  
				bk_printf("BLE_CFG_NTFIND:0x%x\r\n", *(uint8_t *)param);			  
				ais_bt_init_info.nc.on_ccc_change(*(uint8_t *)param); 	   
			}		
			break;		  
		case BLE_CFG_INDICATE:		
			{			 
				bk_printf("BLE_CFG_NTFIND:0x%x\r\n", *(uint8_t *)param);			 
				ais_bt_init_info.ic.on_ccc_change(*(uint8_t *)param);		  
			} 	   
			break;	
		case BLE_TX_DONE:
			{
				bk_printf("BLE_TX_DONE\r\n");
				if(g_indication_txdone)
				{
					(*g_indication_txdone)(AIS_ERR_SUCCESS);
				}
			}
			break;
		case BLE_START_ADV_SUCCESS:
			{
				bk_printf("BLE_START_ADV_SUCCESS");
			}
			break;
		case BLE_START_ADV_FAIL:
			{
				bk_printf("BLE_START_ADV_FAIL");
			}
			break;
		case BLE_STOP_ADV:
			{
				bk_printf("BLE_STOP_ADV");
			}
			break;
		default:			 
			bk_printf("UNKNOW EVENT\r\n"); 	   
			break;	 
	}
}

void ble_write_callback(write_req_t *param)
{	 
	if (param->att_idx == FFB3S_IDX_FED6_VAL_IND_CFG)
	{
		uint16_t ind_cfg = (param->value[0]) | (param->value[1] << 8);
		breeze_cfg.ind_cfg = ind_cfg;
		ble_event_callback(BLE_CFG_INDICATE, (void *)(&(breeze_cfg.ind_cfg)));
	}
	else if (param->att_idx == FFB3S_IDX_FED8_VAL_NTF_CFG)
	{
		uint16_t ntf_cfg = (param->value[0]) | (param->value[1] << 8);
		breeze_cfg.ntf_cfg = ntf_cfg;
		ble_event_callback(BLE_CFG_NOTIFY, (void *)(&(breeze_cfg.ntf_cfg)));
	}
	else if (param->att_idx == FEB3S_IDX_FED5_VAL_VALUE)
	{
		ais_bt_init_info.wc.on_write(param->value, param->len);
	}
	else if (param->att_idx == FEB3S_IDX_FED7_VAL_VALUE)
	{
		ais_bt_init_info.wwnrc.on_write(param->value, param->len);
	}
	else
	{
		bk_printf("Write Not support\r\n");
	}
}

uint8_t ble_read_callback(read_req_t *param)
{
	uint8_t len = 0;
	
	if (param->att_idx == FFB3S_IDX_FED6_VAL_IND_CFG)
	{
		param->value = breeze_cfg.ind_cfg;
		len = 2;
	}
	else if (param->att_idx == FFB3S_IDX_FED8_VAL_NTF_CFG)
	{
		param->value = breeze_cfg.ntf_cfg;
		len = 2;
	}
	else if (param->att_idx == FEB3S_IDX_FED4_VAL_VALUE)
	{
		len = ais_bt_init_info.rc.on_read(param->value, param->size);
	}
	else
	{
		bk_printf("Read Not Support\r\n");
	}

	return len;
}

ais_err_t ble_stack_init(ais_bt_init_t *ais_init)
{
    memcpy((uint8_t *)&ais_bt_init_info,(uint8_t *)ais_init,sizeof(ais_bt_init_t));
    memset(&breeze_cfg, 0x0, sizeof(breeze_config_t));

    ble_set_write_cb(ble_write_callback);
    ble_set_event_cb(ble_event_callback);
    ble_set_read_cb(ble_read_callback);

	ble_activate(NULL);
	return AIS_ERR_SUCCESS;
}

ais_err_t ble_stack_deinit()
{
    memset((uint8_t *)&ais_bt_init_info,0,sizeof(ais_bt_init_t));

    ble_send_msg(BLE_MSG_EXIT);
    return AIS_ERR_SUCCESS;
}

ais_err_t ble_send_notification(uint8_t *p_data, uint16_t length)
{
    ais_err_t status = AIS_ERR_SUCCESS;
	
    status = bk_ble_send_ntf_value(length, p_data, 0, FEB3S_IDX_FED8_VAL_VALUE);

    bk_printf("status:%d\r\n", status);
	
    return status;
}

ais_err_t ble_send_indication(uint8_t *p_data, uint16_t length, void (*txdone)(uint8_t res))
{
    ais_err_t status = AIS_ERR_SUCCESS;

    status = bk_ble_send_ind_value(length, p_data, 0, FEB3S_IDX_FED6_VAL_VALUE);
	
	g_indication_txdone = txdone;
	
    return status;
}

void ble_disconnect(uint8_t reason)
{
    bk_printf("ble_disconnect\r\n");
    appm_disconnect();
}

ais_err_t ble_advertising_start(ais_adv_init_t *adv)
{
    ais_err_t status = AIS_ERR_SUCCESS;
    uint8_t adv_idx, adv_name_len;
    
    adv_idx = 0;
    adv_info.advData[adv_idx] = 0x02; adv_idx++;
    adv_info.advData[adv_idx] = 0x01; adv_idx++;
    adv_info.advData[adv_idx] = 0x06; adv_idx++;

    adv_info.advData[adv_idx] = adv->vdata.len + 1; adv_idx +=1;
    adv_info.advData[adv_idx] = 0xFF; adv_idx +=1;
    memcpy(&adv_info.advData[adv_idx], adv->vdata.data, adv->vdata.len); adv_idx += adv->vdata.len;

    adv_name_len = strlen(adv->name.name) + 1;
    adv_info.advData[adv_idx] = adv_name_len; adv_idx +=1;
    adv_info.advData[adv_idx] = adv->name.ntype + 0x08; adv_idx +=1; //name
    memcpy(&adv_info.advData[adv_idx], adv->name.name, strlen(adv->name.name)); adv_idx +=strlen(adv->name.name);

    adv_info.advDataLen = adv_idx;

    adv_idx = 0;
        
    adv_info.respData[adv_idx] = adv_name_len; adv_idx +=1;
    adv_info.respData[adv_idx] = adv->name.ntype + 0x07; adv_idx +=1; //name
    memcpy(&adv_info.respData[adv_idx], adv->name.name, strlen(adv->name.name)); adv_idx +=strlen(adv->name.name);
    adv_info.respDataLen = adv_idx;
    
    status = appm_start_advertising();

    return status;
}

ais_err_t ble_advertising_stop()
{
    bk_printf("%s\r\n", __func__);
    ais_err_t status = AIS_ERR_SUCCESS;
    
    status = appm_stop_advertising();

    return status;
}

ais_err_t ble_get_mac(uint8_t *mac)
{
    bk_printf("%s\r\n", __func__);
    ais_err_t status = AIS_ERR_SUCCESS;
    
    memcpy(mac, &common_default_bdaddr, AIS_BT_MAC_LEN);

    return status;
}

#ifdef EN_LONG_MTU
int ble_get_att_mtu(uint16_t *att_mtu)
{
    if(att_mtu == NULL || g_connected == 0){
        bk_printf("Failed to get ble connection\r\n");
        return -1;
    }
    *att_mtu = gattc_get_mtu(0);
    return 0;
}
#endif
