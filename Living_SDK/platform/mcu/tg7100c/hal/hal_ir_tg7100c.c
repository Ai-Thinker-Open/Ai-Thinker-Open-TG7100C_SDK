#include <stdio.h>
#include <stdint.h>
#include "aos/hal/irdma.h"
#include <hal_ir.h>
#include <utils_log.h>

/*port support 0, 1, 4, 5, 8, 9, 12, 13, 16, 17, 20, 21
 * */
int hal_irdma_init(irdma_dev_t *irdma)
{
    int ret;

    if (irdma == NULL || (irdma->chip_type != 0 && irdma->chip_type != 1)) {
        return -1;
    }

    ret = hal_irled_init(irdma->chip_type, irdma->port);

    return 0;
}

int hal_irdma_send_data(irdma_dev_t *irdma, int data_num, uint32_t *buf)
{
    if (irdma == NULL || buf == NULL || data_num < 0 || data_num == 0) {
        return -1;
    }

    hal_irled_send_data(data_num, buf);

    return 0;
}

int32_t hal_irdma_finalize(irdma_dev_t *irdma)
{
    if (irdma == NULL) {
        return -1;
    }

    hal_irled_release();

    return 0;
}
