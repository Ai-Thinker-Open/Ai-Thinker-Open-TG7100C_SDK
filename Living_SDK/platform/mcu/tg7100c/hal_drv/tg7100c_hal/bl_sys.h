#ifndef __BL_SYS_H__
#define __BL_SYS_H__

#include <stdint.h>

int bl_sys_logall_enable(void);
int bl_sys_logall_disable(void);
int bl_sys_reset_por(void);
void bl_sys_reset_system(void);
int bl_sys_isxipaddr(uint32_t addr);
int bl_sys_early_init(void);
int bl_sys_init(void);

#endif
