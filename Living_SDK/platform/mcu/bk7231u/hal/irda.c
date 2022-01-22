#include "aos/aos.h"
#include "hal/soc/soc.h"

#include "intc_pub.h"
#include "icu_pub.h"
#include "irda_pub.h"
#include "gpio_pub.h"
#include "drv_model_pub.h"
#include "irda.h"

void hal_irda_set_usrcode(uint16_t ir_usercode)
{
	IR_key.IR_UserCode = ir_usercode;
}

void hal_irda_set_cb(void *func)
{
	IR_key.irda_send_key = func;
}

void hal_irda_init_app(void)
{
	UINT32 param;
	
	IR_key.valid_flag = 0;
	
	param = PCLK_POSI_IRDA;
	sddev_control(ICU_DEV_NAME,CMD_CONF_PCLK_26M,&param);//irda clk:26M
	
	param = PWD_IRDA_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);//clk power up

	param = GFUNC_MODE_IRDA;
	sddev_control(GPIO_DEV_NAME,CMD_GPIO_ENABLE_SECOND,&param);//gpio config

	param = 0;
	sddev_control(IRDA_DEV_NAME, IRDA_CMD_SET_POLARITY, &param);//polarity set: low effective
	param = 1;
	sddev_control(IRDA_DEV_NAME, IRDA_CMD_ACTIVE, &param);//NEC IRDA enable
	param = 0x3921;
	sddev_control(IRDA_DEV_NAME, IRDA_CMD_SET_CLK, &param);//irda_clk*562.5 (26*562.5 = 0x3921)
	param = 0x7; /*IRDA_RIGHT_INT_MASK|IRDA_REPEAT_INT_MASK|IRDA_END_INT_MASK*/
	sddev_control(IRDA_DEV_NAME, IRDA_CMD_SET_INT_MASK, &param);
	
	/*interrupt setting about IRDA*/
	intc_enable(IRQ_IRDA);
	param = GINTR_IRQ_BIT;
	sddev_control(ICU_DEV_NAME, CMD_ICU_GLOBAL_INT_ENABLE, &param);
}

