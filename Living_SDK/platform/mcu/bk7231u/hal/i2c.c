#include "hal/soc/soc.h"
#include "include.h"
#include "i2c_pub.h"
#include "drv_model_pub.h"

static DD_HANDLE i2c1_hdl = DD_HANDLE_UNVALID;

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    uint32_t status, div;
	
	div = I2C_CLK_DIVID(i2c->config.freq);
	i2c1_hdl = ddev_open(I2C1_DEV_NAME, &status, div);

	return 0;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    I2C_OP_ST i2c_op;
	
	if(i2c1_hdl == DD_HANDLE_UNVALID)
	{
		return -1;
	}
	
	if(i2c->config.address_width == I2C_MEM_ADDR_SIZE_8BIT)
	{
		i2c_op.addr_width = ADDR_WIDTH_8;
	}
	else if(i2c->config.address_width == I2C_MEM_ADDR_SIZE_16BIT)
	{
		i2c_op.addr_width = ADDR_WIDTH_16;
	}
	i2c_op.op_addr  = dev_addr;
	i2c_op.salve_id = i2c->config.dev_addr;
	ddev_write(i2c1_hdl, data, size, (unsigned long)&i2c_op);

	return 0;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    I2C_OP_ST i2c_op;
	
	if(i2c1_hdl == DD_HANDLE_UNVALID)
	{
		return -1;
	}

	if(i2c->config.address_width == I2C_MEM_ADDR_SIZE_8BIT)
	{
		i2c_op.addr_width = ADDR_WIDTH_8;
	}
	else if(i2c->config.address_width == I2C_MEM_ADDR_SIZE_16BIT)
	{
		i2c_op.addr_width = ADDR_WIDTH_16;
	}
	i2c_op.op_addr  = dev_addr;
	i2c_op.salve_id = i2c->config.dev_addr;
	ddev_read(i2c1_hdl, data, size, (unsigned long)&i2c_op);

	return 0;
}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    ddev_close(i2c1_hdl);
	i2c1_hdl = DD_HANDLE_UNVALID;
	
	return 0;
}


