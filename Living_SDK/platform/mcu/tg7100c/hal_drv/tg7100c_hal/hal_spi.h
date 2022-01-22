#ifndef __HAL_SPI_H
#define __HAL_SPI_H

#include <tg7100c_dma.h>
#include <hal/soc/soc.h>

typedef struct _spi_hw {
    uint8_t mode;
    uint8_t data_width;     /* spi data width */
    uint8_t data_shift;     /* spi data shift mode */
    uint32_t freq;
    uint8_t polar_phase;
    uint8_t tx_dma_ch;
    uint8_t rx_dma_ch;
    uint8_t pin_clk;
    uint8_t pin_cs;
    uint8_t pin_mosi;
    uint8_t pin_miso;
} spi_hw_t;

typedef struct {
    DMA_LLI_Ctrl_Type *ptxlli;
    DMA_LLI_Ctrl_Type *prxlli;
    uint8_t *RxData;
    uint32_t length;
    hal_spi_cb_t cb;
} spi_data_cb_t;

void hal_spi_hw_init(spi_hw_t *arg);
void hal_spi_dma_init(spi_hw_t *arg);
int hal_spi_dma_trans(spi_hw_t *arg, uint8_t *TxData, uint8_t *RxData, uint32_t Len, uint32_t timeout);
void hal_spi_hw_deinit(spi_hw_t *arg);
int hal_spi_dma_deinit(spi_hw_t *arg);
spi_data_cb_t *bl_spi_get_cb_ctx(void);
#endif
