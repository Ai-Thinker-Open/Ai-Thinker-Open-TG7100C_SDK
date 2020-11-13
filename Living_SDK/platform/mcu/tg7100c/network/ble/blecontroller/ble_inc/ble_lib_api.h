#ifndef BLE_LIB_API_H_
#define BLE_LIB_API_H_

#include <stdbool.h>

void ble_controller_init(uint8_t task_priority);
void ble_controller_deinit(void);

#if defined(CONFIG_BLE_MFG)
int le_rx_test_cmd_handler(uint16_t src_id, uint8_t rx_channel, bool from_hci);
int le_tx_test_cmd_handler(uint16_t src_id, uint8_t tx_channel, uint8_t data_len, uint8_t payload_type, bool from_hci);
int le_test_end_cmd_handler(bool from_hci);
int le_set_tx_pwr_handler(uint8_t power);
#endif

// return sleep duration, in unit of 1/32768s
// if 0, means not allow sleep
// if -1, means allow sleep, but there is no end of sleep interrupt (ble core deep sleep is not enabled)
int32_t ble_controller_sleep(void);

void ble_controller_wakeup(void);

uint8_t ble_get_deep_sleep_stat(void);

//#if(LE_PDS_ENABLE)
bool ble_controller_sleep_is_ongoing(void);
//#endif

#endif
