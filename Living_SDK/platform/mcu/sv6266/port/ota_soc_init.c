#include <nds32_intrinsic.h>
#include "soc_types.h"
#include "soc_defs.h"

#include "ota_hal_gpio.h"
#include "ota_hal_pinmux.h"
#include "custom_io_hal.h"
#include "custom_io_chk.h"
#include "attrs.h"

void _ota_soc_io_init(void) ATTRIBUTE_SECTION_OTA_FBOOT;
void _ota_soc_io_init(void)
{
    _ota_hal_pinmux_set_raw(_M_PIN_MODE, _M_FUNC_SEL);
}

