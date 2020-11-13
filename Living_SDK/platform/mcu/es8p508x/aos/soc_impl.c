/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>

#if (RHINO_CONFIG_MM_TLF > 0)

#if !defined (__CC_ARM) /* Keil / armcc */
extern void         *heap_start;
extern void         *heap_end;
extern void         *heap_len;

//extern void         *heap2_start;
//extern void         *heap2_len;
#endif


#if defined (__CC_ARM) /* Keil / armcc */
#define HEAP_BUFFER_SIZE 1024*5
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}, {(uint8_t *)0x10000000, 0x8000}};
#else
//k_mm_region_t g_mm_region[] = {{(uint8_t*)&heap_start,(size_t)&heap_len},{(uint8_t*)&heap2_start,(size_t)&heap2_len}};
k_mm_region_t g_mm_region[] = {{(uint8_t*)&heap_start,(size_t)&heap_len}};
#endif
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

#endif

size_t soc_get_cur_sp()
{
    size_t sp = 0;
#if defined (__GNUC__)&&!defined(__CC_ARM)
	asm volatile(
        "mov %0,sp\n"
        :"=r"(sp));
#endif
    return sp;
}


static void soc_print_stack()
{
    void    *cur, *end;
    int      i=0;
    int     *p;

    end   = krhino_cur_task_get()->task_stack_base + krhino_cur_task_get()->stack_size;
    cur = (void *)soc_get_cur_sp();
    p = (int*)cur;
    while(p < (int*)end) {
        if(i%4==0) {
            printf("\r\n%08x:",(uint32_t)p);
        }
        printf("%08x ", *p);
        i++;
        p++;
    }
    printf("\r\n");
    return;
}

void soc_err_proc(kstat_t err)
{
    (void)err;
    soc_print_stack();
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;
