#include <FreeRTOS.h>
#include <task.h>

#include <bl_sys_time.h>

static uint64_t epoch_time = 0; //in ms
static uint64_t time_synced = 0; //in ms

void bl_sys_time_update(uint64_t epoch)
{
    time_synced = xTaskGetTickCount();
    epoch_time = epoch;
}

int bl_sys_time_get(uint64_t *epoch)
{
    int ms_diff;

    if (0 == epoch_time) {
        return -1;
    }

    //TODO we should use RTC hardware to track time
    ms_diff = ((int32_t)xTaskGetTickCount()) - ((int32_t)time_synced);
    *epoch = epoch_time + ms_diff;

    return 0;
}
