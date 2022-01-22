/**
 * @file spi.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_SPI_H
#define HAL_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_spi SPI
 *  spi hal API.
 *
 *  @{
 */

#include <stdint.h>

/* SPI interrupt events */
typedef enum __SPI_INT_EVENTS__{
    SPI_INT_CRC,       /* crc check error */
    SPI_INT_FE,        /* Frame error */
    SPI_INT_OV,        /* Overrun error */
    SPI_INT_RC,        /* Receive complete */
    SPI_INT_TC,        /* Transmit complete */
    SPI_INT_TXE,       /* Transmit data register empty */
    SPI_INT_DMA_TH,    /* DMA transmit half */
    SPI_INT_DMA_TC,    /* DMA transmit complete */
    SPI_INT_DMA_TE,    /* DMA transmit error */
    SPI_INT_DMA_RH,    /* DMA receive half */
    SPI_INT_DMA_RC,    /* DMA receive complete */
    SPI_INT_DMA_RE     /* DMA receive error */
} spi_event_t;

/* SPI data in DMA mode interrupt callback */
typedef struct {
    uint32_t size;  /* data size */
    void *data;  /* spi data */
} spi_data_t;

/* SPI interrupt callback */
typedef void (*hal_spi_cb_t)(spi_event_t event, void *data, uint32_t size);

/* Define the wait forever timeout macro */
#define HAL_WAIT_FOREVER 0xFFFFFFFFU

/* spi mode */
#define HAL_SPI_MODE_MASTER         1  /* spi communication is master mode */
#define HAL_SPI_MODE_SLAVE          2  /* spi communication is slave mode */

/* data size */
#define HAL_SPI_DATA_WIDTH_4B       4  /* spi data width is 4 bits */
#define HAL_SPI_DATA_WIDTH_7B       7  /* spi data width is 7 bits */
#define HAL_SPI_DATA_WIDTH_8B       8  /* spi data width is 8 bits */
#define HAL_SPI_DATA_WIDTH_9B       9  /* spi data width is 9 bits */
#define HAL_SPI_DATA_WIDTH_16B      16 /* spi data width is 16 bits */

/* data shift mode */
#define HAL_SPI_DATA_SHIFT_MSB      0  /* spi data shift msb */
#define HAL_SPI_DATA_SHIFT_LSB      1  /* spi data shift lsb */

/* clock mode */
#define HAL_SPI_CLK_POL_LOW_PHA_1E  0  /* spi clock polarity low, phase 1 edge */
#define HAL_SPI_CLK_POL_LOW_PHA_2E  1  /* spi clock polarity low, phase 2 edge */
#define HAL_SPI_CLK_POL_HIGH_PHA_1E 2  /* spi clock polarity high, phase 1 edge */
#define HAL_SPI_CLK_POL_HIGH_PHA_2E 3  /* spi clock polarity high, phase 2 edge */

/* Define spi config args */
typedef struct {
    uint8_t mode;           /* spi communication mode */
    uint8_t data_width;     /* spi data width */
    uint8_t data_shift;     /* spi data shift mode */
    uint8_t clock_mode;     /* spi clock mode */
    uint32_t freq;          /* communication frequency Hz */
} spi_config_t;

/* Define spi dev handle */
typedef struct {
    uint8_t       port;   /**< spi port */
    spi_config_t  config; /**< spi config */
    void         *priv;   /**< priv data */
} spi_dev_t;

/**
 * Initialises the SPI interface for a given SPI device
 *
 * @param[in]  spi  the spi device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_spi_init(spi_dev_t *spi);

/**
 * Spi send
 *
 * @param[in]  spi      the spi device
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * spi_recv
 *
 * @param[in]   spi      the spi device
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 * @param[in]   timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * spi send data and recv
 *
 * @param[in]  spi      the spi device
 * @param[in]  tx_data  spi send data
 * @param[out] rx_data  spi recv data
 * @param[in]  size     spi data to be sent and recived
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0, on success,  otherwise is error
 */
int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout);
/**
 *
 * @param [in]   spi          the SPI interface
 * @param [in]   cb           Non-zero pointer is the transceive callback handler;
 *                            NULL pointer for transceive unregister operation
 *                            spi in cb must be the same pointer with spi pointer passed to hal_spi_send_recv_cb_reg
 *                            driver must notify upper layer by calling cb if data transceive done or error in SPI's hw
 * @return 0: on success, negative no.: if an error occured with any step
 */
int32_t hal_spi_send_recv_cb_reg(spi_dev_t *spi, hal_spi_cb_t cb);

/**
 * De-initialises a SPI interface
 *
 *
 * @param[in]  spi  the SPI device to be de-initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_spi_finalize(spi_dev_t *spi);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HAL_SPI_H */

