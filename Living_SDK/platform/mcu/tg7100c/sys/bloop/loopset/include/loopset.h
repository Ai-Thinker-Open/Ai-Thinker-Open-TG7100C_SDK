#ifndef __LOOPSET_H__
#define __LOOPSET_H__

#define IR_PRIORITY  3

int loopset_led_hook_on_looprt(void);
void loopset_led_trigger(int pin, unsigned int timeon_ms);
int loopset_led_cli_init(void);

#endif
