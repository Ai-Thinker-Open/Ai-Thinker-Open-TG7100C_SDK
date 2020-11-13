#include <stdio.h>
#include <stdint.h>
#include <hal/soc/soc.h>
#include <hal_spi.h>

spi_hw_t dev_spi;

int32_t hal_spi_init(spi_dev_t *spi)
{
    dev_spi.pin_cs = 2;
    dev_spi.pin_clk = 11;
    dev_spi.pin_mosi = 4;
    dev_spi.pin_miso = 1;
    dev_spi.mode = (spi->config).mode;
    dev_spi.freq = (spi->config).freq;
    dev_spi.polar_phase = 0;
    dev_spi.tx_dma_ch = 2;
    dev_spi.rx_dma_ch = 3;

    hal_spi_hw_init(&dev_spi);
    hal_spi_dma_init(&dev_spi);
    
    return 0;
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t rxdata[4];
    int ret;

    if (data == NULL || timeout < 0 || size < 0) {
        printf("not correct para \r\n");
    }

    ret = hal_spi_dma_trans(&dev_spi, (uint8_t *)data, rxdata, size, timeout);
    
    return ret;
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
    hal_spi_hw_deinit(&dev_spi);

    return 0;
}
