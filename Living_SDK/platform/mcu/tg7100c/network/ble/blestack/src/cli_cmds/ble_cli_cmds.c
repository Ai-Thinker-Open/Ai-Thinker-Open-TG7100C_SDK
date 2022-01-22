#include <stdlib.h>
#include "conn.h"
#include "gatt.h"
#include "hci_core.h"
#include <aos/kernel.h>
#include <k_api.h>
#include <aos/cli.h>

#include "ble_cli_cmds.h"

#define 		PASSKEY_MAX  		0xF423F
#define 		NAME_LEN 			30
#define 		CHAR_SIZE_MAX       512

static u8_t 	selected_id = BT_ID_DEFAULT;
bool 			ble_inited 	= false;
#if defined(CONFIG_BT_CONN)
struct bt_conn *default_conn = NULL;
#endif

struct bt_data ad_discov[2] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    #if defined(HOST_TG7100C)
    BT_DATA(BT_DATA_NAME_COMPLETE, "TG7100C-BLE-DEV", 13),
    #else
    BT_DATA(BT_DATA_NAME_COMPLETE, "BL70X-BLE-DEV", 13),
    #endif
};

#define vOutputString(...)  printf(__VA_ARGS__)

static void ble_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_get_device_name(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_set_device_name(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#if defined(CONFIG_BT_OBSERVER)
static void ble_start_scan(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_stop_scan(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif
static void ble_read_local_address(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_set_adv_channel(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_start_advertise(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_stop_advertise(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#if defined(CONFIG_BT_CENTRAL)
static void ble_connect(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_disconnect(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif
static void ble_select_conn(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_unpair(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_conn_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#if defined(CONFIG_BT_SMP)
static void ble_security(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_auth(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_auth_cancel(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_auth_passkey_confirm(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_auth_pairing_confirm(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_auth_passkey(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif
static void ble_exchange_mtu(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_discover(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_read(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_write(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_write_without_rsp(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_subscribe(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_unsubscribe(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_set_data_len(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void ble_get_all_conn_info(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);

#if defined(CONFIG_SET_TX_PWR)
static void ble_set_tx_pwr(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif

#ifndef STATIC_CLI_CMD_ATTRIBUTE 
#define STATIC_CLI_CMD_ATTRIBUTE 
#endif
const struct cli_command btStackCmdSet[] STATIC_CLI_CMD_ATTRIBUTE = {
#if 0
    /*1.The cmd string to type, 2.Cmd description, 3.The function to run, 4.Number of parameters*/

    {"ble_init", "\r\nble_init:[Initialize]\r\n Parameter[Null]\r\n", ble_init},

    {"ble_get_device_name", "\r\nble_get_device_name:[Read local device name]\r\n Parameter[Null]\r\n", ble_get_device_name},

    {"ble_set_device_name", "\r\nble_set_device_name:\r\n\[Lenth of name]\r\n\[name]\r\n", ble_set_device_name},


#if defined(CONFIG_BT_OBSERVER)
    {"ble_start_scan", "\r\nble_start_scan:\r\n\
    [Scan type, 0:passive scan, 1:active scan]\r\n\
    [Duplicate filtering, 0:Disable duplicate filtering, 1:Enable duplicate filtering]\r\n\
    [Scan interval, 0x0004-4000,e.g.0080]\r\n\
    [Scan window, 0x0004-4000,e.g.0050]\r\n", ble_start_scan},

    {"ble_stop_scan", "\r\nble_stop_scan:[Stop scan]\r\nParameter[Null]\r\n", ble_stop_scan},
#endif

#if defined(CONFIG_BT_PERIPHERAL)
    {"ble_start_adv", "\r\nble_start_adv:\r\n\
    [Adv type,0:adv_ind,1:adv_scan_ind,2:adv_nonconn_ind]\r\n\
    [Mode, 0:discov, 1:non-discov]\r\n\
    [Adv Interval Min,0x0020-4000,e.g.0030]\r\n\
    [Adv Interval Max,0x0020-4000,e.g.0060]\r\n", ble_start_advertise},
     
    {"ble_stop_adv", "\r\nble_stop_adv:[Stop advertising]\r\nParameter[Null]\r\n", ble_stop_advertise},

    {"ble_read_local_address", "\r\nble_read_local_address:[Read local address]\r\n", ble_read_local_address},
#endif

#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
    {"ble_connect", "\r\nble_connect:[Connect remote device]\r\n\
     [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
     [Address value, e.g.112233AABBCC]\r\n", ble_connect},
#endif //CONFIG_BT_CENTRAL
    
    {"ble_disconnect", "\r\nble_disconnect:[Disconnect remote device]\r\n\
    [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value,e.g.112233AABBCC]\r\n", ble_disconnect},
	
    {"ble_select_conn", "\r\nble_select_conn:[Select a specific connection]\r\n\
    [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value, e.g.112233AABBCC]\r\n", ble_select_conn},
     
    {"ble_unpair", "\r\nble_unpair:[Unpair connection]\r\n\
    [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value, all 0: unpair all connection, otherwise:unpair specific connection]\r\n", ble_unpair},

    {"ble_conn_update", "\r\nble_conn_update:\r\n\
    [Conn Interval Min,0x0006-0C80,e.g.0030]\r\n\
    [Conn Interval Max,0x0006-0C80,e.g.0030]\r\n\
    [Conn Latency,0x0000-01f3,e.g.0004]\r\n\
    [Supervision Timeout,0x000A-0C80,e.g.0010]\r\n", ble_conn_update},
#endif //CONFIG_BT_CONN
 
#if defined(CONFIG_BT_SMP)
    {"ble_security", "\r\nble_security:[Start security]\r\n\
    [Security level, Default value 4, 2:BT_SECURITY_MEDIUM, 3:BT_SECURITY_HIGH, 4:BT_SECURITY_FIPS]\r\n", ble_security},

    {"ble_auth", "\r\nble_auth:[Register auth callback]\r\n", ble_auth},

    {"ble_auth_cancel", "\r\nble_auth_cancel:[Register auth callback]\r\n", ble_auth_cancel},

    {"ble_auth_passkey_confirm", "\r\nble_auth_passkey_confirm:[Confirm passkey]\r\n", ble_auth_passkey_confirm},

    {"ble_auth_pairing_confirm", "\r\nble_auth_pairing_confirm:[Confirm pairing in secure connection]\r\n", ble_auth_pairing_confirm},

    {"ble_auth_passkey", "\r\nble_auth_passkey:[Input passkey]\r\n[Passkey, 00000000-000F423F]", ble_auth_passkey},

#endif //CONFIG_BT_SMP

#if defined(CONFIG_BT_GATT_CLIENT)
    {"ble_exchange_mtu", "\r\nble_exchange_mtu:[Exchange mtu]\r\n Parameter[Null]\r\n", ble_exchange_mtu},

    {"ble_discover", "\r\nble_discover:[Gatt discovery]\r\n\
    [Discovery type, 0:Primary, 1:Secondary, 2:Include, 3:Characteristic, 4:Descriptor]\r\n\
    [Uuid value, 2 Octets, e.g.1800]\r\n\
    [Start handle, 2 Octets, e.g.0001]\r\n\
    [End handle, 2 Octets, e.g.ffff]\r\n", ble_discover},

    {"ble_read", "\r\nble_read:[Gatt Read]\r\n\
    [Attribute handle, 2 Octets]\r\n\
    [Value offset, 2 Octets]\r\n", ble_read},

    {"ble_write", "\r\nble_write:[Gatt write]\r\n\
    [Attribute handle, 2 Octets]\r\n\
    [Value offset, 2 Octets]\r\n\
    [Value length, 2 Octets]\r\n\
    [Value data]\r\n", ble_write},

    {"ble_write_without_rsp", "\r\nble_write_without_rsp:[Gatt write without response]\r\n\
    [Sign, 0: No need signed, 1:Signed write cmd if no smp]\r\n\
    [Attribute handle, 2 Octets]\r\n\
    [Value length, 2 Octets]\r\n\
    [Value data]\r\n", ble_write_without_rsp},

    {"ble_subscribe", "\r\nble_subscribe:[Gatt subscribe]\r\n\
    [CCC handle, 2 Octets]\r\n\
    [Value handle, 2 Octets]\r\n\
    [Value, 1:notify, 2:indicate]\r\n", ble_subscribe},

    {"ble_unsubscribe", "\r\nble_unsubscribe:[Gatt unsubscribe]\r\n Parameter[Null]\r\n", ble_unsubscribe},
#endif /*CONFIG_BT_GATT_CLIENT*/

    {"ble_set_data_len",
    "\r\nble_set_data_len:[LE Set Data Length]\r\n\
    [tx octets, 2 octets]\r\n\
    [tx time, 2 octets]\r\n",
    ble_set_data_len},

    {"ble_conn_info", "\r\nble_conn_info:[LE get all connection devices info]\r\n", ble_get_all_conn_info},

#if defined(CONFIG_SET_TX_PWR)
    {"ble_set_tx_pwr",
    "\r\nble_set_tx_pwr:[Set tx power mode]\r\n\
    [mode, 1 octet, value:5,6,7]\r\n",
    ble_set_tx_pwr},
#endif

#else
    {"ble_init", "", ble_init},
    {"ble_get_device_name", "", ble_get_device_name},
    {"ble_set_device_name", "", ble_set_device_name},
#if defined(CONFIG_BT_OBSERVER)
    {"ble_start_scan", "", ble_start_scan},
    {"ble_stop_scan", "", ble_stop_scan},
#endif
#if defined(CONFIG_BT_PERIPHERAL)
    {"ble_set_adv_channel", "", ble_set_adv_channel},
    {"ble_start_adv", "", ble_start_advertise},
    {"ble_stop_adv", "", ble_stop_advertise},
    {"ble_read_local_address", "", ble_read_local_address},
#endif
#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
    {"ble_connect", "", ble_connect},
    {"ble_disconnect", "", ble_disconnect},
#endif //CONFIG_BT_CENTRAL
    {"ble_select_conn", "", ble_select_conn},
    {"ble_unpair", "", ble_unpair},
    {"ble_conn_update", "", ble_conn_update},
#endif //CONFIG_BT_CONN
#if defined(CONFIG_BT_SMP)
    {"ble_security", "", ble_security},
    {"ble_auth", "", ble_auth},
    {"ble_auth_cancel", "", ble_auth_cancel},
    {"ble_auth_passkey_confirm", "", ble_auth_passkey_confirm},
    {"ble_auth_pairing_confirm", "", ble_auth_pairing_confirm},
    {"ble_auth_passkey", "", ble_auth_passkey},
#endif //CONFIG_BT_SMP
#if defined(CONFIG_BT_GATT_CLIENT)
    {"ble_exchange_mtu", "", ble_exchange_mtu},
    {"ble_discover", "", ble_discover},
    {"ble_read", "", ble_read},
    {"ble_write", "", ble_write},
    {"ble_write_without_rsp", "", ble_write_without_rsp},
    {"ble_subscribe", "", ble_subscribe},
    {"ble_unsubscribe", "", ble_unsubscribe},
#endif /*CONFIG_BT_GATT_CLIENT*/
    {"ble_set_data_len", "", ble_set_data_len},
    {"ble_conn_info", "", ble_get_all_conn_info},
#if defined(CONFIG_SET_TX_PWR)
    {"ble_set_tx_pwr", "", ble_set_tx_pwr},
#endif
#endif
};

#if defined(CONFIG_BT_CONN)
static void connected(struct bt_conn *conn, u8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		vOutputString("Failed to connect to %s (%u) \r\n", addr,
			     err);
		return;
	}

	vOutputString("Connected: %s \r\n", addr);

	if (!default_conn) {
		default_conn = conn;
	}
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	vOutputString("Disconnected: %s (reason %u) \r\n", addr, reason);

	if (default_conn == conn) {
		default_conn = NULL;
	}
}

static void le_param_updated(struct bt_conn *conn, u16_t interval,
			     u16_t latency, u16_t timeout)
{
	vOutputString("LE conn param updated: int 0x%04x lat %d to %d \r\n", interval, latency, timeout);
}

#if defined(CONFIG_BT_SMP)
static void identity_resolved(struct bt_conn *conn, const bt_addr_le_t *rpa,
			      const bt_addr_le_t *identity)
{
	char addr_identity[BT_ADDR_LE_STR_LEN];
	char addr_rpa[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(identity, addr_identity, sizeof(addr_identity));
	bt_addr_le_to_str(rpa, addr_rpa, sizeof(addr_rpa));

	vOutputString("Identity resolved %s -> %s \r\n", addr_rpa, addr_identity);
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	vOutputString("Security changed: %s level %u \r\n", addr, level);
}
#endif

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
	.le_param_updated = le_param_updated,
#if defined(CONFIG_BT_SMP)
	.identity_resolved = identity_resolved,
	.security_changed = security_changed,
#endif
};
#endif //CONFIG_BT_CONN

static void ble_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if(ble_inited){
        vOutputString("Has initialized \r\n");
        return;
    }

    #if defined(CONFIG_BT_CONN)
    default_conn = NULL;
    bt_conn_cb_register(&conn_callbacks);
    #endif
    ble_inited = true;
    vOutputString("Init successfully \r\n");
}

static void ble_get_device_name(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	const char *device_name = bt_get_name();

	if(device_name){
		vOutputString("device_name: %s\r\n",device_name);
	}else
		vOutputString("Failed to read device name\r\n");
}

static void ble_set_device_name(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int	err = 0;

	if(strlen(argv[1]) > 0 && strlen(argv[1])<=CONFIG_BT_DEVICE_NAME_MAX){
		err = bt_set_name((char*)argv[1]);
		if(err){
			vOutputString("Failed to set device name\r\n");
		}else
			vOutputString("Set the device name successfully\r\n");
	}else{
		vOutputString("Invaild lenth(%d)\r\n",strlen(argv[1]));
	}
}


#if defined(CONFIG_BT_OBSERVER)
static bool data_cb(struct bt_data *data, void *user_data)
{
	char *name = user_data;
    u8_t len;

	switch (data->type) {
	case BT_DATA_NAME_SHORTENED:
	case BT_DATA_NAME_COMPLETE:
        len = (data->data_len > NAME_LEN - 1)?(NAME_LEN - 1):(data->data_len);
		memcpy(name, data->data, len);
		return false;		
	default:
		return true;
	}
}


static void device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t evtype,
			 struct net_buf_simple *buf)
{
	char 		le_addr[BT_ADDR_LE_STR_LEN];
	char 		name[NAME_LEN];

	(void)memset(name, 0, sizeof(name));
	bt_data_parse(buf, data_cb, name);
	bt_addr_le_to_str(addr, le_addr, sizeof(le_addr));
	
	vOutputString("[DEVICE]: %s, AD evt type %u, RSSI %i %s \r\n",le_addr, evtype, rssi, name);
}

static void ble_start_scan(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    struct bt_le_scan_param scan_param;
    int err;

    (void)err;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    get_uint8_from_string(&argv[1], &scan_param.type);
    
    get_uint8_from_string(&argv[2], &scan_param.filter_dup);
    
    get_uint16_from_string(&argv[3], &scan_param.interval);
    
    get_uint16_from_string(&argv[4], &scan_param.window);

    err = bt_le_scan_start(&scan_param, device_found);
    
    if(err){
        vOutputString("Failed to start scan (err %d) \r\n", err);
    }else{
        vOutputString("Start scan successfully \r\n");
    }
}


static void ble_stop_scan(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err;
    
	err = bt_le_scan_stop();
	if (err) {
		vOutputString("Stopping scanning failed (err %d)\r\n", err);
	} else {
		vOutputString("Scan successfully stopped \r\n");
	}
}
#endif //#if defined(CONFIG_BT_OBSERVER)


#if defined(CONFIG_BT_PERIPHERAL)
static void ble_read_local_address(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	bt_addr_le_t local_pub_addr;
	bt_addr_le_t local_ram_addr;
	char le_addr[BT_ADDR_LE_STR_LEN];

       bt_get_local_public_address(&local_pub_addr);
	bt_addr_le_to_str(&local_pub_addr, le_addr, sizeof(le_addr));
	vOutputString("Local public addr : %s\r\n", le_addr);

	bt_get_local_ramdon_address(&local_ram_addr);
	bt_addr_le_to_str(&local_ram_addr, le_addr, sizeof(le_addr));
	vOutputString("Local random addr : %s\r\n", le_addr);
}

static void ble_set_adv_channel(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    u8_t channel = 7;

    if(argc != 2)
    {
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    get_uint8_from_string(&argv[1], &channel);

    if (set_adv_channel_map(channel) == 0)
    {
        vOutputString("Set adv channel success\r\n");
    }
    else
    {
        vOutputString("Failed to Set adv channel\r\n");
    }
}

static void ble_start_advertise(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    struct bt_le_adv_param param;
	const struct bt_data *ad;
	size_t ad_len;
	int err = 0;
    uint8_t adv_type, tmp;
	
    if(argc != 3 && argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    param.id = selected_id;
    param.interval_min = BT_GAP_ADV_FAST_INT_MIN_2;
    param.interval_max = BT_GAP_ADV_FAST_INT_MAX_2;

    /*Get adv type, 0:adv_ind,  1:adv_scan_ind, 2:adv_nonconn_ind 3: adv_direct_ind*/
    get_uint8_from_string(&argv[1], &adv_type);
    vOutputString("adv_type 0x%x\r\n",adv_type);
    if(adv_type == 0){
        param.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_ONE_TIME);
    }else if(adv_type == 1){
        param.options = BT_LE_ADV_OPT_USE_NAME;
    }else if(adv_type == 2){
        param.options = 0;
    }else{
        vOutputString("Arg1 is invalid\r\n");
        return;
    }

    /*Get mode, 0:General discoverable,  1:non discoverable, 2:limit discoverable*/
    get_uint8_from_string(&argv[2], &tmp);
	 vOutputString("tmp 0x%x\r\n",tmp);
    if(tmp == 0 || tmp == 1 || tmp == 2){
		
        if(tmp == 1)
            ad_discov[0].data = 0;
        ad = ad_discov;
        ad_len = ARRAY_SIZE(ad_discov);

    }else{
        vOutputString("Arg2 is invalid\r\n");
        return;
    }

    if(argc == 5){
        get_uint16_from_string(&argv[3], &param.interval_min);
        get_uint16_from_string(&argv[4], &param.interval_max);

      	vOutputString("interval min 0x%x\r\n",param.interval_min);  
      	
      	vOutputString("interval max 0x%x\r\n",param.interval_max);  
    }
	
    if(adv_type == 1){
		
        err = bt_le_adv_start(&param, ad, ad_len, &ad_discov[0], 1);
		
    }else{
        err = bt_le_adv_start(&param, ad, ad_len, NULL, 0);
    }
 
    if(err){
        vOutputString("Failed to start advertising\r\n");
    }else{
        vOutputString("Advertising started\r\n");
    }

}

static void ble_stop_advertise(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if(bt_le_adv_stop()){
        vOutputString("Failed to stop advertising\r\n");	
    }else{ 
        vOutputString("Advertising stopped\r\n");
    }
}


#endif //#if defined(CONFIG_BT_PERIPHERAL)

#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
static void ble_connect(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    bt_addr_le_t addr;
    struct bt_conn *conn;
    u8_t  addr_val[6];
	s8_t  type = -1;
	int i=0;

	struct bt_le_conn_param param = {
		.interval_min =  BT_GAP_INIT_CONN_INT_MIN,
		.interval_max =  BT_GAP_INIT_CONN_INT_MAX,
		.latency = 0,
		.timeout = 400,
	};

	(void)memset(addr_val,0,6);

    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
       
    get_uint8_from_string(&argv[1], (u8_t *)&type);

	/*Get addr type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
    addr.type = type;

    get_bytearray_from_string(&argv[2], addr_val,6);
     for(i=0;i<6;i++){
		vOutputString("addr[%d]:[0x%x]\r\n",i,addr_val[i]);
    }
	
    reverse_bytearray(addr_val, addr.a.val, 6);
   
    conn = bt_conn_create_le(&addr, /*BT_LE_CONN_PARAM_DEFAULT*/&param);

    if(!conn){
        vOutputString("Connection failed\r\n");
    }else{
        vOutputString("Connection pending\r\n");
    }
}

static void ble_disconnect(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    bt_addr_le_t addr;
    u8_t  addr_val[6];
    struct bt_conn *conn;
	s8_t  type = -1;
	
    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    get_uint8_from_string(&argv[1], (u8_t *)&type);
    get_bytearray_from_string(&argv[2], addr_val,6);
    reverse_bytearray(addr_val, addr.a.val, 6);

	/*Get addr type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
    addr.type = type;

    conn = bt_conn_lookup_addr_le(selected_id, &addr);

    if(!conn){
        vOutputString("Not connected\r\n");
        return;
    }

    if(bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN)){
        vOutputString("Disconnection failed\r\n");
    }else{
        vOutputString("Disconnect successfully\r\n");
    }
    bt_conn_unref(conn);
}
#endif //#if defined(CONFIG_BT_CENTRAL)

static void ble_select_conn(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    bt_addr_le_t addr;
    struct bt_conn *conn;
    u8_t  addr_val[6];

    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    /*Get addr type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
    get_uint8_from_string(&argv[1], &addr.type);

    get_bytearray_from_string(&argv[2], addr_val,6);

    reverse_bytearray(addr_val, addr.a.val, 6);
    
    conn = bt_conn_lookup_addr_le(selected_id, &addr);
           
    if(!conn){
        vOutputString("No matching connection found\r\n");
        return;
    }

    if(default_conn){
        bt_conn_unref(default_conn);
    }

    default_conn = conn;
}

static void ble_unpair(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    bt_addr_le_t addr;
    u8_t  addr_val[6];
    int err;

    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    /*Get addr type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
    get_uint8_from_string(&argv[1], &addr.type);
    
    get_bytearray_from_string(&argv[2], addr_val,6);

    reverse_bytearray(addr_val, addr.a.val, 6);
        
    err = bt_unpair(selected_id, &addr);

    if(err){
        vOutputString("Failed to unpair\r\n");
    }else{
        vOutputString("Unpair successfully\r\n");
    }
}

static void ble_conn_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	struct bt_le_conn_param param;
	int err;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    get_uint16_from_string(&argv[1], &param.interval_min);
    get_uint16_from_string(&argv[2], &param.interval_max);
    get_uint16_from_string(&argv[3], &param.latency);
    get_uint16_from_string(&argv[4], &param.timeout);	
    err = bt_conn_le_param_update(default_conn, &param);

	if (err) {
		vOutputString("conn update failed (err %d)\r\n", err);
	} else {
		vOutputString("conn update initiated\r\n");
	}
}
#endif //#if defined(CONFIG_BT_CONN)

#if defined(CONFIG_BT_SMP)
static void ble_security(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int err;
   	u8_t sec_level = /*BT_SECURITY_FIPS*/BT_SECURITY_L4;

    if(!default_conn){
        vOutputString("Please firstly choose the connection using ble_select_conn\r\n");
        return;
    }

    if(argc == 2)
        get_uint8_from_string(&argv[1], &sec_level);
    
    err = bt_conn_set_security(default_conn, sec_level);

    if(err){
        vOutputString("Failed to start security, (err %d) \r\n", err);
    }else{
        vOutputString("Start security successfully\r\n");
    }
}

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));    

    vOutputString("passkey_str is: %06u\r\n", passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	vOutputString("Confirm passkey for %s: %06u\r\n", addr, passkey);
}

static void auth_passkey_entry(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	vOutputString("Enter passkey for %s\r\n", addr);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	
	vOutputString("Pairing cancelled: %s\r\n", addr);
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	vOutputString("Confirm pairing for %s\r\n", addr);
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	vOutputString("%s with %s\r\n", bonded ? "Bonded" : "Paired", addr);
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	vOutputString("Pairing failed with %s\r\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = auth_passkey_entry,
	.passkey_confirm = auth_passkey_confirm,
	.cancel = auth_cancel,
	.pairing_confirm = auth_pairing_confirm,
	.pairing_failed = auth_pairing_failed,
	.pairing_complete = auth_pairing_complete,
};

static void ble_auth(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int err;

    err = bt_conn_auth_cb_register(&auth_cb_display);

    if(err){
        vOutputString("Auth callback has already been registered\r\n");
    }else{
        vOutputString("Register auth callback successfully\r\n");
    }
}

static void ble_auth_cancel(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	struct bt_conn *conn;
    
	if (default_conn) {
		conn = default_conn;
	}else {
		conn = NULL;
	}

	if (!conn) {
        vOutputString("Not connected\r\n");
		return;
	}

	bt_conn_auth_cancel(conn);
}

static void ble_auth_passkey_confirm(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    
	if (!default_conn) {
        vOutputString("Not connected\r\n");
		return;
	}

	bt_conn_auth_passkey_confirm(default_conn);
}

static void ble_auth_pairing_confirm(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
   
	if (!default_conn) {
        vOutputString("Not connected\r\n");
		return;
	}

	bt_conn_auth_pairing_confirm(default_conn);
}

static void ble_auth_passkey(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	uint32_t passkey;

    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (!default_conn) {
        vOutputString("Not connected\r\n");
		return;
	}

    passkey = atoi(argv[1]);
	if (passkey > PASSKEY_MAX) {
        vOutputString("Passkey should be between 0-999999\r\n");
		return;
	}

	bt_conn_auth_passkey_entry(default_conn, passkey);
}

#endif //#if defined(CONFIG_BT_SMP)

#if defined(CONFIG_BT_GATT_CLIENT)
static void exchange_func(struct bt_conn *conn, u8_t err,
			  struct bt_gatt_exchange_params *params)
{
	vOutputString("Exchange %s\r\n", err == 0U ? "successful" : "failed");
}

static struct bt_gatt_exchange_params exchange_params;

static void ble_exchange_mtu(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err;
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

	exchange_params.func = exchange_func;

	err = bt_gatt_exchange_mtu(default_conn, &exchange_params);
	if (err) {
		vOutputString("Exchange failed (err %d)\r\n", err);
	} else {
		vOutputString("Exchange pending\r\n");
	}
}

static struct bt_gatt_discover_params discover_params;
static struct bt_uuid_16 uuid = BT_UUID_INIT_16(0);

static void print_chrc_props(u8_t properties)
{
	vOutputString("Properties: ");

	if (properties & BT_GATT_CHRC_BROADCAST) {
		vOutputString("[bcast]\r\n");
	}

	if (properties & BT_GATT_CHRC_READ) {
		vOutputString("[read]\r\n");
	}

	if (properties & BT_GATT_CHRC_WRITE) {
		vOutputString("[write]\r\n");
	}

	if (properties & BT_GATT_CHRC_WRITE_WITHOUT_RESP) {
		vOutputString("[write w/w rsp]\r\n");
	}

	if (properties & BT_GATT_CHRC_NOTIFY) {
		vOutputString("[notify]\r\n");
	}

	if (properties & BT_GATT_CHRC_INDICATE) {
		vOutputString("[indicate]");
	}

	if (properties & BT_GATT_CHRC_AUTH) {
		vOutputString("[auth]\r\n");
	}

	if (properties & BT_GATT_CHRC_EXT_PROP) {
		vOutputString("[ext prop]\r\n");
	}
}

u8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params)
{
	struct bt_gatt_service_val *gatt_service;
	struct bt_gatt_chrc *gatt_chrc;
	struct bt_gatt_include *gatt_include;
	char str[37];

	if (!attr) {
		vOutputString( "Discover complete\r\n");
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

	switch (params->type) {
	case BT_GATT_DISCOVER_SECONDARY:
	case BT_GATT_DISCOVER_PRIMARY:
		gatt_service = attr->user_data;
		bt_uuid_to_str(gatt_service->uuid, str, sizeof(str));
		vOutputString("Service %s found: start handle %x, end_handle %x\r\n", str, attr->handle, gatt_service->end_handle);
		break;
	case BT_GATT_DISCOVER_CHARACTERISTIC:
		gatt_chrc = attr->user_data;
		bt_uuid_to_str(gatt_chrc->uuid, str, sizeof(str));
		vOutputString("Characteristic %s found: attr->handle %x  chrc->handle %x \r\n", str, attr->handle,gatt_chrc->value_handle);
		print_chrc_props(gatt_chrc->properties);
		break;
	case BT_GATT_DISCOVER_INCLUDE:
		gatt_include = attr->user_data;
		bt_uuid_to_str(gatt_include->uuid, str, sizeof(str));
		vOutputString("Include %s found: handle %x, start %x, end %x\r\n", str, attr->handle,
			    gatt_include->start_handle, gatt_include->end_handle);
		break;
	default:
		bt_uuid_to_str(attr->uuid, str, sizeof(str));
		vOutputString("Descriptor %s found: handle %x\r\n", str, attr->handle);
		break;
	}

	return BT_GATT_ITER_CONTINUE;
}

static void ble_discover(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err;
    u8_t disc_type;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

	discover_params.func = discover_func;
	discover_params.start_handle = 0x0001;
	discover_params.end_handle = 0xffff;

    get_uint8_from_string(&argv[1], &disc_type);
    if(disc_type == 0){
        discover_params.type = BT_GATT_DISCOVER_PRIMARY;
    }else if(disc_type == 1){
        discover_params.type = BT_GATT_DISCOVER_SECONDARY;
    }else if(disc_type == 2){
        discover_params.type = BT_GATT_DISCOVER_INCLUDE;
    }else if(disc_type == 3){
        discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
    }else if(disc_type == 4){
        discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
    }else{
        vOutputString("Invalid discovery type\r\n");
        return;
    }
    get_uint16_from_string(&argv[2], &uuid.val);
	
    if(uuid.val)
        discover_params.uuid = &uuid.uuid;
    else
        discover_params.uuid = NULL;

    get_uint16_from_string(&argv[3], &discover_params.start_handle);
    get_uint16_from_string(&argv[4], &discover_params.end_handle);

	err = bt_gatt_discover(default_conn, &discover_params);
	if (err) {
		vOutputString("Discover failed (err %d)\r\n", err);
	} else {
		vOutputString("Discover pending\r\n");
	}
}

static struct bt_gatt_read_params read_params;

static u8_t read_func(struct bt_conn *conn, u8_t err, struct bt_gatt_read_params *params, const void *data, u16_t length)
{
	vOutputString("Read complete: err %u length %u \r\n", err, length);

	char str[22]; 
	u8_t *buf = (u8_t *)data;

	memset(str,0,15);
	
	if(length > 0 && length <= sizeof(str)){
		memcpy(str,buf,length);
		vOutputString("device name : %s \r\n",str);
	}

	if (!data) {
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_CONTINUE;
}

static void ble_read(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err;

    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

    get_uint16_from_string(&argv[1], &read_params.single.handle);
    get_uint16_from_string(&argv[2], &read_params.single.offset);

    read_params.func = read_func;
	read_params.handle_count = 1;

	err = bt_gatt_read(default_conn, &read_params);
	if (err) {
		vOutputString("Read failed (err %d)\r\n", err);
	} else {
		vOutputString("Read pending\r\n");
	}
}

static struct bt_gatt_write_params write_params;
static u8_t gatt_write_buf[CHAR_SIZE_MAX];

static void write_func(struct bt_conn *conn, u8_t err,
		       struct bt_gatt_write_params *params)
{
	vOutputString("Write complete: err %u \r\n", err);

	(void)memset(&write_params, 0, sizeof(write_params));
}

static void ble_write(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err;
    uint16_t data_len;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

	if (write_params.func) {
		vOutputString("Write ongoing\r\n");
		return;
	}

    get_uint16_from_string(&argv[1], &write_params.handle);
    get_uint16_from_string(&argv[2], &write_params.offset);
    get_uint16_from_string(&argv[3], &write_params.length);
    data_len = write_params.length > sizeof(gatt_write_buf)? (sizeof(gatt_write_buf)):(write_params.length);
    get_bytearray_from_string(&argv[4], gatt_write_buf,data_len);
    
	write_params.data = gatt_write_buf;
	write_params.length = data_len;
	write_params.func = write_func;
	
	err = bt_gatt_write(default_conn, &write_params);

	if (err) {
		vOutputString("Write failed (err %d)\r\n", err);
	} else {
		vOutputString("Write pending\r\n");
	}
}

static void ble_write_without_rsp(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	u16_t handle;
	int err;
	u16_t len;
	bool sign;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
   
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

    get_uint8_from_string(&argv[1], (uint8_t *)&sign);
    get_uint16_from_string(&argv[2], &handle);
	get_uint16_from_string(&argv[3], &len);
    len = len > sizeof(gatt_write_buf)? (sizeof(gatt_write_buf)):(len);
	get_bytearray_from_string(&argv[4], gatt_write_buf,len);
	
	err = bt_gatt_write_without_response(default_conn, handle, gatt_write_buf, len, sign);

	vOutputString("Write Complete (err %d)\r\n", err);
}

static struct bt_gatt_subscribe_params subscribe_params;

static u8_t notify_func(struct bt_conn *conn,
			struct bt_gatt_subscribe_params *params,
			const void *data, u16_t length)
{
	if (!params->value) {
		vOutputString("Unsubscribed\r\n");
		params->value_handle = 0U;
		return BT_GATT_ITER_STOP;
	}

	vOutputString("Notification: data %p length %u\r\n", data, length);

	return BT_GATT_ITER_CONTINUE;
}

static void ble_subscribe(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err;

    (void)err;

    if(argc != 4){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (subscribe_params.value_handle) {
		vOutputString( "Cannot subscribe: subscription to %x already exists\r\n", subscribe_params.value_handle);
		return;
	}

	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

    get_uint16_from_string(&argv[1], &subscribe_params.ccc_handle);
    get_uint16_from_string(&argv[2], &subscribe_params.value_handle);
    get_uint16_from_string(&argv[3], &subscribe_params.value);
	subscribe_params.notify = notify_func;

	err = bt_gatt_subscribe(default_conn, &subscribe_params);
	if (err) {
		vOutputString("Subscribe failed (err %d)\r\n", err);
	} else {
		vOutputString("Subscribed\r\n");
	}

}

static void ble_unsubscribe(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int err;

    (void)err;
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

	if (!subscribe_params.value_handle) {
		vOutputString("No subscription found\r\n");
		return;
	}

	err = bt_gatt_unsubscribe(default_conn, &subscribe_params);
	if (err) {
		vOutputString("Unsubscribe failed (err %d)\r\n", err);
	} else {
		vOutputString("Unsubscribe success\r\n");
	}
}
#endif /* CONFIG_BT_GATT_CLIENT */

static void ble_set_data_len(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	u16_t tx_octets;
	u16_t tx_time;
	int err;

	if(argc != 3){
	    vOutputString("Number of Parameters is not correct\r\n");
	    return;
	}

	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

	get_uint16_from_string(&argv[1], &tx_octets);
	get_uint16_from_string(&argv[2], &tx_time);

	err = bt_le_set_data_len(default_conn, tx_octets, tx_time);
	if (err) {
		vOutputString("ble_set_data_len, LE Set Data Length (err %d)\r\n", err);
	}
	else
	{
		vOutputString("ble_set_data_len, LE Set Data Length success\r\n");
	}
}

static void ble_get_all_conn_info(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
        struct bt_conn_info info[CONFIG_BT_MAX_CONN];
        char le_addr[BT_ADDR_LE_STR_LEN];
        int link_num;

        link_num = bt_conn_get_remote_dev_info(info);

        if(link_num > 0)
        {
                bt_addr_le_to_str(info[0].le.local, le_addr, sizeof(le_addr));
                vOutputString("ble local device address: %s\r\n", le_addr);
        }

        vOutputString("ble connected devices count: %d\r\n", link_num);
        for(int i = 0; i < link_num; i++)
        {
	        bt_addr_le_to_str(info[i].le.remote, le_addr, sizeof(le_addr));
	        vOutputString("[%d]: address %s\r\n", i, le_addr);
        }
}

#if defined(CONFIG_SET_TX_PWR)
static void ble_set_tx_pwr(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    u8_t power;
    int err;

    if(argc != 2){
	    vOutputString("Number of Parameters is not correct\r\n");
	    return;
	}

    get_uint8_from_string(&argv[1],&power);

    if(power > 0x14){
        vOutputString("ble_set_tx_pwr, invalid value, value shall be in [0x%x - 0x%x]\r\n", 0x00, 0x14);
        return;
    }
        
    err = bt_set_tx_pwr((int8_t)power);

    if(err){
		vOutputString("ble_set_tx_pwr, Fail to set tx power (err %d)\r\n", err);
	}
	else{
		vOutputString("ble_set_tx_pwr, Set tx power successfully\r\n");
	}
    
}
#endif


int ble_cli_register(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(btStackCmdSet, sizeof(btStackCmdSet)/sizeof(btStackCmdSet[0]));
    return 0;
}
