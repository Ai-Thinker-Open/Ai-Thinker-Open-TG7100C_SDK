#ifndef __RWNX_MSG_TX_H__
#define __RWNX_MSG_TX_H__
#include "bl_defs.h"

int bl_send_reset(struct bl_hw *bl_hw);
int bl_send_monitor_enable(struct bl_hw *bl_hw, struct mm_monitor_cfm *cfm);
/*
 *  use_40MHZ:
 *      0: Don't use 40MHZ
 *      1: Use lower band as second band
 *      2: Use higher band as second band
 * */
int bl_send_monitor_channel_set(struct bl_hw *bl_hw, struct mm_monitor_channel_cfm *cfm, int channel, int use_40Mhz);
int bl_send_version_req(struct bl_hw *bl_hw, struct mm_version_cfm *cfm);
int bl_send_me_config_req(struct bl_hw *bl_hw);
int bl_send_me_chan_config_req(struct bl_hw *bl_hw);
int bl_send_me_rate_config_req(struct bl_hw *bl_hw, uint8_t sta_idx, uint16_t fixed_rate_cfg);
int bl_send_start(struct bl_hw *bl_hw);
int bl_send_add_if(struct bl_hw *bl_hw, const unsigned char *mac,
                     enum nl80211_iftype iftype, bool p2p, struct mm_add_if_cfm *cfm);
int bl_send_remove_if(struct bl_hw *bl_hw, uint8_t inst_nbr);
int bl_send_scanu_req(struct bl_hw *bl_hw);
int bl_send_scanu_raw_send(struct bl_hw *bl_hw, uint8_t *pkt, int len);
int bl_send_sm_connect_req(struct bl_hw *bl_hw, struct cfg80211_connect_params *sme, struct sm_connect_cfm *cfm);
int bl_send_sm_disconnect_req(struct bl_hw *bl_hw, u16 reason);
int bl_send_mm_powersaving_req(struct bl_hw *bl_hw, int mode);
int bl_send_apm_start_req(struct bl_hw *bl_hw, struct apm_start_cfm *cfm, char *ssid, char *password, int channel, uint8_t vif_index);
int bl_send_apm_stop_req(struct bl_hw *bl_hw, uint8_t vif_idx);
int bl_send_apm_sta_del_req(struct bl_hw *bl_hw, struct apm_sta_del_cfm *cfm, uint8_t sta_idx, uint8_t vif_idx);
int bl_send_channel_set_req(struct bl_hw *bl_hw, int channel);
void bl_msg_update_channel_cfg(const char *code);
int bl_msg_get_channel_nums();
#endif