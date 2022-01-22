
#ifndef __APP_UART_H__
#define __APP_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "buffer.h"

#define RING_BUFF0_INDEX 	        0
#define RING_BUFF1_INDEX 	        1
#define RING_BUFF_INDEX_MAX         2
#define UART_RECV_BUF               256    /* all ringbuff is this */

extern void uart_recv_buff_all_init(void);
extern uint16_t uart_how_many_valid_data_recv_buf(uint8_t index);
extern void uart_push_to_recv_buff(uint8_t index, uint8_t *write_buf, uint16_t frame_len);
extern void uart_pop_from_recv_buff(uint8_t index, uint8_t *read_buf, uint16_t frame_len);
extern void uart_pop_from_recv_buff_no_change(uint8_t index, uint8_t *read_buf, uint16_t frame_len);

#ifdef __cplusplus
}
#endif

#endif

