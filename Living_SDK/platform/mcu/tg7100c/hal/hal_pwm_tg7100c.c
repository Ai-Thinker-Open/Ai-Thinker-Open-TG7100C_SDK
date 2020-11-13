#include <stdio.h>
#include <stdint.h>
#include "hal/soc/soc.h"

const int port2pin[5] = {
    0,  // ch 0   pin 0
    1,  // ch 1   pin 1
    2,  // ch 2   pin 2
    3,  // ch 3   pin 3
    4   // ch 4   pin 4
};
/**
 * Initialises a PWM pin
 *
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_init(pwm_dev_t *pwm)
{
    int pin;
    uint8_t ch;

    if (NULL == pwm || pwm->port >= 5) {
        printf("arg error.\r\n");
        return -1;
    }
    ch = pwm->port;

    pin = port2pin[ch];
    bl_pwm_init(ch, pin, pwm->config.freq);
    bl_pwm_set_freq(ch, pwm->config.freq);
    bl_pwm_set_duty(ch, pwm->config.duty_cycle*100);
    return 0;
}

/**
 * Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_start(pwm_dev_t *pwm)
{
    uint8_t ch;

    if (NULL == pwm || pwm->port >= 5) {
        printf("arg error.\r\n");
        return -1;
    }

    ch = pwm->port;
    bl_pwm_start(ch);
    return 0;
}

/**
 * Stops output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_stop(pwm_dev_t *pwm)
{
    uint8_t ch;

    if (NULL == pwm || pwm->port >= 5) {
        printf("arg error.\r\n");
        return -1;
    }

    ch = pwm->port;
    bl_pwm_stop(ch);
    return 0;
}

/**
 * De-initialises an PWM interface, Turns off an PWM hardware interface
 *
 * @param[in]  pwm  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_finalize(pwm_dev_t *pwm)
{
    return hal_pwm_stop(pwm);
}


