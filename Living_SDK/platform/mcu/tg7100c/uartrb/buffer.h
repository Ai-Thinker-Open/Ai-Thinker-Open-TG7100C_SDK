
#ifndef __BUFFER_H__
#define __BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "stdint.h"

typedef struct {
    uint8_t   *buf;
    uint16_t  head_index;
    uint16_t  tail_index;
    uint16_t  cap;
} buf_data_s;

extern void pop_data_no_change(uint16_t max_len, buf_data_s *data_p, uint8_t *p, uint16_t len);
extern uint16_t how_many_valid_data(uint16_t max_len, buf_data_s *data_p);
extern void pop_data(uint16_t max_len, buf_data_s *data_p, uint8_t *p, uint16_t len);
extern void push_data(uint16_t max_len, buf_data_s *data_p, uint8_t *p, uint16_t len);
extern void init_data_buf(uint16_t max_len, buf_data_s *data_p);

#ifdef __cplusplus
}
#endif

#endif

