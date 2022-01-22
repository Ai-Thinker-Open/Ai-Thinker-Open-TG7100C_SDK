#include <stdio.h>
#include <stdint.h>
#include "hal/soc/soc.h"
#include "tg7100c_i2c.h"
#include "bl_i2c.h"
#include "bl_timer.h"
#include "tg7100c_gpio.h"

#define PUT_UINT32_LE(n,b,i)                      \
{                                                 \
    (b)[(i)    ] = (uint8_t) ( (n)       );       \
    (b)[(i) + 1] = (uint8_t) ( (n) >>  8 );       \
    (b)[(i) + 2] = (uint8_t) ( (n) >> 16 );       \
    (b)[(i) + 3] = (uint8_t) ( (n) >> 24 );       \
}

int32_t I2C_MasterSend(I2C_ID_Type i2cNo, I2C_Transfer_Cfg *cfg, uint32_t timeout)
{
    uint8_t i;
    int64_t time_start, time_end;
    uint32_t temp = 0;
    uint32_t I2Cx = I2C_BASE;

    /* Check the parameters */
    CHECK_PARAM(IS_I2C_ID_TYPE(i2cNo));

    I2C_Disable(i2cNo);
    I2C_Init(i2cNo, I2C_WRITE, cfg);
    I2C_Enable(i2cNo);
    
    time_start =  bl2_timer_now_ms();
    /* Set I2C write data */
    for(i=0; i<cfg->dataSize; i++){
        temp += (cfg->data[i] << ((i % 4) * 8));
        if((i + 1) % 4 == 0){
            //time_start =  bl2_timer_now_ms(); 
            //timeOut = I2C_FIFO_STATUS_TIMEOUT;
            while(BL_GET_REG_BITS_VAL(BL_RD_REG(I2Cx, I2C_FIFO_CONFIG_1), I2C_TX_FIFO_CNT) == 0){
                time_end =  bl2_timer_now_ms();
                if (0xFFFFFFFF == timeout) {
                    continue;
                }
                if((time_end - time_start) >= timeout){
                    I2C_Disable(i2cNo);
                    return -1;
                }
            }
            BL_WR_REG(I2Cx, I2C_FIFO_WDATA, temp);
            temp = 0;
        }
    }
    if((cfg->dataSize%4) != 0){
        //timeOut = I2C_FIFO_STATUS_TIMEOUT;
        while(BL_GET_REG_BITS_VAL(BL_RD_REG(I2Cx, I2C_FIFO_CONFIG_1), I2C_TX_FIFO_CNT) == 0){
            time_end =  bl2_timer_now_ms();
            if (0xFFFFFFFF == timeout) {
                continue;
            }
            if((time_end - time_start) >= timeout){
                I2C_Disable(i2cNo);
                return -1;
            }
        }
        BL_WR_REG(I2Cx, I2C_FIFO_WDATA, temp);
    }

    //timeOut = I2C_FIFO_STATUS_TIMEOUT;
    while(I2C_IsBusy(i2cNo) || !I2C_TransferEndStatus(i2cNo)){
        time_end =  bl2_timer_now_ms();
        if (0xFFFFFFFF == timeout) {
            continue;
        }
        if((time_end - time_start) >= timeout){
            I2C_Disable(i2cNo);
            return -1;
        }
    }

    I2C_Disable(i2cNo);
    return 0;
}

int32_t I2C_MasterReceive(I2C_ID_Type i2cNo, I2C_Transfer_Cfg *cfg, uint32_t timeout)
{
    uint8_t i = 0;
    int64_t time_start, time_end;
    uint32_t temp = 0;
    uint32_t I2Cx = I2C_BASE;

    /* Check the parameters */
    CHECK_PARAM(IS_I2C_ID_TYPE(i2cNo));

    I2C_Disable(i2cNo);
    I2C_Init(i2cNo, I2C_READ, cfg);
    I2C_Enable(i2cNo);
    
    time_start = bl2_timer_now_ms();
    /* Read I2C data */
    while(cfg->dataSize - i >= 4){
        //time_start = bl2_timer_now_ms();
        //timeOut = I2C_FIFO_STATUS_TIMEOUT;
        while(BL_GET_REG_BITS_VAL(BL_RD_REG(I2Cx, I2C_FIFO_CONFIG_1), I2C_RX_FIFO_CNT) == 0){
            time_end =  bl2_timer_now_ms();
            if (0xFFFFFFFF == timeout) {
                continue;
            }
            if((time_end - time_start) >= timeout){
                I2C_Disable(i2cNo);
                return -1;
            }
        }
        temp = BL_RD_REG(I2Cx, I2C_FIFO_RDATA);
        PUT_UINT32_LE(temp, cfg->data, i);
        i += 4;
    }
    if(i < cfg->dataSize){
        //timeOut = I2C_FIFO_STATUS_TIMEOUT;
        while(BL_GET_REG_BITS_VAL(BL_RD_REG(I2Cx, I2C_FIFO_CONFIG_1), I2C_RX_FIFO_CNT) == 0){
            time_end =  bl2_timer_now_ms();
            if (0xFFFFFFFF == timeout) {
                continue;
            }
            if((time_end - time_start) >= timeout){
                I2C_Disable(i2cNo);
                return -1;
            }
        }
        temp = BL_RD_REG(I2Cx, I2C_FIFO_RDATA);
        while(i < cfg->dataSize){
            cfg->data[i] = (temp & 0xff);
            temp = (temp >> 8);
            i ++;
        }
    }

    //timeOut = I2C_FIFO_STATUS_TIMEOUT;
    while(I2C_IsBusy(i2cNo) || !I2C_TransferEndStatus(i2cNo)){
        time_end =  bl2_timer_now_ms();
        if (0xFFFFFFFF == timeout) {
            continue;
        }
        if((time_end - time_start) >= timeout){
            I2C_Disable(i2cNo);
            return -1;
        }
    }       

    I2C_Disable(i2cNo);
    return SUCCESS;
}

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    if (i2c == NULL || i2c->port != 0) {
        return -1;
    }
   
    i2c_gpio_init(i2c->port);
    i2c_set_freq(i2c->config.freq, 0);
    //I2C_BusyClr(i2c->port);
	return 0;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    I2C_Transfer_Cfg i2cCfg_send = {
        .slaveAddr = 0x50,
        .stopEveryByte = DISABLE,
        .subAddrSize = 0,
        .subAddr = 0x00,
        .dataSize = 0,
        .data = NULL,
    };

    if (i2c == NULL || data == NULL || size < 1) {
        return -1;
    }

    i2cCfg_send.slaveAddr = dev_addr;
    i2cCfg_send.subAddr = data[0];
    i2cCfg_send.subAddrSize = 0;
    i2cCfg_send.data = (uint8_t *)data;
    i2cCfg_send.dataSize = size;

    return I2C_MasterSend(I2C0_ID, &i2cCfg_send, timeout);
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    I2C_Transfer_Cfg i2cCfg_recv = {
        .slaveAddr = 0x50,
        .stopEveryByte = DISABLE,
        .subAddrSize = 0,
        .subAddr = 0x00,
        .dataSize = 0,
        .data = NULL,
    };

    if (i2c == NULL || data == NULL || size < 1) {
        return -1;
    }

    i2cCfg_recv.slaveAddr = dev_addr;
    i2cCfg_recv.subAddr = data[0];
    i2cCfg_recv.subAddrSize = 0;
    i2cCfg_recv.data = (uint8_t *)data;
    i2cCfg_recv.dataSize = size;

    return I2C_MasterReceive(I2C0_ID, &i2cCfg_recv, timeout);
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    I2C_Transfer_Cfg i2cCfg_send = {
        .slaveAddr = 0x50,
        .stopEveryByte = DISABLE,
        .subAddrSize = 2,
        .subAddr = 0x00,
        .dataSize = 0,
        .data = NULL,
    };

    if (i2c == NULL || data == NULL) {
        return -1;
    }

    i2cCfg_send.slaveAddr = dev_addr;
    i2cCfg_send.subAddr = mem_addr;
    i2cCfg_send.subAddrSize = mem_addr_size;
    i2cCfg_send.data = (uint8_t *)data;
    i2cCfg_send.dataSize = size;

    return I2C_MasterSend(I2C0_ID, &i2cCfg_send, timeout);
}

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    I2C_Transfer_Cfg i2cCfg_recv = {
        .slaveAddr = 0x50,
        .stopEveryByte = DISABLE,
        .subAddrSize = 2,
        .subAddr = 0x00,
        .dataSize = 0,
        .data = NULL,
    };

    if (i2c == NULL || data == NULL) {
        return -1;
    }

    i2cCfg_recv.slaveAddr = dev_addr;
    i2cCfg_recv.subAddr = mem_addr;
    i2cCfg_recv.subAddrSize = mem_addr_size;
    i2cCfg_recv.data = (uint8_t *)data;
    i2cCfg_recv.dataSize = size;

    return I2C_MasterReceive(I2C0_ID, &i2cCfg_recv, timeout);
}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    if (i2c == NULL || i2c->port != 0) {
        return -1;
    }
    I2C_Disable(i2c->port);	
	return 0;
}


