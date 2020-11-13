#ifndef __UTILS_CRC_H__
#define __UTILS_CRC_H__
#include <stdint.h>

uint16_t utils_crc16(void *dataIn, uint32_t len);
uint32_t utils_crc32(void *dataIn, uint32_t len);

#endif
