#ifndef __BL_PHY_API_H__
#define __BL_PHY_API_H__

void bl_mfg_phy_init();
void bl_mfg_rf_cal();
void bl_mfg_rx_start();
void bl_mfg_rx_stop();
void bl_mfg_channel_switch(uint8_t chan_no);
int8_t bl_mfg_tx11n_start_raw(uint8_t mcs_n, uint16_t frame_len, uint8_t pwr_dbm);
int8_t bl_mfg_tx11b_start_raw(uint8_t mcs_b, uint16_t frame_len, uint8_t pwr_dbm);
void bl_mfg_tx_stop();
void phy_powroffset_set(int8_t power_offset[14]);

#endif

