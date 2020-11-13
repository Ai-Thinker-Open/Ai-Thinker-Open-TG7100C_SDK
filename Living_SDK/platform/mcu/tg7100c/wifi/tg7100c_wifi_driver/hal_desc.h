
/**
 ****************************************************************************************
 *
 * @file hal_desc.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */


#ifndef _HAL_DESC_H_
#define _HAL_DESC_H_

#include "lmac_types.h"

#define NX_TX_MAX_RATES 4
/* Rate and policy table */

#define N_CCK  8
#define N_OFDM 8
#define N_HT   (8 * 2 * 2 * 4)
#define N_VHT  (10 * 4 * 2 * 8)
#define N_RATE (N_CCK + N_OFDM + N_HT + N_VHT)

/* Values for bwTx */
#define __CHBW_CBW20   0
#define __CHBW_CBW40   1
#define __CHBW_CBW80   2
#define __CHBW_CBW160  3

/* Values for formatModTx */
#define FORMATMOD_NON_HT          0
#define FORMATMOD_NON_HT_DUP_OFDM 1
#define FORMATMOD_HT_MF           2
#define FORMATMOD_HT_GF           3
#define FORMATMOD_VHT             4

/* Values for navProtFrmEx */
#define NAV_PROT_NO_PROT_BIT                 0
#define NAV_PROT_SELF_CTS_BIT                1
#define NAV_PROT_RTS_CTS_BIT                 2
#define NAV_PROT_RTS_CTS_WITH_QAP_BIT        3
#define NAV_PROT_STBC_BIT                    4

union bl_mcs_index {
    struct {
        u32 mcs : 3;
        u32 nss : 2;
    } ht;
    struct {
        u32 mcs : 4;
        u32 nss : 3;
    } vht;
    u32 legacy : 7;
};

/* c.f RW-WLAN-nX-MAC-HW-UM */
union bl_rate_ctrl_info {
    struct {
        u32 mcsIndexTx      : 7;
        u32 bwTx            : 2;
        u32 shortGITx       : 1;
        u32 preTypeTx       : 1;
        u32 formatModTx     : 3;
        u32 navProtFrmEx    : 3;
        u32 mcsIndexProtTx  : 7;
        u32 bwProtTx        : 2;
        u32 formatModProtTx : 3;
        u32 nRetry          : 3;
    };
    u32 value;
};

/* c.f RW-WLAN-nX-MAC-HW-UM */
struct bl_power_ctrl_info {
    u32 txPwrLevelPT          : 8;
    u32 txPwrLevelProtPT      : 8;
    u32 reserved              :16;
};

/* c.f RW-WLAN-nX-MAC-HW-UM */
union bl_pol_phy_ctrl_info_1 {
    struct {
        u32 rsvd1     : 3;
        u32 bfFrmEx   : 1;
        u32 numExtnSS : 2;
        u32 fecCoding : 1;
        u32 stbc      : 2;
        u32 rsvd2     : 5;
        u32 nTx       : 3;
        u32 nTxProt   : 3;
    };
    u32 value;
};

/* c.f RW-WLAN-nX-MAC-HW-UM */
union bl_pol_phy_ctrl_info_2 {
    struct {
        u32 antennaSet : 8;
        u32 smmIndex   : 8;
        u32 beamFormed : 1;
    };
    u32 value;
};

/* c.f RW-WLAN-nX-MAC-HW-UM */
union bl_pol_mac_ctrl_info_1 {
    struct {
        u32 keySRamIndex   : 10;
        u32 keySRamIndexRA : 10;
    };
    u32 value;
};

/* c.f RW-WLAN-nX-MAC-HW-UM */
union bl_pol_mac_ctrl_info_2 {
    struct {
        u32 longRetryLimit  : 8;
        u32 shortRetryLimit : 8;
        u32 rtsThreshold    : 12;
    };
    u32 value;
};

#define POLICY_TABLE_PATTERN    0xBADCAB1E

struct tx_policy_tbl {
    /* Unique Pattern at the start of Policy Table */
    u32 upatterntx;
    /* PHY Control 1 Information used by MAC HW */
    union bl_pol_phy_ctrl_info_1 phyctrlinfo_1;
    /* PHY Control 2 Information used by MAC HW */
    union bl_pol_phy_ctrl_info_2 phyctrlinfo_2;
    /* MAC Control 1 Information used by MAC HW */
    union bl_pol_mac_ctrl_info_1 macctrlinfo_1;
    /* MAC Control 2 Information used by MAC HW */
    union bl_pol_mac_ctrl_info_2 macctrlinfo_2;

    union bl_rate_ctrl_info  ratectrlinfos[NX_TX_MAX_RATES];
    struct bl_power_ctrl_info powerctrlinfos[NX_TX_MAX_RATES];
};

/**
 * struct bl_hw_txstatus - Bitfield of confirmation status
 *
 * @tx_done: packet has been sucessfully transmitted
 * @retry_required: packet has been transmitted but not acknoledged.
 * Driver must repush it.
 * @sw_retry_required: packet has not been transmitted (FW wasn't able to push
 * it when it received it: not active channel ...). Driver must repush it.
 */
union bl_hw_txstatus {
    struct {
        u32 tx_done            : 1;
        u32 retry_required     : 1;
        u32 sw_retry_required  : 1;
        u32 reserved           :29;
    };
    u32 value;
};
/* Modem */

#define MDM_PHY_CONFIG_TRIDENT     0
#define MDM_PHY_CONFIG_ELMA        1
#define MDM_PHY_CONFIG_KARST       2

// MODEM features (from reg_mdm_stat.h)
/// MUMIMOTX field bit
#define MDM_MUMIMOTX_BIT    ((u32)0x80000000)
/// MUMIMOTX field position
#define MDM_MUMIMOTX_POS    31
/// MUMIMORX field bit
#define MDM_MUMIMORX_BIT    ((u32)0x40000000)
/// MUMIMORX field position
#define MDM_MUMIMORX_POS    30
/// BFMER field bit
#define MDM_BFMER_BIT       ((u32)0x20000000)
/// BFMER field position
#define MDM_BFMER_POS       29
/// BFMEE field bit
#define MDM_BFMEE_BIT       ((u32)0x10000000)
/// BFMEE field position
#define MDM_BFMEE_POS       28
/// LDPCDEC field bit
#define MDM_LDPCDEC_BIT     ((u32)0x08000000)
/// LDPCDEC field position
#define MDM_LDPCDEC_POS     27
/// LDPCENC field bit
#define MDM_LDPCENC_BIT     ((u32)0x04000000)
/// LDPCENC field position
#define MDM_LDPCENC_POS     26
/// CHBW field mask
#define MDM_CHBW_MASK       ((u32)0x03000000)
/// CHBW field LSB position
#define MDM_CHBW_LSB        24
/// CHBW field width
#define MDM_CHBW_WIDTH      ((u32)0x00000002)
/// DSSSCCK field bit
#define MDM_DSSSCCK_BIT     ((u32)0x00800000)
/// DSSSCCK field position
#define MDM_DSSSCCK_POS     23
/// NESS field mask
#define MDM_NESS_MASK       ((u32)0x00700000)
/// NESS field LSB position
#define MDM_NESS_LSB        20
/// NESS field width
#define MDM_NESS_WIDTH      ((u32)0x00000003)
/// RFMODE field mask
#define MDM_RFMODE_MASK     ((u32)0x000F0000)
/// RFMODE field LSB position
#define MDM_RFMODE_LSB      16
/// RFMODE field width
#define MDM_RFMODE_WIDTH    ((u32)0x00000004)
/// NSTS field mask
#define MDM_NSTS_MASK       ((u32)0x0000F000)
/// NSTS field LSB position
#define MDM_NSTS_LSB        12
/// NSTS field width
#define MDM_NSTS_WIDTH      ((u32)0x00000004)
/// NSS field mask
#define MDM_NSS_MASK        ((u32)0x00000F00)
/// NSS field LSB position
#define MDM_NSS_LSB         8
/// NSS field width
#define MDM_NSS_WIDTH       ((u32)0x00000004)
/// NTX field mask
#define MDM_NTX_MASK        ((u32)0x000000F0)
/// NTX field LSB position
#define MDM_NTX_LSB         4
/// NTX field width
#define MDM_NTX_WIDTH       ((u32)0x00000004)
/// NRX field mask
#define MDM_NRX_MASK        ((u32)0x0000000F)
/// NRX field LSB position
#define MDM_NRX_LSB         0
/// NRX field width
#define MDM_NRX_WIDTH       ((u32)0x00000004)

#define __MDM_PHYCFG_FROM_VERS(v)  (((v) & MDM_RFMODE_MASK) >> MDM_RFMODE_LSB)

#define RIU_FCU_PRESENT_MASK       ((u32)0xFF000000)
#define RIU_FCU_PRESENT_LSB        24

#define __RIU_FCU_PRESENT(v)  (((v) & RIU_FCU_PRESENT_MASK) >> RIU_FCU_PRESENT_LSB == 5)

/// AGC load version field mask
#define RIU_AGC_LOAD_MASK          ((u32)0x00C00000)
/// AGC load version field LSB position
#define RIU_AGC_LOAD_LSB           22

#define __RIU_AGCLOAD_FROM_VERS(v)  (((v) & RIU_AGC_LOAD_MASK) >> RIU_AGC_LOAD_LSB)


#endif // _HAL_DESC_H_
