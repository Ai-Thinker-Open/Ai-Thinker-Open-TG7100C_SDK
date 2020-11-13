#include "hal/soc/soc.h"
#include "include.h"
#include "spi_pub.h"

int32_t hal_spi_init(spi_dev_t *spi)
{
	uint32_t ret;
	
	if(spi->config.mode == HAL_SPI_MODE_MASTER)
	{
		ret = bk_spi_master_init(spi->config.freq, spi->port, 0);
	}
	else if(spi->config.mode == HAL_SPI_MODE_SLAVE)
	{
		ret = bk_spi_slave_init(spi->config.freq, spi->port, 0);
	}

	return ret;
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	int len;
    struct spi_message spi_msg;
	
	spi_msg.send_buf = data;
	spi_msg.send_len = size;
	
	spi_msg.recv_buf = NULL;
	spi_msg.recv_len = 0;
	
	if(spi->config.mode == HAL_SPI_MODE_MASTER)
	{
		aos_cli_printf("master send message\r\n");
		
		len = bk_spi_master_xfer(&spi_msg);
		
		return len;
	}
	else if(spi->config.mode == HAL_SPI_MODE_SLAVE)
	{
	
		aos_cli_printf("slave send message\r\n");
		
		len = bk_spi_slave_xfer(&spi_msg);
		
		return len;
		
	}
	else
	{
		return 0;		
	}
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
	int len;
    struct spi_message spi_msg;
	
	spi_msg.send_buf = NULL;
	spi_msg.send_len = 0;
	
	spi_msg.recv_buf = data;
	spi_msg.recv_len = size;
	
	if(spi->config.mode == HAL_SPI_MODE_MASTER)
	{
		len = bk_spi_master_xfer(&spi_msg);

		return len;
	}
	else if(spi->config.mode == HAL_SPI_MODE_SLAVE)
	{
		len = bk_spi_slave_xfer(&spi_msg);

		return len;
	}
	else
	{	
		return 0;
	}

}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
    struct spi_message spi_msg;
	
	spi_msg.send_buf = tx_data;
	spi_msg.send_len = size;
	
	spi_msg.recv_buf = rx_data;
	spi_msg.recv_len = size;
	
	if(spi->config.mode == HAL_SPI_MODE_MASTER)
	{
		bk_spi_master_xfer(&spi_msg);
	}
	else if(spi->config.mode == HAL_SPI_MODE_SLAVE)
	{
		bk_spi_slave_xfer(&spi_msg);
	}
	return 0;
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
	if(spi->config.mode == HAL_SPI_MODE_MASTER)
	{
		bk_spi_master_deinit();
	}
	else if(spi->config.mode == HAL_SPI_MODE_SLAVE)
	{
		bk_spi_slave_deinit();
	}
}

