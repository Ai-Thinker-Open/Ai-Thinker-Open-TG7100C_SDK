#ifndef __HAL_PWM_H__
#define __HAL_PWM_H__

#include <stdint.h>
#include <utils_list.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*hal_pwm_cb_t) (void *p_arg);

typedef struct {
    struct utils_list_hdr item;
    float duty;
    float adjust_duty;
    uint32_t remain_ms;
    hal_pwm_cb_t cb;
    void *p_arg;
} hal_pwm_ev_t;

typedef struct {
    struct utils_list list_head;
    uint8_t active;
    uint32_t freq;
    float now_duty;
    hal_pwm_ev_t *p_now_ev;
} hal_pwm_dev_t;

/**
 * Initialises a PWM pin
 *
 * @param[in]  id   the PWM chan id
 * @param[in]  pin  the PWM pin
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hal_pwm_init(uint8_t id, int pin);

/**
 * update PWM frequency
 *
 * @param[in]  id    the PWM chan id
 * @param[in]  freq  the PWM frequency (611~400KHZ)
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hal_pwm_freq_update(uint8_t id, uint32_t freq);

/**
 * get PWM frequency
 *
 * @param[in]  id      the PWM chan id
 * @param[out] p_freq  the pointer to memory frequency
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hal_pwm_freq_get(uint8_t id, uint32_t *p_freq);

/**
 * set PWM duty
 *
 * @param[in]  id          the PWM chan id
 * @param[in]  duty        the PWM duty (original duty * 100)
 * @param[in]  duration_ms time (ms) to adjust duty cycle
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hal_pwm_duty_set(uint8_t id, uint32_t duty, uint32_t duration_ms);

/**
 * get PWM duty
 *
 * @param[in]  id      the PWM chan id
 * @param[out] p_duty  the pointer to memory duty(original duty * 100)
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hal_pwm_duty_get(uint8_t id, uint32_t *p_duty);

/**
 * stop PWM
 *
 * @param[in]  id      the PWM chan id
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hal_pwm_stop(uint8_t id);

/**
 * start PWM
 *
 * @param[in]  id      the PWM chan id
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hal_pwm_start(uint8_t id);

int32_t vfs_pwm_init(uint32_t fdt, uint32_t dtb_uart_offset);

#ifdef __cplusplus
}
#endif

#endif
