#include <stdio.h>
#include <stdint.h>
#include <hal/soc/soc.h>
#include <hal_spi.h>
#include <utils_log.h>

#define SPI_PIN_CS              (2)
#define SPI_PIN_CLK             (11)
#define SPI_PIN_MISO            (4)
#define SPI_PIN_MOSI            (1)

#define SPI_DMA_CH_RX           (3)
#define SPI_DMA_CH_TX           (2)

#define SPI_CFG_POLAR_PHASE     (0)

/*
 *  dma :  ch_2  ch_3
 *
 *  default :
 *      cs: 2
 *      clk:3
 *      mosi: 1
 *      miso: 0
 **/

/**
 * Initialises the SPI interface for a given SPI device
 *
 * @param[in]  spi  the spi device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_spi_init(spi_dev_t *spi)
{
    spi_hw_t *dev_spi;
    if (spi == NULL) {
        return -1;
    }
    spi->priv = aos_malloc(sizeof(spi_hw_t));
    if (NULL == spi->priv) {
        log_error("mem error.\r\n");
        return -1;
    }
    dev_spi = (spi_hw_t *)spi->priv;

    dev_spi->pin_cs      = SPI_PIN_CS;
    dev_spi->pin_clk     = SPI_PIN_CLK;
    dev_spi->pin_mosi    = SPI_PIN_MOSI;
    dev_spi->pin_miso    = SPI_PIN_MISO;
    dev_spi->tx_dma_ch   = SPI_DMA_CH_TX;
    dev_spi->rx_dma_ch   = SPI_DMA_CH_RX;

    dev_spi->data_width = (spi->config).data_width;
    dev_spi->data_shift = (spi->config).data_shift;
    dev_spi->polar_phase = (spi->config).clock_mode;
    dev_spi->mode        = (spi->config).mode;
    dev_spi->freq        = (spi->config).freq;

    hal_spi_hw_init(dev_spi);
    hal_spi_dma_init(dev_spi);
 
    return 0;
}

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
int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int ret;

    if (NULL == spi || data == NULL) {
        log_error("not init.\r\n");
        return -1;
    }

    if (data == NULL || timeout < 0 || size < 0) {
        printf("not correct para \r\n");
    }

    ret = hal_spi_dma_trans((spi_hw_t *)spi->priv, (uint8_t *)data, NULL, size, timeout);
    
    return ret;
}

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
int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int ret;

    if (NULL == spi || data == NULL) {
        log_error("not init.\r\n");
        return -1;
    }

    if (data == NULL || timeout < 0 || size < 0) {
        printf("not correct para \r\n");
    }

    ret = hal_spi_dma_trans((spi_hw_t *)spi->priv, NULL, data, size, timeout);
    
    return ret;
}

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
                          uint16_t size, uint32_t timeout)
{
    int ret;
    if (NULL == spi || tx_data == NULL || rx_data == NULL) {
        log_error("not init.\r\n");
        return -1;
    }
    ret = hal_spi_dma_trans((spi_hw_t *)spi->priv, (uint8_t *)tx_data, (uint8_t *)rx_data, size, timeout);
    return ret;
}

/**
 * register spi send recv cb
 *
 * @param [in]   spi          the SPI interface
 * @param [in]   cb           Non-zero pointer is the transceive callback handler;
 *                            NULL pointer for transceive unregister operation
 *                            spi in cb must be the same pointer with spi pointer passed to hal_spi_send_recv_cb_reg
 *                            driver must notify upper layer by calling cb if data transceive done or error in SPI's hw
 * @return 0: on success, negative no.: if an error occured with any step
 */
int32_t hal_spi_send_recv_cb_reg(spi_dev_t *spi, hal_spi_cb_t cb)
{
    spi_data_cb_t *ctx = NULL;

    if (NULL == spi) {
        log_error("not init.\r\n");
        return -1;
    }
   
    ctx = bl_spi_get_cb_ctx();

    ctx->cb = cb;
}

/**
 * De-initialises a SPI interface
 *
 *
 * @param[in]  spi  the SPI device to be de-initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_spi_finalize(spi_dev_t *spi)
{
    spi_hw_t *arg = (spi_hw_t *)spi->priv;
    
    if (spi == NULL) {
        return -1;
    }
    
    hal_spi_hw_deinit(arg);
    hal_spi_dma_deinit(arg);
    
    if (spi->priv) {
        aos_free(spi->priv);
        spi->priv = NULL;
    }

    return 0;
}

