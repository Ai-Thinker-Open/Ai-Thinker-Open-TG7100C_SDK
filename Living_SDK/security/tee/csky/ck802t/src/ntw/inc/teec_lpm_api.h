/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#ifndef _TEEC_LPM_API_H_
#define _TEEC_LPM_API_H_

#include "tee_types.h"

/**
 * switch to security to handle lpm.
 *
 * @arg[in] the arg.
 * return 0 means seccess.
 */
uint32_t TEEC_lpm_to_security(void *arg);

#endif /* _TEEC_LPM_API_H_ */
