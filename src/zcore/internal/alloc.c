/*
 * zcorelib - foundational C library with C++ wrappers
 * Copyright (C) 2026 Joe Turner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "zcore/internal/alloc.h"

#include <stdlib.h>

void *zc_internal_malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }
    return malloc(size);
}

void *zc_internal_realloc(void *ptr, size_t size)
{
    if (size == 0) {
        zc_internal_free(ptr);
        return NULL;
    }
    return realloc(ptr, size);
}

void zc_internal_free(void *ptr)
{
    free(ptr);
}
