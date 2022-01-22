/**
 ****************************************************************************************
 *
 * @file lega_cm4.h
 *
 * @brief define arm cm4 SOC architecture
 *
 * Copyright (C) ASR
 *
 ****************************************************************************************
 */


/*************************   **************************************/
#ifndef __LEGA_CM4_H__
#define __LEGA_CM4_H__

#ifdef __cplusplus
 extern "C" {
#endif

#define __CM0_REV                 0 /*!< Core Revision r0p0                            */
#define __MPU_PRESENT             0 /*!< legawifi do not provide MPU                  */
#define __NVIC_PRIO_BITS          3 /*!< legawifi uses 3 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0 /*!< Set to 1 if different SysTick Config is used  */
//hightest interrupt priority is configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY&((1<<__NVIC_PRIO_BITS)-1)=5
//lowest interrupt priority is configLIBRARY_LOWEST_INTERRUPT_PRIORITY&((1<<__NVIC_PRIO_BITS)-1)=7
//set normal interrupt priority 6
#define configLIBRARY_NORMAL_INTERRUPT_PRIORITY 6

typedef enum IRQn
{
/**************   Processor Exceptions Numbers ******************************************/
  NonMaskableInt_IRQn           = -14,      /*!< 2 Non Maskable Interrupt                         */
  MemoryManagement_IRQn         = -12,      /*!< 4 Cortex-M3 Memory Management Interrupt          */
  BusFault_IRQn                 = -11,      /*!< 5 Cortex-M3 Bus Fault Interrupt                  */
  UsageFault_IRQn               = -10,      /*!< 6 Cortex-M3 Usage Fault Interrupt                */
  SVCall_IRQn                   = -5,       /*!< 11 Cortex-M3 SV Call Interrupt                   */
  DebugMonitor_IRQn             = -4,       /*!< 12 Cortex-M3 Debug Monitor Interrupt             */
  PendSV_IRQn                   = -2,       /*!< 14 Cortex-M3 Pend SV Interrupt                   */
  SysTick_IRQn                  = -1,       /*!< 15 Cortex-M3 System Tick Interrupt               */
/******   Interrupt Numbers *******************************************************/
  CEVA_RW_IP_IRQn               = 0,        /*!< CEVA RW IP Interrupt                             */
  SLEEP_IRQn                    = 1,        /*!< Sleep Wake-Up Interrupt                          */
  WDG_IRQn                      = 2,        /*!< Window WatchDog                                  */
  FLASH_IRQn                    = 3,        /*!< FLASH Interrupt                                  */
  GPIO_IRQn                     = 4,        /*!< GPIO Interrupt                                   */
  TIMER_IRQn                    = 5,        /*!< Timer Interrupt                                  */
  CRYPTOCELL310_IRQn            = 6,        /*!< CryptoCell 310 Interrupt                         */
  DMA_CTRL_IRQn                 = 7,        /*!< Generic DMA Ctrl Interrupt                       */
  UART0_IRQn                    = 8,        /*!< UART0 Interrupt                                  */
  UART1_IRQn                    = 9,        /*!< UART1 Interrupt                                  */
  UART2_IRQn                    = 10,       /*!< UART2 Interrupt                                  */
  SPI0_IRQn                     = 11,       /*!< SPI0 Interrupt                                   */
  SPI1_IRQn                     = 12,       /*!< SPI1 Interrupt                                   */
  SPI2_IRQn                     = 13,       /*!< SPI2                                             */
  I2C0_IRQn                     = 14,       /*!< I2C0 Interrupt                                   */
  I2C1_IRQn                     = 15,       /*!< I2C1 Interrupt                                   */
  SDIO_IRQn                     = 16,       /*!< SDIO Combined Interrupt                          */
  D_APLL_UNLOCK_IRQn            = 17,       /*!< RF added: D_APLL_UNLOCK Interrupt                */
  D_SX_UNLOCK_IRQn              = 18,       /*!< RF added: D_SX_UNLOCK Interrupt                  */
  AUX_ADC_IRQn                  = 20,       /*!< ADC Interrupt                                    */
  PLF_WAKEUP_IRQn               = 23,       /*!< WiFi Platform Wake-Up Interrupt                  */
} IRQn_Type;


#ifdef LEGA_CM4
#include "core_cm4.h"
#endif

#include <stdint.h>

// ---------------------------------------------------------------------------
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
#define XTAL_26M               26000000

extern uint32_t system_bus_clk;
extern uint32_t system_core_clk;

#if defined LEGA_A0V1
#define SYSTEM_CLOCK           (26000000)
#elif defined LEGA_A0V2
#define SYSTEM_CORE_CLOCK      system_core_clk
#define SYSTEM_CLOCK           system_bus_clk
#endif

#define SYSTEM_CLOCK_NORMAL    (80000000)

#define SYSTEM_CORE_CLOCK_INIT  ( 52000000)
#define SYSTEM_CORE_CLOCK_HIGH  (160000000)

#define SYSTEM_BUS_CLOCK_INIT   (52000000)
#define SYSTEM_BUS_CLOCK_HIGH   (80000000)

#define HAPS_FPGA 0
#define V7_FPGA 1
#define FPGA_PLATFORM V7_FPGA

#if (defined LEGA_A0V1)
#define PINMUX_CTRL_REG0                0x40000038 //pad0-7
#define PINMUX_CTRL_REG1                0x4000003C //pad8-15
#define PINMUX_CTRL_REG2                0x40000048 //pad16-23
#define PINMUX_CTRL_REG3                0x4000004C //pad24-31

#elif (defined LEGA_A0V2)
#define PINMUX_CTRL_REG0                0x40000004 //pad0-7
#define PINMUX_CTRL_REG1                0x40000008 //pad8-15
#define PINMUX_CTRL_REG2                0x4000000C //pad16-23
#define PINMUX_CTRL_REG3                0x40000010 //pad24-31
#endif

#define SYS_REG_BASE                    0x40000000
#define SYS_REG_BASE_FLASH_CLK          ((SYS_REG_BASE + 0x808))
#define PERI_CLK_CFG                    (SYS_REG_BASE + 0x850)
#define SYS_REG_BASE_C310_CLK           ((SYS_REG_BASE + 0x900))
#define REG_INTERRUPT_ENABLE            (SYS_REG_BASE + 0x944)

#define SYS_CRM_SYS_CLK_CTRL1           *((volatile uint32_t *)(SYS_REG_BASE + 0x950))
#define SYS_CRM_REG_0x804               *((volatile uint32_t *)(SYS_REG_BASE + 0x804))
#define SYS_CRM_UART2_FRAC_DIV          *((volatile uint32_t *)(SYS_REG_BASE + 0x82C))

#define ALWAYS_ON_REGFILE           0x40000A00
#define REG_AHB_BUS_CTRL            *((volatile uint32_t *)(ALWAYS_ON_REGFILE + 0x90))

#define REG_PLF_WAKEUP_INT_EN       (0x1<<23)
#define SDIO_HCLK_EN                    (1 << 4)
#define PWM_CLK_EN                      (1 << 2)
#define WDG_CLK_EN                      (1 << 1)
#define TIMER_SCLK_EN                   0x1

#define SYS_CRM_WIFI_BLK_CLK        *((volatile uint32_t *)(SYS_REG_BASE + 0x85C))
#define MDM_CLKGATEFCTRL0_ADDR      0x60C00874
#define MDM_CLKGATEFCTRL0           *((volatile uint32_t *)(MDM_CLKGATEFCTRL0_ADDR))
#define RTC_REG_RCO32K_ADDR         0x40000A44
#define RTC_REG_RCO32K              *((volatile uint32_t *)(RTC_REG_RCO32K_ADDR))
#define SYS_CRM_CLR_HCLK_REC        *((volatile uint32_t *)(SYS_REG_BASE + 0x844))

#define TRX_PD_CTRL1_REG_ADDR       0x06
#define TRX_PD_CTRL2_REG_ADDR       0x07
#define APLL_PD_CTRL_REG_ADDR       0x0D
#define APLL_RST_CTRL_REG_ADDR      0x0E
#define XO_PD_CTRL_REG_ADDR         0x0F
#define APLL_CLK_PHY_REG_ADDR       0x6B
#define APLL_FCAL_FSM_CTRL_ADDR     0x6E

#ifdef LEGA_A0V1
#define _FLASH_DIRECT_BOOT_EN_
#endif

//efuse memory
typedef struct
{
    uint8_t mac_addr0[6];           //0x90-0x95
    uint8_t freq_err;               //0x96
    uint8_t tmmt1;                  //0x97
    uint8_t tmmt2;                  //0x98
    uint8_t cus_tx_pwr[19];         //0x99-0xab
    uint8_t cal_tx_pwr0[6];         //0xac-0xb1
    uint8_t cus_tx_total_pwr[3];    //0xb2-0xb4
    uint8_t cal_tx_evm0[6];         //0xb5-0xba
    uint8_t reserved1[5];           //0xbb-0xbf
    uint8_t mac_addr1[6];           //0xc0-0xc5
    uint8_t mac_addr2[6];           //0xc6-0xcb
    uint8_t cal_tx_pwr1[6];         //0xcc-0xd1
    uint8_t cal_tx_evm1[6];         //0xd2-0xd7
    uint8_t cal_tx_pwr2[6];         //0xd8-0xdd
    uint8_t cal_tx_evm2[6];         //0xde-0xe3
    uint8_t reserved2[12];          //0xe4-0xef
}efuse_info_t;
#define EFUSE_INFO_START_ADDR       0x90
#define EFUSE_INFO_LEN              (sizeof(efuse_info_t))
#define EFUSE_INFO_CHIP_TYPE_ADDR   0x1F6

#ifdef _FLASH_DIRECT_BOOT_EN_
#define BOOTLOADER_FLASH_START_ADDR 0x00000000
#define INFO_FLASH_START_ADDR       0x00010000
#define APP_FLASH_START_ADDR        0x00040000
#define OTA_FLASH_START_ADDR        0x00100000
#define KV_FLASH_START_ADDR         0x001E0000
#define OLL_FLASH_START_ADDR         0x001E4000
#else
#define BOOTLOADER_FLASH_START_ADDR 0x10000000
#ifdef VENDOR_MXCHIP
#define INFO_FLASH_START_ADDR       0x10005000
#define KV_FLASH_START_ADDR         0x10006000
#define APP_FLASH_START_ADDR        0x1000A000
#define OTA_FLASH_START_ADDR        0x10100000
#define OLL_FLASH_START_ADDR        0x101E4000
#else
#define INFO_FLASH_START_ADDR       0x10010000
#define APP_FLASH_START_ADDR        0x10040000
#define OTA_FLASH_START_ADDR        0x10100000
#define KV_FLASH_START_ADDR         0x101E0000
#define OLL_FLASH_START_ADDR        0x101E4000
#endif
#endif

#ifdef VENDOR_MXCHIP
#define BOOTLOADER_MAX_SIZE         0x5000
#define INFO_MAX_SIZE               0x1000
#define KV_MAX_SIZE                 0x4000
#define APP_MAX_SIZE                0xF6000
#define OTA_MAX_SIZE                0xA4000
#define OLL_MAX_SIZE                0x1000
#else
#define BOOTLOADER_MAX_SIZE         0x10000
#define INFO_MAX_SIZE               0x1000
#define KV_MAX_SIZE                 0x20000
#define APP_MAX_SIZE                0xC0000
#define OTA_MAX_SIZE                0xC0000
#define OLL_MAX_SIZE                0x1000
#endif

#ifdef ALIOS_SUPPORT
#include <k_api.h>
#define lega_intrpt_enter() krhino_intrpt_enter()
#define lega_intrpt_exit() krhino_intrpt_exit()
#else
#define lega_intrpt_enter()
#define lega_intrpt_exit()
#endif

#endif
