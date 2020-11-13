#ifndef _IRDA_PUB_H_
#define _IRDA_PUB_H_

#define IRDA_FAILURE                (1)
#define IRDA_SUCCESS                (0)

#define IRDA_DEV_NAME                "irda"

#define IRDA_CMD_MAGIC              (0xe290000)
enum
{
    IRDA_CMD_ACTIVE = IRDA_CMD_MAGIC + 1,
    IRDA_CMD_SET_POLARITY,
    IRDA_CMD_SET_CLK,
    IRDA_CMD_SET_INT_MASK,
    TRNG_CMD_GET
};

enum
{
	IR_KEY_TYPE_SHORT = 0,
	IR_KEY_TYPE_LONG,
	IR_KEY_TYPE_HOLD,
	IR_KEY_TYPE_MAX,
};

struct IR_KEY_ST
{
	uint16_t IR_UserCode;
	uint8_t  valid_flag;
	uint8_t  IR_key_code;
	uint8_t  repeat_flag;
	uint8_t  repeat_cnt;
	uint8_t  timer_cnt;
    int (*irda_send_key)(uint32_t m);
};

extern struct IR_KEY_ST IR_key;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
void irda_init(void);
void irda_exit(void);
void irda_isr(void);
#endif //_IRDA_PUB_H_
