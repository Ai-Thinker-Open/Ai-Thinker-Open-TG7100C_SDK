/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <k_api.h>
#include <csi_config.h>
#include <soc.h>
#include <drv_timer.h>

uint64_t g_sys_tick_count;
extern uint32_t ticks_from_last_interrupt;
void systick_handler(void)
{
    g_sys_tick_count++;
#if (YUNOS_CONFIG_DYNTICKLESS == 0)
    krhino_tick_proc();
#else
    krhino_tickless_proc(ticks_from_last_interrupt);
#endif
}
