/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bzopt.h"
#include "utils.h"

uint8_t hex2ascii(uint8_t digit)
{
    uint8_t val;

    if (digit <= 9) {
        val = digit - 0x0 + '0';
    } else {
        val = digit - 0xA + 'a';
    }

    return val;
}

void hex2string(uint8_t *hex, uint32_t len, uint8_t *str)
{
    uint32_t index;

    for (index = 0; index < len; index++) {
        str[index * 2] = hex2ascii(hex[index] >> 4 & 0x0f);
        str[index * 2 + 1] = hex2ascii(hex[index] & 0x0f);
    }
}

static void utf8_to_str(uint8_t *data, uint8_t len, char *result)
{
    memcpy(result, data, len);
}

void utf8_to_ssid(uint8_t *data, uint8_t len, char *ssid)
{
    utf8_to_str(data, len, ssid);
}

void utf8_to_pw(uint8_t *data, uint8_t len, char *pw)
{
    utf8_to_str(data, len, pw);
}

void get_random(uint8_t *random, uint8_t random_len)
{
    uint8_t bytes_available = 0;
    uint32_t seed = os_now_ms();
    uint8_t byte[5];
    uint32_t result;
    uint16_t bytes_copy;

    srand((unsigned int)seed);
    result = rand();

    while (bytes_available < random_len) {
        seed += result;
        seed = seed % 9999;
        snprintf((char *)byte, sizeof(byte), "%04u", seed);
        bytes_copy = random_len - bytes_available;
        bytes_copy = (bytes_copy > 4) ? 4 : bytes_copy;
        memcpy(random + bytes_available, byte, bytes_copy);
        bytes_available += bytes_copy;
    }
}

static void hex_byte_dump(uint8_t *data, int len, int tab_num)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02x ", data[i]);

        if (!((i + 1) % tab_num)) {
            printf("\r\n");
        }
    }

    printf("\r\n");
}

void hex_byte_dump_debug(uint8_t *data, int len, int tab_num)
{
#if (CONFIG_BLDTIME_MUTE_DBGLOG)

#else
    hex_byte_dump(data, len, tab_num);
#endif
}

void hex_byte_dump_verbose(uint8_t *data, int len, int tab_num)
{
#if defined(BZ_VERBOSE_DEBUG)
    hex_byte_dump(data, len, tab_num);
#else
    
#endif
}
