#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__

#include <bl_adc.h>

int hal_bladc_init(int mode, int freq, int data_num, int gpio_num);
int hal_bladc_finalize(void);
int hal_adc_callback_register(bl_adc_cb_t cb, uint32_t *data, uint32_t size, int oneshot);
/* the following function for mode 0*/
int hal_adc_add_channel(int gpio_num);
int32_t hal_adc_get_data(int gpio_num, int raw_flag);
int32_t hal_prase_adc_data(uint32_t *ptr, int gpio_num, int raw_flag);
/* the following function for mode 1*/
int hal_parse_data_arr(uint32_t *ptr, uint32_t *output, uint32_t data_size);
#endif
