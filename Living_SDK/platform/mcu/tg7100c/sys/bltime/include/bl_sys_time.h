#ifndef __BL_SYS_TIME_H__
#define __BL_SYS_TIME_H__
#include <stdint.h>
void bl_sys_time_update(uint64_t epoch);
int bl_sys_time_get(uint64_t *epoch);
int bl_sys_time_get(uint64_t *epoch);
int bl_sys_time_cli_init(void);

#endif
