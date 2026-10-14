/*
 * zcorelib - foundational C library with C++ wrappers
 * Copyright (C) 2026 Joe Turner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/**
 * @file alloc.h
 * @brief Internal allocation indirection. Not installed, not public API.
 *
 * Every allocation inside the library goes through these functions rather
 * than calling malloc()/free() directly. Today they are thin wrappers over
 * libc. The indirection exists so a caller-supplied allocator can be added
 * later (a global hook, or a per-object allocator) **without changing a
 * single public signature** — the one decision in a C library that is
 * genuinely impossible to retrofit otherwise.
 *
 * If you add an allocation site, use these. A direct malloc() in src/ is a
 * review defect.
 */

#ifndef ZCORE_INTERNAL_ALLOC_H
#define ZCORE_INTERNAL_ALLOC_H

#include <stddef.h>

/**
 * @brief Allocate @p size bytes.
 * @param size Byte count. Allocating 0 bytes returns NULL.
 * @return Pointer to uninitialised storage, or NULL on failure.
 */
void *zc_internal_malloc(size_t size);

/**
 * @brief Resize the allocation at @p ptr to @p size bytes.
 * @param ptr Existing allocation, or NULL to allocate fresh.
 * @param size New byte count.
 * @return Pointer to the resized storage, or NULL on failure (@p ptr still valid).
 */
void *zc_internal_realloc(void *ptr, size_t size);

/**
 * @brief Release an allocation obtained from this module.
 * @param ptr Allocation to release, or NULL.
 */
void zc_internal_free(void *ptr);

#endif /* ZCORE_INTERNAL_ALLOC_H */
