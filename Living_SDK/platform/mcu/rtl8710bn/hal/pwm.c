#include "hal/soc/soc.h"
#include "PeripheralNames.h"
#include "pwmout_api.h"

#define PWM_NUM 5
static pwmout_t PWM_OBJ[PWM_NUM];
static PinName PWM_MAP[PWM_NUM] = {
    [0] = PA_22,
    [1] = PA_0,
    [2] = PA_12,
    [3] = PA_14,
    [4] = PA_5,
};

int32_t hal_pwm_init(pwm_dev_t * pwm)
{
    static int init_clk = 0;

    if (pwm->port >= PWM_NUM) {
        return -1;
    }
    pwm->priv = &PWM_OBJ[pwm->port];
    pwmout_init(pwm->priv, PWM_MAP[pwm->port]);
#if 0
    if(!init_clk) {
        pwmout_clk_set(pwm->priv, 1, 0);
        init_clk = 1;
    }
#endif
    return 0;
}

int32_t hal_pwm_start(pwm_dev_t * pwm)
{
    int us = 1000000/pwm->config.freq;
    pwmout_period_us(pwm->priv, us);
    pwmout_write(pwm->priv, pwm->config.duty_cycle);

    return 0;
}

int32_t hal_pwm_stop(pwm_dev_t * pwm)
{
    pwmout_stop(pwm->priv);
    return 0;
}

int32_t hal_pwm_finalize(pwm_dev_t * pwm)
{
    pwmout_free(pwm->priv);
    return 0;
}
