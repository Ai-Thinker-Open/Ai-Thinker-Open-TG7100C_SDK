
#include "stdint.h"
#include "buffer.h"
#include "app_uart.h"

static uint8_t uart_recv_buf[RING_BUFF_INDEX_MAX][UART_RECV_BUF];
static buf_data_s uart_recv_data[RING_BUFF_INDEX_MAX];

static void uart_recv_buff_init(uint8_t i)
{
	uart_recv_data[i].buf = uart_recv_buf[i];
	init_data_buf(UART_RECV_BUF, &uart_recv_data[i]);
}

void uart_recv_buff_all_init()
{
    static uint8_t init = 0;
	int i;

	if (0 == init) {
        for(i = 0; i < RING_BUFF_INDEX_MAX; i++) {
            uart_recv_buff_init(i);
        }
		init = 1;
	}
}

uint16_t uart_how_many_valid_data_recv_buf(uint8_t index)
{
	uint16_t len;
	len = how_many_valid_data(UART_RECV_BUF, &uart_recv_data[index]);
    return len;
}

void uart_pop_from_recv_buff(uint8_t index, uint8_t *read_buf, uint16_t frame_len)
{
    pop_data(UART_RECV_BUF, &uart_recv_data[index], read_buf, frame_len);
}
void uart_pop_from_recv_buff_no_change(uint8_t index, uint8_t *read_buf, uint16_t frame_len)
{
    pop_data_no_change(UART_RECV_BUF, &uart_recv_data[index], read_buf, frame_len);
}

void uart_push_to_recv_buff(uint8_t index, uint8_t *write_buf, uint16_t frame_len)
{
    push_data(UART_RECV_BUF, &uart_recv_data[index], write_buf, frame_len);
}

