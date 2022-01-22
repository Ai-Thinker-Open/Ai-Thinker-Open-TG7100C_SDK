/**
 * @file adc.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_ADC_H
#define HAL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_adc ADC
 *  ADC hal API.
 *
 *  @{
 */

#include <stdint.h>

/* Define the wait forever timeout macro */
#define HAL_WAIT_FOREVER 0xFFFFFFFFU
#define HAL_DMA_BUF_MIN_SIZE    200

/* ADC interrupt events */
typedef enum __ADC_INT_EVENTS__{
    ADC_INT_OV,         /* Overrun error */
    ADC_INT_EOS,        /* End of sample */
    ADC_INT_DMA_TRH,    /* DMA transceive half */
    ADC_INT_DMA_TRC,    /* DMA transceive complete */
    ADC_INT_DMA_TRE,    /* DMA transceive error */
} adc_event_t;

/* ADC data type */
typedef struct {
    uint32_t size;  /* sampled data size */
    void *data;     /* sampled data, aligned with resolution (until the next power of two) */
} adc_data_t;

/* ADC internal channels */
typedef enum {
    ADC_PORT_VREF = 24, /**< reference volage channel */
    ADC_PORT_VBG,       /**< bandgap volage channel */
    ADC_PORT_VCC,       /**< power supply volage channel */
}adc_port_num_t;

typedef enum {
    ADC_ONE_SHOT,	/**< Single time sampling */
    ADC_CONTINUE	/**< Continuous sampling */
}adc_sample_mode_t;

typedef enum {
    VOLT_ADC,  // single sample
    DMA_ADC,   // dma sample
}adc_sample_type_t;

/* Define ADC config args */
typedef struct {
    uint32_t sampling_cycle; /**< sampling period in number of ADC clock cycles */
    uint32_t sampling_freq;   /**< sampling frequency in Hz */
     adc_sample_mode_t mode; /* adc sampling mode */
     //adc_sample_type_t type; /* VOLT type or DMA type */
     //uint32_t dma_data_size; /* use dma type, should assign sample size */
     uint8_t sample_resolution; /* adc sampling resolution */
} adc_config_t;

/* Define ADC dev hal handle */
typedef struct {
    uint8_t      port;   /**< adc port */
    adc_config_t config; /**< adc config */
    void        *priv;   /**< priv data */
} adc_dev_t;

/* ADC interrupt callback */
typedef void (*hal_adc_cb_t)(adc_event_t event, void *data, uint32_t size);
//typedef void (*hal_adc_cb_t)(adc_event_t event, uint32_t *data, uint32_t size, void *arg);

/**
 * Initialises an ADC interface, Prepares an ADC hardware interface for sampling
 *
 * @param[in]  adc  the interface which should be initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_adc_init(adc_dev_t *adc);

/**
 * Takes a single sample from an ADC interface
 *
 * @note If the ADC is configured with mutiple channels, the result of each channel is
 *       copied to output buffer one by one according to the sequence of the channel registered,
 *       and each result takes sizeof(uint32_t) bytes.
 *
 * @param[in]   adc      the interface which should be sampled
 * @param[out]  output   pointer to a variable which will receive the sample
 * @param[in]   timeout  ms timeout
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout);


/**
 *  * ADC sampling use DMA mode
 *   
 *    @param[in]   adc           the ADC interface
 *    @param[in]   hal_adc_cb_t  interrupt callback
 *    @param[in]   data          adc data buffer
 *    @param[in]   size          data buffer size aligned with resolution (until the next power of two)
 *        
 *    @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_adc_start_DMA(adc_dev_t *adc, hal_adc_cb_t *cb, uint32_t *data, uint32_t size);

/**
 *  * ADC sampling cb register
 * @param [in]   adc          the ADC interface
 * @param [in]   cb           Non-zero pointer is the sample callback handler;
 *                            NULL pointer for send unregister operation
 *                            adc in cb must be the same pointer with adc pointer passed to hal_adc_sample_cb_reg
 *                            driver must notify upper layer by calling cb if ADC data is ready in HW or memory(DMA)
 * @return 0: on success, negative no.: if an error occured with any step
 */
int32_t hal_adc_sample_cb_reg(adc_dev_t *adc, hal_adc_cb_t cb);

/**
 * ADC calibration
 *
 * @param[in]   adc             the ADC interface
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_adc_calibration(adc_dev_t *adc);

/**
 * ADC sampling start
 *
 * @param[in]   adc             the ADC interface
 * @param[in]   data            adc data buffer
 * @param[in]   size            data buffer size aligned with resolution (until the next power of two)
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_adc_start(adc_dev_t *adc, void *data, uint32_t size);

/**
 * ADC sampling stop
 *
 * @param[in]   adc             the ADC interface
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_adc_stop(adc_dev_t *adc);

/**
 * De-initialises an ADC interface, Turns off an ADC hardware interface
 *
 * @param[in]  adc  the interface which should be de-initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_adc_finalize(adc_dev_t *adc);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HAL_ADC_H */

