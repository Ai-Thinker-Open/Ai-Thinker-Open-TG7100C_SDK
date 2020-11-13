#ifndef _BK7011_CAL_PUB_H_
#define _BK7011_CAL_PUB_H_

typedef struct tmp_pwr_st {
    UINT8 mod;
    UINT8 pa;
} TMP_PWR_ST, *TMP_PWR_PTR;

extern void calibration_main(void);
extern INT32 rwnx_cal_load_trx_rcbekn_reg_val(void);
extern void rwnx_cal_set_txpwr_by_rate(INT32 rate, UINT32 test_mode);
extern INT32 rwnx_cal_save_trx_rcbekn_reg_val(void);
extern void do_calibration_in_temp_dect(void);
extern void bk7011_cal_bias(void);
extern void bk7011_cal_dpll(void);
extern void rwnx_cal_set_txpwr(UINT32 pwr_gain);

extern int manual_cal_get_txpwr(UINT32 rate, UINT32 channel, UINT32 bandwidth, UINT32 *pwr_gain);
extern void manual_cal_save_txpwr(UINT32 rate, UINT32 channel, UINT32 pwr_gain);
extern UINT32 manual_cal_fitting_txpwr_tab(void);
extern void manual_cal_show_txpwr_tab(void);
extern UINT32 manual_cal_load_txpwr_tab_flash(void);
extern int manual_cal_save_txpwr_tab_to_flash(void);
extern int manual_cal_save_chipinfo_tab_to_flash(void);
extern UINT8 manual_cal_wirte_otp_flash(UINT32 addr, UINT32 len, UINT8 *buf);
extern UINT8 manual_cal_read_otp_flash(UINT32 addr, UINT32 len, UINT8 *buf);
extern UINT32 manual_cal_load_default_txpwr_tab(UINT32 is_ready_flash);
extern void manual_cal_set_dif_g_n40(UINT32 diff);
extern void manual_cal_get_current_temperature(void);

extern void bk7011_micopwr_config_tssi_read_prepare(void);
extern void bk7011_micopwr_tssi_read(void);
extern void bk7011_micopwr_tssi_show(void);

extern void manual_cal_tmp_pwr_init(UINT16 init_temp, UINT16 init_thre, UINT16 init_dist);
extern void manual_cal_tmp_pwr_init_reg(UINT16 reg_mod, UINT16 reg_pa);
extern void manual_cal_temp_pwr_unint(void);
extern TMP_PWR_PTR manual_cal_set_tmp_pwr(UINT16 cur_val, UINT16 thre, UINT16 *last);
extern UINT32 manual_cal_load_temp_tag_flash(void);

extern void rwnx_cal_set_reg_mod_pa(UINT16 reg_mod, UINT16 reg_pa);
extern void rwnx_cal_do_temp_detect(UINT16 cur_val, UINT16 thre, UINT16 *last);


#endif // _BK7011_CAL_PUB_H_
