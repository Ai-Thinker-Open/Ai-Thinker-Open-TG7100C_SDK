/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "cpp_mem.h"

void *operator new[](size_t size)
{
    return krhino_mm_alloc(size);
}

void *operator new(size_t size)
{
    return krhino_mm_alloc(size);
}

void operator delete[](void *ptr)
{
    krhino_mm_free(ptr);
}

void operator delete(void *ptr)
{
    krhino_mm_free(ptr);
}

