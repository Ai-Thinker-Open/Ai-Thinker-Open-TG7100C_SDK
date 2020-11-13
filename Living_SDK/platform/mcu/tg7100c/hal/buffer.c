
#include "buffer.h"

volatile uint8_t g_buffer_block_flag = 0;

uint16_t how_many_valid_data(uint16_t max_len, buf_data_s *data_p)
{
	return (max_len - data_p->cap);
	//return (data_p->tail_index >= data_p->head_index)?((data_p->tail_index - data_p->head_index)):
		//          ((data_p->head_index - data_p->tail_index) - 1);
}

void pop_data(uint16_t max_len, buf_data_s *data_p, uint8_t *p, uint16_t len)
{
    if(max_len - data_p->cap < len) {
		g_buffer_block_flag = 1;
		return;
	}

    while(len--){
        *p++ = data_p->buf[data_p->head_index];
        data_p->head_index = (data_p->head_index + 1) % max_len;
        //data_p->cap++;
    }
	data_p->cap = (data_p->tail_index >= data_p->head_index)?(max_len - (data_p->tail_index - data_p->head_index)):
		          ((data_p->head_index - data_p->tail_index));
}

void pop_data_no_change(uint16_t max_len, buf_data_s *data_p, uint8_t *p, uint16_t len)
{

    uint16_t head_index = data_p->head_index;

    while(len--){
        *p++ = data_p->buf[head_index];
        head_index = (head_index + 1) % max_len;
    }
}

void push_data(uint16_t max_len, buf_data_s *data_p, uint8_t *p, uint16_t len)
{
    if(data_p->cap < len) {
		g_buffer_block_flag = 1; 
		return;
	}

    while(len--) {
        data_p->buf[data_p->tail_index] = *p++;
        data_p->tail_index = (data_p->tail_index + 1) % max_len;
        //data_p->cap--;
    }
	data_p->cap = (data_p->tail_index >= data_p->head_index)?(max_len - (data_p->tail_index - data_p->head_index)):
		          ((data_p->head_index - data_p->tail_index));
}

void init_data_buf(uint16_t max_len, buf_data_s *data_p)
{
    data_p->head_index = 0;
    data_p->tail_index = 0;
    data_p->cap = max_len;
}

