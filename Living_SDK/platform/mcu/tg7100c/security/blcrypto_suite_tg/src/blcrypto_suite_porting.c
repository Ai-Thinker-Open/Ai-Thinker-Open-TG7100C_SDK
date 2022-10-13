#include <string.h>
#include <os_hal.h>

void *blcrypto_suite_calloc(size_t numitems, size_t size)
{
    size_t sizel = numitems * size;
    void *ptr = bl_os_malloc(sizel);
    if (ptr) {
        memset(ptr, 0, sizel);
    }
    return ptr;
}

void blcrypto_suite_free(void *ptr)
{
    bl_os_free(ptr);
}
