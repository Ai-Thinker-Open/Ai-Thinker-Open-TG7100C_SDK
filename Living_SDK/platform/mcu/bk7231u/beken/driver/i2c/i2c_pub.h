#ifndef __I2C1_PUB_H__
#define __I2C1_PUB_H__

#define I2C1_FAILURE                (1)
#define I2C1_SUCCESS                (0)
#define I2C2_FAILURE                (1)
#define I2C2_SUCCESS                (0)

#define I2C1_DEV_NAME               "i2c1"
#define I2C2_DEV_NAME               "i2c2"

#define I2C1_CMD_MAGIC              (0x0AB00000)
#define I2C2_CMD_MAGIC              (0x0ABF0000)

#include "uart_pub.h"
#define I2C_DEBUG
#undef 	I2C_DEBUG

#ifdef I2C_DEBUG
#define I2C_PRT                    os_printf
#else
#define I2C_PRT                    null_prf
#define I2C_WPRT                   null_prf
#endif

#define ADDR_WIDTH_8               1
#define ADDR_WIDTH_16              2

enum
{
    I2C1_CMD_SET_ENSMB = I2C1_CMD_MAGIC + 1,
    I2C1_CMD_SET_SMBUS_STA,
    I2C1_CMD_SET_SMBUS_STOP,
    I2C1_CMD_SET_SMBUS_ACK_TX,
    I2C1_CMD_SET_SMBUS_TX_MODE,
    I2C1_CMD_SET_FREQ_DIV,
    I2C1_CMD_GET_SMBUS_INTERRUPT,
    I2C1_CMD_CLEAR_SMBUS_INTERRUPT,
    I2C1_CMD_GET_ACK_RX,
    I2C1_CMD_GET_ACK_REQ,
    I2C1_CMD_GET_SMBUS_BUSY,  
};

enum
{
    I2C2_CMD_SET_IDLE_CR = I2C2_CMD_MAGIC + 1,
	I2C2_CMD_SET_SCL_CR,
	I2C2_CMD_SET_FREQ_DIV,
	I2C2_CMD_SET_SMBUS_CS,
	I2C2_CMD_SET_TIMEOUT_EN,
	I2C2_CMD_SET_FREE_DETECT,
	I2C2_CMD_SET_SLAVE_EN,
	I2C2_CMD_SET_SMBUS_EN,
	I2C2_CMD_SET_POWER_UP,
	I2C2_CMD_SET_POWER_DOWN,
	I2C2_CMD_GET_MESSAGE,
	I2C2_CMD_GET_BUSY,
};

typedef struct i2c_op_st {
	UINT8 addr_width;
    UINT8 salve_id;
    UINT16 op_addr;
} I2C_OP_ST, *I2C_OP_PTR;

typedef struct
{	
	UINT8 TxMode;		//0: Read;  1: Write
	UINT8 WkMode;       // work mode
                        //RW(bit 0):  0:write,  1:read
                       // MS(bit 1):  0:master, 1:slave
                       // AL(bit 2):  0:7bit address, 1:10bit address
                       // IA(bit 3):  0:without inner address, 1: with inner address
                       // reserved(bit [4:7]):  reserved
	
	UINT8  InnerAddr;
	UINT8  SendAddr;   //only master send address
	UINT32 CurrentNum;
	UINT32 AllDataNum;
	UINT8  *pData;
	UINT8  SalveID;	  //slave address
	UINT8  AddrFlag;
	UINT8  TransDone;
	UINT8  ack_check;	//0: don't care ACK; 1: care ACK
	UINT8  ErrorNO;
}i2c_op_message;


#define NUM_ROUND_UP(a,b)   ((a) / (b) + (((a) % (b)) ? 1 : 0))

#define I2C1_DEFAULT_CLK     26000000
#define I2C_BAUD_1KHZ        1000
#define I2C_BAUD_100KHZ      100000
#define I2C_BAUD_400KHZ      400000
#define I2C_BAUD_4MHZ        4000000
#define I2C_DEFAULT_BAUD     I2C_BAUD_400KHZ
#define I2C_CLK_DIVID(rate)  (NUM_ROUND_UP(NUM_ROUND_UP(I2C1_DEFAULT_CLK, rate) - 6, 3) - 1)
#define I2C_DEF_DIV          0x16

#define I2C2_MSG_WORK_MODE_RW_BIT        (1<<0)      /* 0:write,  1:read */
#define I2C2_MSG_WORK_MODE_MS_BIT        (1<<1)      /* 0:master, 1:slave */
#define I2C2_MSG_WORK_MODE_AL_BIT        (1<<2)      /* 0:7bit address, 1:10bit address */
#define I2C2_MSG_WORK_MODE_IA_BIT        (1<<3)      /* 0:without inner address, 1: with inner address */


void i2c1_init(void);
void i2c1_exit(void);
void i2c2_init(void);
void i2c2_exit(void);

#endif // __I2C1_PUB_H__

