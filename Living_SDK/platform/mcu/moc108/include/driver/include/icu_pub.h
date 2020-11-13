#ifndef _ICU_PUB_H_
#define _ICU_PUB_H_

#define ICU_FAILURE                (1)
#define ICU_SUCCESS                (0)

#define ICU_DEV_NAME                "icu"

#define ICU_CMD_MAGIC              (0xe220000)
enum
{
    CMD_ICU_CLKGATING_DISABLE = ICU_CMD_MAGIC + 1,
    CMD_ICU_CLKGATING_ENABLE,
    CMD_ICU_INT_DISABLE,
    CMD_ICU_INT_ENABLE,
    CMD_ICU_GLOBAL_INT_DISABLE,
    CMD_ICU_GLOBAL_INT_ENABLE,
    CMD_GET_INTR_STATUS,
    CMD_CLR_INTR_STATUS,
    CMD_GET_INTR_RAW_STATUS,
    CMD_CLR_INTR_RAW_STATUS,
    CMD_CLK_PWR_DOWN,
    CMD_CLK_PWR_UP,
    CMD_CONF_PWM_PCLK,
    CMD_CONF_PWM_LPOCLK,
    CMD_TL410_CLK_PWR_DOWN,
    CMD_TL410_CLK_PWR_UP,
    CMD_CONF_PCLK_26M,
    CMD_CONF_PCLK_DCO,
    CMD_SET_JTAG_MODE,
    CMD_GET_JTAG_MODE
};

/*CMD_CONF_PCLK*/
#define PCLK_POSI                                  (0)

/*CMD_CONF_PWM_PCLK, CMD_CONF_PWM_LPOCLK*/
// *param = channel_id

/*CMD_CLK_PWR_DOWN CMD_CLK_PWR_UP*/
#define PWD_FFT_CLK_BIT                      (1 << 19)
#define PWD_USB_CLK_BIT                      (1 << 18)
#define PWD_SDIO_CLK_BIT                     (1 << 17)
#define PWD_TL410_WATCHDOG_BIT               (1 << 16)
#define PWD_AUDIO_CLK_BIT                    (1 << 15)
#define PWD_PWM5_CLK_BIT                     (1 << 14)
#define PWD_PWM4_CLK_BIT                     (1 << 13)
#define PWD_PWM3_CLK_BIT                     (1 << 12)
#define PWD_PWM2_CLK_BIT                     (1 << 11)
#define PWD_PWM1_CLK_BIT                     (1 << 10)
#define PWD_PWM0_CLK_BIT                     (1 <<  9)
#define PWD_ARM_WATCHDOG_CLK_BIT             (1 <<  8)
#define PWD_SARADC_CLK_BIT                   (1 <<  7)
#define PWD_SPI_CLK_BIT                      (1 <<  6)
#define PWD_I2C2_CLK_BIT                     (1 <<  5)
#define PWD_I2S_PCM_CLK_BIT                  (1 <<  4)
#define PWD_IRDA_CLK_BIT                     (1 <<  3)
#define PWD_I2C1_CLK_BIT                     (1 <<  2)
#define PWD_UART2_CLK_BIT                    (1 <<  1)
#define PWD_UART1_CLK_BIT                    (1 <<  0)

/* CMD_ICU_CLKGATING_DISABLE CMD_ICU_CLKGATING_ENABLE*/
#define CLKGATE_MAC_AHB_BIT                      (1 << 16)
#define CLKGATE_FFT_AHB_BIT                      (1 << 15)
#define CLKGATE_USB_AHB_BIT                      (1 << 14)
#define CLKGATE_SDIO_AHB_BIT                     (1 << 13)
#define CLKGATE_SARADC_APB_BIT                   (1 << 12)
#define CLKGATE_AUDIO_APB_BIT                    (1 << 11)
#define CLKGATE_PWM_APB_BIT                      (1 << 10)
#define CLKGATE_WATCHDOG_APB_BIT                 (1 << 9)
#define CLKGATE_GPIO_APB_BIT                     (1 << 8)
#define CLKGATE_SPI_APB_BIT                      (1 << 7)
#define CLKGATE_I2C2_APB_BIT                     (1 << 6)
#define CLKGATE_I2S_PCM_APB_BIT                  (1 << 5)
#define CLKGATE_IRDA_APB_BIT                     (1 << 4)
#define CLKGATE_I2C1_APB_BIT                     (1 << 3)
#define CLKGATE_UART2_APB_BIT                    (1 << 2)
#define CLKGATE_UART1_APB_BIT                    (1 << 1)
#define CLKGATE_ICU_APB_BIT                      (1 << 0)

/* CMD ICU_TL410_CLK_PWD*/
#define PWD_TL410_CLK_BIT                    (1 <<  0)

/* ICU_JTAG_SELECT */
#define JTAG_ARM_MODE                            0
#define JTAG_TL410_MODE                          1

/*CMD_ICU_INT_DISABLE CMD_ICU_INT_ENABLE*/
#define FIQ_DPLL_UNLOCK_BIT                  (1 << 28) 
#define FIQ_SPI_DMA_BIT                      (1 << 27) 
#define FIQ_MAC_WAKEUP_BIT                   (1 << 26) 
#define FIQ_MAILBOX1_BIT                     (1 << 25) 
#define FIQ_MAILBOX0_BIT                     (1 << 24) 
#define FIQ_SDIO_DMA_BIT                     (1 << 23) 
#define FIQ_MAC_GENERAL_BIT                  (1 << 22) 
#define FIQ_MAC_PROT_TRIGGER_BIT             (1 << 21) 
#define FIQ_MAC_TX_TRIGGER_BIT               (1 << 20) 
#define FIQ_MAC_RX_TRIGGER_BIT               (1 << 19) 
#define FIQ_MAC_TX_RX_MISC_BIT               (1 << 18) 
#define FIQ_MAC_TX_RX_TIMER_BIT              (1 << 17) 
#define FIQ_MODEM_BIT                        (1 << 16) 
#define IRQ_GDMA_BIT                         (1 << 15) 
#define IRQ_FFT_BIT                          (1 << 14) 
#define IRQ_USB_BIT                          (1 << 13) 
#define IRQ_SDIO_BIT                         (1 << 12) 
#define IRQ_SARADC_BIT                       (1 << 11) 
#define IRQ_AUDIO_BIT                        (1 << 10) 
#define IRQ_PWM_BIT                          (1 << 9) 
#define IRQ_TL410_WATCHDOG_BIT               (1 << 8) 
#define IRQ_GPIO_BIT                         (1 << 7) 
#define IRQ_SPI_BIT                          (1 << 6) 
#define IRQ_I2C2_BIT                         (1 << 5) 
#define IRQ_I2S_PCM_BIT                      (1 << 4) 
#define IRQ_IRDA_BIT                         (1 << 3) 
#define IRQ_I2C1_BIT                         (1 << 2) 
#define IRQ_UART2_BIT                        (1 << 1) 
#define IRQ_UART1_BIT                        (1 << 0) 

/* CMD_ICU_GLOBAL_INT_DISABLE CMD_ICU_GLOBAL_INT_ENABLE*/
#define GINTR_FIQ_BIT                            (1 << 1)
#define GINTR_IRQ_BIT                            (1 << 0)

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void icu_init(void);
extern void icu_exit(void);

#endif //_ICU_PUB_H_ 

