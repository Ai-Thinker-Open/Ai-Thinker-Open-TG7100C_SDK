#include <stdio.h>
#include "board.h"
#include "lega_cm4.h"
#include "lega_common.h"
#include "systick_delay.h"
#include "lega_uart.h"
#include "lega_wdg.h"
#include "lega_flash.h"
#include "lega_common.h"
#include "lega_wlan_api.h"

#define EFUSE_SEL 0xd6

#define SEL_VIRGIN 0x00
#define SEL_MAC1 0x01
#define SEL_MAC2 0x03


#define SYS_APP_VERSION_SEG __attribute__((section("app_version_sec")))
SYS_APP_VERSION_SEG const char app_version[] = SYSINFO_APP_VERSION;

extern hal_wifi_module_t sim_aos_wifi_lega;
extern void NVIC_init();
extern int soc_pre_init(void);

#ifdef ALIOS_SUPPORT
extern void ota_roll_back_pro(void);
#endif

static void efuse_read(uint8_t *data,uint8_t addr,uint8_t len)
{
    for (int i = 0; i < len; i++)
    {
        data[i] = lega_efuse_byte_read(addr + i);
    }
}
void efuse_info_update_cb(efuse_info_t *efuse_info)
{
    printf("efuse_info_update\r\n");
    if (lega_efuse_byte_read(EFUSE_SEL) == SEL_MAC1)
    {
        efuse_read(efuse_info->mac_addr2,0x90,6);
    }else if(lega_efuse_byte_read(EFUSE_SEL) == SEL_MAC2)
    {
        efuse_read(efuse_info->mac_addr2,0xC8,6);
    }else{
        printf("empty mac\r\n");
    }
    efuse_read(&(efuse_info->freq_err),0x96,1);
    efuse_read(&(efuse_info->tmmt1),0x97,1);
    efuse_read(&(efuse_info->tmmt2),0x98,1);
    efuse_read(efuse_info->cal_tx_pwr2,0xAC,6);
    efuse_read(efuse_info->cal_tx_evm2,0xB5,6);

    efuse_read(efuse_info->cus_tx_pwr,0x99,19);
    efuse_read(efuse_info->cus_tx_total_pwr,0xB2,3);
}
static void wifi_common_init()
{
    printf("start------wifi_hal\r\n");
    lega_wlan_register_efuse_info_update_cb(efuse_info_update_cb);
    hal_wifi_register_module(&sim_aos_wifi_lega);
    hal_wifi_init();
}


/***********************************************************
* init IRQ, set priority and enable IRQ
*
**********************************************************/
void NVIC_init()
{
    //set irq priority, default set configLIBRARY_NORMAL_INTERRUPT_PRIORITY
    NVIC_SetPriority(UART0_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(UART1_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(UART2_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(CEVA_RW_IP_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(D_APLL_UNLOCK_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(D_SX_UNLOCK_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SLEEP_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(WDG_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(FLASH_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(GPIO_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(TIMER_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(CRYPTOCELL310_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(DMA_CTRL_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SPI0_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SPI1_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SPI2_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(I2C0_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(I2C1_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SDIO_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(PLF_WAKEUP_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
}

#ifdef ALIOS_SUPPORT
uart_dev_t uart_0;
void hal_uart1_callback_handler(char ch);
#endif

void uart_init(void)
{
#ifdef ALIOS_SUPPORT
    //uart_0.port=LEGA_UART1_INDEX;
    uart_0.port = PORT_UART_STD;  /*logic port*/
#ifdef HIGHFREQ_MCU160_SUPPORT
    uart_0.config.baud_rate=UART_BAUDRATE_1000000;
#else
    uart_0.config.baud_rate=UART_BAUDRATE_115200;
#endif
    uart_0.config.data_width = DATA_8BIT;
    uart_0.config.flow_control = FLOW_CTRL_DISABLED;
    uart_0.config.parity = PARITY_NO;
    uart_0.config.stop_bits = STOP_1BIT;
    //uart_0.priv = (void *)(hal_uart1_callback_handler);
    hal_uart_init(&uart_0);
#endif
}

#ifdef HIGHFREQ_MCU160_SUPPORT
//all peripheral reinit code should place here
void peripheral_reinit(void)
{
    uart_init();
}
#endif

#ifdef SYSTEM_RECOVERY
lega_wdg_dev_t lega_wdg;
void wdg_init(void)
{
    lega_wdg.port = 0;
    lega_wdg.config.timeout = WDG_TIMEOUT_MS;
    lega_wdg_init(&lega_wdg);
}
#endif
/***********************************************************
*  init device, such as irq, system clock, uart
**********************************************************/

uint32_t system_bus_clk  = SYSTEM_BUS_CLOCK_INIT;
uint32_t system_core_clk = SYSTEM_CORE_CLOCK_INIT;

void lega_devInit()
{
#ifdef _SPI_FLASH_ENABLE_
    lega_flash_init();
#endif

#ifdef ALIOS_SUPPORT
    ota_roll_back_pro();
#endif

#ifdef SYSTEM_RECOVERY
    wdg_init();
#endif

    NVIC_init();

#ifdef DCDC_PFMMODE_CLOSE
    lega_drv_close_dcdc_pfm();
#endif

    lega_drv_rco_cal();

    SysTick_Config(SYSTEM_CORE_CLOCK/RHINO_CONFIG_TICKS_PER_SECOND);

    //init uart
    uart_init();

#ifdef CFG_MIMO_UF
    //config to support smartconfig in MIMO scenario
    //lega_wlan_smartconfig_mimo_enable();
#endif

#ifdef ALIOS_SUPPORT
    hw_start_hal();
#endif
}

/**************************************************
*  after task run use board_sys_init to init board
**************************************************/
int board_after_init(void)
{
    lega_devInit();

    //hw_start_hal();
    //NVIC_init();

    tcpip_init( NULL, NULL );
    wifi_common_init();
    //init_uwifi();

    return 0;
}

/**************************************************
*  before task run use board_sys_init to init board
**************************************************/
void board_init(void)
{
#ifdef LEGA_A0V1
    // Clear RFA Only Mode
    REG_PMU_CTRL &= ~ENABLE_RFA_DEBUG;
    REG_WR(SYS_REG_BASE_FLASH_CLK, 0x01); //26MHz flash,default 13MHz
#endif
    flash_partition_init();
}
