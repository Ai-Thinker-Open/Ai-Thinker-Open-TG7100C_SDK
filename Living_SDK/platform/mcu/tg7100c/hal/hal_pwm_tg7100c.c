#include <stdio.h>
#include <stdint.h>
#include "hal/soc/soc.h"
#include <tg7100c_gpio.h>

#define PWM_CHANNAL_MAX     5

/*
 * mapping rules: pwm_ch = pin % 5
 * example: pin=14, pwm_ch = 14%5 = 4
*/
const int port2pin[5] = {
    5,   // ch 0   pin 5
    1,   // ch 1   pin 1
    12,  // ch 2   pin 12
    3,   // ch 3   pin 3
    14   // ch 4   pin 14
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
    uint8_t ch;
	GLB_GPIO_Cfg_Type cfg = {
        .drive = 0,
        .smtCtrl = 1,
        .gpioMode = GPIO_MODE_OUTPUT,
        .pullType = GPIO_PULL_DOWN,
        .gpioPin = 0,
        .gpioFun = 8,
    };
    
    if (NULL == pwm || pwm->port >= PWM_CHANNAL_MAX) {
        printf("arg error.\r\n");
        return -1;
    }
    
    ch = pwm->port;
    cfg.gpioPin = port2pin[ch];
    GLB_GPIO_Init(&cfg);

    PWM_Channel_Disable(ch);
    PWM_Smart_Configure(ch, pwm->config.freq, (uint8_t)(pwm->config.duty_cycle*100));

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

    if (NULL == pwm || pwm->port >= PWM_CHANNAL_MAX) {
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
 * @param[in]  pwm  the PWM device, para  set duty and  freq
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
    uint8_t ch;

    if (NULL == pwm || pwm->port >= PWM_CHANNAL_MAX) {
        printf("arg error.\r\n");
        return -1;
    }
    
    ch = pwm->port;
    PWM_Smart_Configure(ch, para.freq, (uint8_t)(para.duty_cycle*100));
    
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

    if (NULL == pwm || pwm->port >= PWM_CHANNAL_MAX) {
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
    uint8_t ch;

    if (NULL == pwm || pwm->port >= PWM_CHANNAL_MAX) {
        printf("arg error.\r\n");
        return -1;
    }

    ch = pwm->port;
    bl_pwm_stop(ch);
    return 0;
}


