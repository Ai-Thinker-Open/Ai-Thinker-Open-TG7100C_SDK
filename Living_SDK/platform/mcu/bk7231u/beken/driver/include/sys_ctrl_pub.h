#ifndef _SCTRL_PUB_H_
#define _SCTRL_PUB_H_

#define SCTRL_DEV_NAME       "sys_ctrl"

#define SCTRL_FAILURE        ((UINT32)-1)
#define SCTRL_SUCCESS        (0)

#define SCTRL_CMD_MAGIC      (0xC123000)

enum
{
    CMD_GET_CHIP_ID              = SCTRL_CMD_MAGIC + 1,
    CMD_GET_DEVICE_ID            = SCTRL_CMD_MAGIC + 2,
    CMD_GET_SCTRL_CONTROL,
    CMD_SET_SCTRL_CONTROL,

    CMD_SCTRL_MCLK_SELECT,
    CMD_SCTRL_MCLK_DIVISION,

    CMD_SCTRL_RESET_SET,
    CMD_SCTRL_RESET_CLR,
    CMD_SCTRL_MODEM_CORE_RESET,
    CMD_SCTRL_MPIF_CLK_INVERT,
    CMD_SCTRL_MODEM_SUBCHIP_RESET,
    CMD_SCTRL_MAC_SUBSYS_RESET,
    CMD_SCTRL_USB_SUBSYS_RESET,
    CMD_SCTRL_DSP_SUBSYS_RESET,
    CMD_SCTRL_BLK_ENABLE,
    CMD_SCTRL_BLK_DISABLE,

    CMD_SCTRL_DSP_POWERDOWN,
    CMD_SCTRL_DSP_POWERUP,
    CMD_SCTRL_USB_POWERDOWN,
    CMD_SCTRL_USB_POWERUP,
    CMD_SCTRL_MAC_POWERDOWN,
    CMD_SCTRL_MAC_POWERUP,
    CMD_SCTRL_MODEM_POWERDOWN,
    CMD_SCTRL_MODEM_POWERUP,
    CMD_SCTRL_BLE_POWERDOWN,
    CMD_SCTRL_BLE_POWERUP,

    CMD_SCTRL_CALI_DPLL,

    CMD_SCTRL_BIAS_REG_SET,
    CMD_SCTRL_BIAS_REG_CLEAN,
    CMD_SCTRL_BIAS_REG_READ,
    CMD_SCTRL_BIAS_REG_WRITE,

    CMD_SCTRL_ANALOG_CTRL4_SET,
    CMD_SCTRL_ANALOG_CTRL4_CLEAN,

    CMD_SCTRL_SET_FLASH_DCO,
    CMD_SCTRL_SET_FLASH_DPLL,
    CMD_SCTRL_NORMAL_SLEEP,
    CMD_SCTRL_NORMAL_WAKEUP,
    CMD_SCTRL_RTOS_IDLE_SLEEP,
    CMD_SCTRL_RTOS_IDLE_WAKEUP,
    CMD_SCTRL_RTOS_DEEP_SLEEP,

    #if (CFG_SOC_NAME != SOC_BK7231)
    CMD_SCTRL_SET_XTALH_CTUNE,
    CMD_SCTRL_GET_XTALH_CTUNE,
    CMD_BLE_RF_BIT_SET,
    CMD_BLE_RF_BIT_CLR,
    CMD_BLE_RF_BIT_GET,

    CMD_EFUSE_WRITE_BYTE,
    CMD_EFUSE_READ_BYTE,

    CMD_QSPI_VDDRAM_VOLTAGE,
    CMD_QSPI_IO_VOLTAGE,
	#endif // (CFG_SOC_NAME != SOC_BK7231)

    #if (CFG_SOC_NAME == SOC_BK7221U)
    CMD_SCTRL_OPEN_DAC_ANALOG,
    CMD_SCTRL_CLOSE_DAC_ANALOG,
    CMD_SCTRL_OPEN_ADC_MIC_ANALOG,
    CMD_SCTRL_CLOSE_ADC_MIC_ANALOG,
    CMD_SCTRL_ENALBLE_ADC_LINE_IN,
    CMD_SCTRL_DISALBLE_ADC_LINE_IN,
    CMD_SCTRL_SET_DAC_VOLUME_ANALOG,
    CMD_SCTRL_SET_LINEIN_VOLUME_ANALOG,    
    CMD_SCTRL_SET_VOLUME_PORT,
    CMD_SCTRL_SET_AUD_DAC_MUTE,
    CMD_SCTRL_SET_ANALOG0,
    CMD_SCTRL_SET_ANALOG1,
    CMD_SCTRL_SET_ANALOG2,
    CMD_SCTRL_SET_ANALOG3,
    CMD_SCTRL_SET_ANALOG4,
    CMD_SCTRL_SET_ANALOG5,
    CMD_SCTRL_SET_ANALOG6,
    CMD_SCTRL_SET_ANALOG7,
    CMD_SCTRL_SET_ANALOG8,
    CMD_SCTRL_SET_ANALOG9,
    CMD_SCTRL_SET_ANALOG10,
    CMD_SCTRL_GET_ANALOG0,
    CMD_SCTRL_GET_ANALOG1,
    CMD_SCTRL_GET_ANALOG2,
    CMD_SCTRL_GET_ANALOG3,
    CMD_SCTRL_GET_ANALOG4,
    CMD_SCTRL_GET_ANALOG5,
    CMD_SCTRL_GET_ANALOG6,
    CMD_SCTRL_GET_ANALOG7,
    CMD_SCTRL_GET_ANALOG8,
    CMD_SCTRL_GET_ANALOG9,
    CMD_SCTRL_GET_ANALOG10,
    CMD_SCTRL_AUDIO_PLL,
    CMD_SCTRL_USB_CHARGE_CAL,
    CMD_SCTRL_USB_CHARGE_START,
    CMD_SCTRL_USB_CHARGE_STOP,
    CMD_SCTRL_UNCONDITIONAL_RF_DOWN,
    CMD_SCTRL_UNCONDITIONAL_RF_UP,
    CMD_SCTRL_UNCONDITIONAL_MAC_DOWN,
    CMD_SCTRL_UNCONDITIONAL_MAC_UP,
    
	#endif // (CFG_SOC_NAME == SOC_BK7221)
    CMD_SCTRL_SET_LOW_PWR_CLK,
    CMD_SCTRL_SET_VDD_VALUE,

};

/*PSRAM_VDDPAD_VOLT */
#define PSRAM_VDD_1_8V                                       (0x0)
#define PSRAM_VDD_2_5V                                       (0x1)
#define PSRAM_VDD_3_3V                                       (0x2)
#define PSRAM_VDD_3_3V_DEF                                   (0x3)

#define QSPI_IO_1_8V                                         (0x0)
#define QSPI_IO_2_5V                                         (0x1)
#define QSPI_IO_3_3V                                         (0x2)
#define QSPI_IO_3V_DEF                                       (0x3)

/*CMD_SCTRL_MCLK_SELECT*/
#define MCLK_SELECT_DCO                                      (0x0)
#define MCLK_SELECT_26M_XTAL                                 (0x1)
#define MCLK_SELECT_DPLL                                     (0x2)
#define MCLK_SELECT_LPO                                      (0x3)

/*CMD_SCTRL_BLK_ENABLE CMD_SCTRL_BLK_DISABLE*/
#if (CFG_SOC_NAME == SOC_BK7231)
#define BLK_BIT_LINEIN                           (1 << 19)
#define BLK_BIT_MIC_R_CHANNEL                    (1 << 18)
#define BLK_BIT_MIC_L_CHANNEL                    (1 << 17)
#define BLK_BIT_AUDIO_R_CHANNEL                  (1 << 16)
#define BLK_BIT_AUDIO_L_CHANNEL                  (1 << 15)
#elif (CFG_SOC_NAME == SOC_BK7231U)
#define BLK_BIT_NC                               (1 << 19)
#define BLK_BIT_MIC_QSPI_RAM_OR_FLASH            (1 << 18)
#define BLK_BIT_MIC_PGA                          (1 << 17)
#define BLK_BIT_AUDIO_PLL                        (1 << 16)
#define BLK_BIT_AUDIO_RANDOM_GENERATOR           (1 << 15)
#elif (CFG_SOC_NAME == SOC_BK7221U)
#define BLK_BIT_NC                               (1 << 19)
#define BLK_BIT_MIC_QSPI_RAM_OR_FLASH            (1 << 18)
#define BLK_BIT_AUDIO                            (1 << 17)
#define BLK_BIT_AUDIO_PLL                        (1 << 16)
#define BLK_BIT_AUDIO_RANDOM_GENERATOR           (1 << 15)
#endif // (CFG_SOC_NAME == SOC_BK7231)
#define BLK_BIT_USB                              (1 << 14)
#define BLK_BIT_SARADC                           (1 << 13)
#define BLK_BIT_TEMPRATURE_SENSOR                (1 << 12)
#define BLK_BIT_26M_XTAL_LOW_POWER               (1 << 11)
#define BLK_BIT_XTAL2RF                          (1 << 10)
#define BLK_BIT_IO_LDO_LOW_POWER                 (1 << 09)
#define BLK_BIT_ANALOG_SYS_LDO                   (1 << 08)
#define BLK_BIT_DIGITAL_CORE_LDO_LOW_POWER       (1 << 07)
#define BLK_BIT_DIGITAL_CORE                     (1 << 06)
#define BLK_BIT_DPLL_480M                        (1 << 05)
#define BLK_BIT_32K_XTAL                         (1 << 04)
#define BLK_BIT_26M_XTAL                         (1 << 03)
#define BLK_BIT_ROSC32K                          (1 << 02)
#define BLK_BIT_DCO                              (1 << 01)
#define BLK_BIT_FLASH                            (1 << 00)

/* CMD_SCTRL_RESET _SET/_CLR*/
#define PARAM_MODEM_CORE_RESET_BIT               (1 << 6)
#define PARAM_TL410_EXT_WAIT_BIT                 (1 << 5)
#define PARAM_USB_SUBSYS_RESET_BIT               (1 << 4)
#define PARAM_TL410_BOOT_BIT                     (1 << 3)
#define PARAM_MAC_SUBSYS_RESET_BIT               (1 << 2)
#define PARAM_DSP_SUBSYS_RESET_BIT               (1 << 1)
#define PARAM_MODEM_SUBCHIP_RESET_BIT            (1 << 0)

/* CMD_GET_SCTRL_CONTROL CMD_SET_SCTRL_CONTROL*/
#define MCLK_MODE_DCO                            (0x0)
#define MCLK_MODE_26M_XTAL                       (0x1)
#define MCLK_MODE_DPLL                           (0x2)
#define MCLK_MODE_LPO                            (0x3)

/*CMD_SCTRL_BIAS_REG_SET CMD_SCTRL_BIAS_REG_CLEAN*/
#define PARAM_BIAS_CAL_OUT_POSI                  (16)
#define PARAM_BIAS_CAL_OUT_MASK                  (0x1F)
#define PARAM_LDO_VAL_MANUAL_POSI                (8)
#define PARAM_LDO_VAL_MANUAL_MASK                (0x1F)
#define PARAM_BIAS_CAL_MANUAL_BIT                (1 << 4)
#define PARAM_BIAS_CAL_TRIGGER_BIT               (1 << 0)

/*CMD_SCTRL_ANALOG_CTRL4_SET CMD_SCTRL_ANALOG_CTRL4_CLEAN*/
#define PARAM_VSEL_SYS_LDO_POSI                  (27)
#define PARAM_VSEL_SYS_LDO_MASK                  (0x3)

#if (CFG_SOC_NAME == SOC_BK7231U)
#define DEFAULT_TXID_XTAL                        (0x19)
#elif (CFG_SOC_NAME == SOC_BK7221U)
#define DEFAULT_TXID_XTAL                        (0x10)
#endif // (CFG_SOC_NAME == SOC_BK7231U)

#if (CFG_SOC_NAME != SOC_BK7231)
#define PARAM_XTALH_CTUNE_MASK                   (0x3F)

#define PARAM_AUD_DAC_GAIN_MASK                  (0x1F)
#endif // (CFG_SOC_NAME != SOC_BK7231)

/*CMD_SCTRL_SET_LOW_PWR_CLK*/
#define LPO_SELECT_ROSC                             (0x0)
#define LPO_SELECT_32K_XTAL                         (0x1)
#define LPO_SELECT_32K_DIV                          (0x2)

typedef union
{
    UINT32 val;
    struct
    {
        UINT32 mclk_mux: 2;
        UINT32 resv0: 2;
        UINT32 mclk_div: 4;
        UINT32 flash_26m_select: 1;
        UINT32 hclk_div2_en: 1;
        UINT32 modem_clk480m_pwd: 1;
        UINT32 mac_clk480m_pwd: 1;
        UINT32 mpif_clk_inv: 1;
        UINT32 sdio_clk_inv: 1;
        UINT32 resv1: 18;
    } bits;
} SYS_CTRL_U;

typedef struct efuse_oper_st
{
    UINT8 addr;
    UINT8 data;    
} EFUSE_OPER_ST, *EFUSE_OPER_PTR;

typedef enum
{
    INTERNAL_HW_MODE = 0,
    INTERNAL_SW_MODE = 1,
    EXTERNAL_HW_MODE = 2,
    EXTERNAL_SW_MODE = 3,
} CHARGE_TYPE;

typedef enum
{
    STEP_STOP = 0,
    STEP_START = 1,
    STEP_TRICKLE = 2,
    STEP_EXTER_CC = 3,
    STEP_INTER_CC = 4,
    STEP_INTER_CV = 5,

} CHARGE_STEP;

typedef struct charge_oper_st
{
    CHARGE_TYPE type;
    CHARGE_STEP step;
    UINT32 elect;
    UINT8 cal[3];
} CHARGE_OPER_ST, *CHARGE_OPER_PTR;

#define CHARGE_ANALOG_CTRL3_CAL_DEFAULT_VALUE       (0x180004A0)
#define CHARGE_ANALOG_CTRL3_CHARGE_DEFAULT_VALUE    (0x180704A0)
#define CHARGE_ANALOG_CTRL4_CAL_DEFAULT_VALUE       (0xC2400520)
#define CHARGE_ANALOG_CTRL4_CHARGE_DEFAULT_VALUE    (0xC2401520)


#define AUDIO_DAC_VOL_DIFF_MODE                      (0)
#define AUDIO_DAC_VOL_SINGLE_MODE                    (1)

#define AUDIO_DAC_ANALOG_UNMUTE                      (0)
#define AUDIO_DAC_ANALOG_MUTE                        (1)

#define EFUSE_ENCRYPT_WORD_ADDR                      (0)
#define EFUSE_ENCRYPT_WORD_LEN                       (16)
#define EFUSE_CHARGE_CAL_ADDR                        (16)
#define EFUSE_CHARGE_CAL_LEN                         (4)
#define EFUSE_UID_ADDR                               (20)
#define EFUSE_UID_LEN                                (4)
#define EFUSE_MAC_START_ADDR                         (24)
#define EFUSE_MAC_LEN                                (6)
#define EFUSE_USER_AREA_ADDR                         (30)
#define EFUSE_USER_AREA_LEN                          (1)
#define EFUSE_CTRL_ADDR                              (31)
#define EFUSE_USER_AREA_LEN                          (1)
#define EFUSE_INIT_VAL                               (0x0)

#define EFUSE_CTRL_JTAG_DISABLE                      (1 << 7)
#define EFUSE_CTRL_FLASH_DOWNLOAD_DISABLE            (1 << 6)
#define EFUSE_CTRL_ENCRYPT_EN                        (1 << 5)
#define EFUSE_CTRL_ENCRYPT_DISABLE_READ              (1 << 4)
#define EFUSE_CTRL_ENCRYPT_DISABLE_WRITE             (1 << 3)
#define EFUSE_CTRL_UID_DISABLE_WRITE                 (1 << 2)
#define EFUSE_CTRL_MAC_DISABLE_WRITE                 (1 << 1)
#define EFUSE_CTRL_ALL_AREA_DISABLE_WRITE            (1 << 0)

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void sctrl_init(void);
extern void sctrl_exit(void);

extern void sctrl_normal_exit_sleep(void);
extern void sctrl_normal_enter_sleep(UINT32 peri_clk);
extern void sctrl_mcu_exit(void);
extern void sctrl_mcu_init(void);
extern void sctrl_mcu_sleep(UINT32 );
extern UINT32 sctrl_mcu_wakeup(void);
extern void sctrl_ps_dump();
extern void sctrl_rf_sleep(void);
extern void sctrl_rf_wakeup(void);
extern void sctrl_sta_rf_sleep(void);
extern void sctrl_sta_rf_wakeup(void);
extern void sctrl_sta_ps_init(void);
extern void sctrl_flash_select_dco(void);
extern UINT8 sctrl_if_rf_sleep(void);
extern UINT32 charger_is_full(void);
extern UINT32 usb_power_is_pluged(void);
#endif // _SCTRL_PUB_H_
