#include <stdint.h>
#include <stddef.h>
#include <utils_hex.h>

const char hex_asc[] = "0123456789abcdef";
const char hex_asc_upper[] = "0123456789ABCDEF";
#define hex_asc_lo(x)   hex_asc_upper[((x) & 0x0f)]
#define hex_asc_hi(x)   hex_asc_upper[((x) & 0xf0) >> 4]

static inline char *hex_byte_pack(char *buf, uint8_t byte)
{
    *buf++ = hex_asc_hi(byte);
    *buf++ = hex_asc_lo(byte);
    return buf;
}

char *utils_bin2hex(char *dst, const void *src, size_t count)
{
    const unsigned char *_src = src;

    while (count--) {
        dst = hex_byte_pack(dst, *_src++);
    }

    return dst;
}
