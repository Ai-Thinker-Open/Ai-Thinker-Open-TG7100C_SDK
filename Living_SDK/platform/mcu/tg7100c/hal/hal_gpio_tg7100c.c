#include <stdio.h>
#include <stdint.h>
#include <bl_gpio.h>
#include <tg7100c_glb.h>
#include "hal/soc/gpio.h"

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    switch (gpio->config) {
    case INPUT_PULL_DOWN:
        bl_gpio_enable_input(gpio->port, 0, 1);
        break;
    case INPUT_PULL_UP:
        bl_gpio_enable_input(gpio->port, 1, 0);
        break;
    case INPUT_HIGH_IMPEDANCE:
        bl_gpio_enable_input(gpio->port, 0, 0);
        break;
    case OUTPUT_PUSH_PULL:
        bl_gpio_enable_output(gpio->port, 1, 0);
        break;
    case OUTPUT_OPEN_DRAIN_NO_PULL:
        bl_gpio_enable_output(gpio->port, 0, 0);
        break;
    case OUTPUT_OPEN_DRAIN_PULL_UP:
        bl_gpio_enable_output(gpio->port, 0, 0);
        break;
    case IRQ_MODE:
        break;
    default:
        return -1;
    }
    return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    bl_gpio_output_set(gpio->port, 1);
    return 0;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    bl_gpio_output_set(gpio->port, 0);
    return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    uint8_t val;
    
    if (gpio == NULL) {
        return -1;
    }

    val = bl_gpio_output_get_value(gpio->port);
    val = (val) ? 0 : 1;
    bl_gpio_output_set(gpio->port, val);
    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    uint8_t hal_val = 0;

    if (NULL == gpio || NULL == value) {
        return -1;
    }

    bl_gpio_input_get(gpio->port, &hal_val);
    *value = hal_val;

    return 0;
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
    gpio_ctx_t ctx;

    if (gpio == NULL) {
        return -1;
    }

    ctx.arg = arg;
    ctx.gpioPin = gpio->port;
    ctx.gpio_handler = handler;
    ctx.next = NULL;
    ctx.intCtrlMod = GLB_GPIO_INT_CONTROL_SYNC;
    if (trigger == IRQ_TRIGGER_FALLING_EDGE) {
        ctx.intTrgMod = GLB_GPIO_INT_TRIG_NEG_PULSE;
    } else if (trigger == IRQ_TRIGGER_RISING_EDGE) {
        ctx.intTrgMod = GLB_GPIO_INT_TRIG_POS_PULSE;
    } else {
        return -1;
    }

    bl_gpio_register(&ctx);

    return 0;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }
    bl_gpio_unregister(gpio->port);
    
    return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    if (gpio  == NULL) {
        return -1;
    }

    bl_gpio_int_clear(gpio->port, 1);
    bl_gpio_int_clear(gpio->port, 0);
    
    return 0;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }
    bl_gpio_unregister(gpio->port);

    return 0;    
}


