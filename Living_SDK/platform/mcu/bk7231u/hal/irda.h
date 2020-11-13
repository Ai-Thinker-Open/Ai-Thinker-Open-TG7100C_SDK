#ifndef HAL_IRDA_H
#define HAL_IRDA_H

void hal_irda_set_cb(void *func);
void hal_irda_set_usrcode(uint16_t ir_usercode);
void hal_irda_init_app(void);

#endif /*HAL_IRDA_H*/
