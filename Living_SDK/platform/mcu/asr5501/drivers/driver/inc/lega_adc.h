#ifndef _LEGA_ADC_H_
#define _LEGA_ADC_H_
#include <stdint.h>
#include "lega_cm4.h"

#define ADC_ENABLE

#if defined ADC_ENABLE

#define ADC_SAMPLE_1M    0
#define ADC_SAMPLE_500K  1
#define ADC_SAMPLE_250K  2
#define ADC_SAMPLE_125K  3

#define ADC_SAMPLE_SEL ADC_SAMPLE_125K

struct LEGA_ADC
{
    union
    {
        struct
        {
            __IO uint32_t adc_resv:24;
            __IO uint32_t adc_int_mode:1;
            __IO uint32_t adc_int_en:1;
            __IO uint32_t adc_int_clr:1;
            __IO uint32_t adc_resv1:5;
        }BITS_ADC_CTRL;
        __IO uint32_t ADC_CTRL;    /* adc control */
    };
    __IO uint32_t ADC_DATA;
};

#define ADC_REG_BASE 0x40019000
#define ADC ((struct LEGA_ADC *)(ADC_REG_BASE))

#define SYS_REG_BASE                   0x40000000

#define SYS_REG_BASE_CORE_CLK          ((SYS_REG_BASE + 0x804))
#define SYS_REG_BASE_FLASH_SEL_CLK     ((SYS_REG_BASE + 0x808))
#define SYS_REG_BASE_MAC_CORE_CLK      ((SYS_REG_BASE + 0x810))
#define SYS_REG_BASE_REF_ROOT_CLK      ((SYS_REG_BASE + 0x80C))
#define SYS_REG_BASE_AUX_REG_CLK       ((SYS_REG_BASE + 0x850))
#define SYS_REG_BASE_WIFI_CLK          ((SYS_REG_BASE + 0x85C))
#define SYS_REG_BASE_DIG32_SEL         ((SYS_REG_BASE + 0x908))

#define SYS_REG_BASE_PMU_CTRL          ((SYS_REG_BASE + 0xA20))
#define SYS_REG_BASE_AUXADC            ((SYS_REG_BASE + 0x19000))
#define SYS_REG_DCDC_REG3              ((SYS_REG_BASE + 0xA50))
#define SYS_REG_BASE_XOCTRL2           ((SYS_REG_BASE + 0xA70))
#define SYS_REG_BASE_XODCCA_CTRL       ((SYS_REG_BASE + 0xA74))

typedef void (*lega_adc_callback_func)(uint32_t *);


#define BIT(pos) (1U<<(pos))

/* Modem Config */
#define MDM_MEMCLKCTRL0_ADDR            0x60C00848
#define MDM_CLKGATEFCTRL0_ADDR          0x60C00874
#define RIU_AGCMEMBISTSTAT_ADDR         0x60C0B238
#define RC_SW_CTRL_ADDR             0x60C0C004
#define SPI_COMMAND                 RC_SW_CTRL_ADDR
#define SPI_RDATA                   RC_SW_CTRL_ADDR

#define ASSERT_ERR(cond) \
                 do{}while(0)


/// Length of a char in bytes
#define CHAR_LEN    1 //(CHAR_BIT/8)

/**
 ****************************************************************************************
 * @brief Converts a CPU pointer into a HW address
 * This macro is used to convert a SW pointer into the corresponding HW address. With CPUs
 * having native byte support, the value returned will be the same as the pointer passed.
 * With TL4, the value returned is the pointer multiplied by 2.
 * @param[in] ptr Pointer to be converted
 * @return The corresponding HW address
 ****************************************************************************************
 */
#define CPU2HW(ptr) (((uint32_t)(ptr)) * CHAR_LEN)

/**
 ****************************************************************************************
 * @brief Converts a HW address into a CPU pointer
 * This macro is doing the reverse operation as @ref CPU2HW.
 * @param[in] ptr Address to be converted
 * @return The corresponding CPU pointer
 ****************************************************************************************
 */
#define HW2CPU(ptr) ((void *)(((uint32_t)(ptr)) / CHAR_LEN))


/*
 * MACROS
 ****************************************************************************************
 */
/// Macro to read a platform register
#define REG_PL_RD(addr)              (*(volatile uint32_t *)(HW2CPU(addr)))

/// Macro to write a platform register
#define REG_PL_WR(addr, value)       (*(volatile uint32_t *)(HW2CPU(addr))) = (value)


typedef enum
{
    ADC_CHANNEL_NUM0,
    ADC_CHANNEL_NUM1,
    ADC_CHANNEL_NUM2,
    ADC_CHANNEL_NUM3,
    ADC_CHANNEL_NUM4,
    ADC_CHANNEL_NUM5,
    ADC_CHANNEL_NUM6,
    ADC_CHANNEL_NUM7,
}lega_adc_channel_t;

//typedef enum
//{
//    ADC_SAMPLE_1M = 0,
//    ADC_SAMPLE_500K,
//    ADC_SAMPLE_250K,
//    ADC_SAMPLE_125K,
//}lega_adc_sample_t;

typedef struct {
    uint32_t sampling_cycle;  /* sampling period in number of ADC clock cycles */
}lega_adc_config_t;

typedef struct {
    uint8_t      port;   /* adc port */
    lega_adc_config_t config; /* adc config */
    void        *priv;   /* priv data */
}lega_adc_dev_t;


extern uint16_t spi_mst_read(uint16_t addr);
extern void spi_mst_write(uint16_t addr, uint16_t data);

/**
 * Initialises a adc interface
 *
 * channnel pin which is set to adc sample.
 * reference voltage 1.6V  ADC resolution 12bit
 * @param[in]  channnel pin which is set to adc sample:rang0-7 (pad4-11)
 *
* @return  int mode:return 0; other:return adc value ,rang:0-4095
 */
int32_t lega_adc_init(lega_adc_dev_t *adc_config);

/**
 * founction for adc sample
 * channnel pin which is set to adc sample.
 * reference voltage 1.6V  ADC resolution 12bit
 * @param[in]  channnel pin which is set to adc sample:rang0-7 (pad4-11)
 *
 * @return  value:adc sample data rang:0-4095, -1: if an error occurred with any step
 */
int32_t lega_adc_get(lega_adc_dev_t *adc_config);

/**
 * Deinitialises a adc interface
 *
 * @param[in]  adc  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_adc_finalize(lega_adc_dev_t *adc_config);
#endif
#endif //_LEGA_ADC_H_
