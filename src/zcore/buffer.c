/*
 * zcorelib - foundational C library with C++ wrappers
 * Copyright (C) 2026 Joe Turner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <zcore/buffer.h>

#include <stdint.h>
#include <string.h>

#include "zcore/internal/alloc.h"

/** @brief Capacity used by the first growth of an empty buffer. */
#define ZC_BUFFER_MIN_CAPACITY 16u

/** @brief Private representation of ::zc_buffer_t. */
struct zc_buffer {
    unsigned char *data; /**< Storage, or NULL when capacity is 0. */
    size_t size;         /**< Bytes in use. */
    size_t capacity;     /**< Bytes allocated. */
};

/**
 * @brief Grow @p buf geometrically until it can hold @p needed bytes.
 *
 * Doubling is capped so the computation cannot wrap; if doubling overflows we
 * fall back to the exact requirement rather than failing.
 */
static zc_error_t zc_buffer_grow(zc_buffer_t *buf, size_t needed)
{
    size_t capacity = buf->capacity;
    unsigned char *data = NULL;

    if (capacity >= needed) {
        return ZC_OK;
    }

    if (capacity < ZC_BUFFER_MIN_CAPACITY) {
        capacity = ZC_BUFFER_MIN_CAPACITY;
    }
    while (capacity < needed) {
        if (capacity > SIZE_MAX / 2u) {
            capacity = needed;
            break;
        }
        capacity *= 2u;
    }

    data = (unsigned char *)zc_internal_realloc(buf->data, capacity);
    if (data == NULL) {
        return ZC_ERR_OUT_OF_MEMORY;
    }

    buf->data = data;
    buf->capacity = capacity;
    return ZC_OK;
}

zc_error_t zc_buffer_create(zc_buffer_t **out)
{
    return zc_buffer_create_with_capacity(0, out);
}

zc_error_t zc_buffer_create_with_capacity(size_t capacity, zc_buffer_t **out)
{
    zc_buffer_t *buf = NULL;

    if (out == NULL) {
        return ZC_ERR_INVALID_ARGUMENT;
    }
    *out = NULL;

    buf = (zc_buffer_t *)zc_internal_malloc(sizeof(*buf));
    if (buf == NULL) {
        return ZC_ERR_OUT_OF_MEMORY;
    }
    buf->data = NULL;
    buf->size = 0;
    buf->capacity = 0;

    if (capacity > 0) {
        const zc_error_t err = zc_buffer_grow(buf, capacity);
        if (err != ZC_OK) {
            zc_internal_free(buf);
            return err;
        }
    }

    *out = buf;
    return ZC_OK;
}

void zc_buffer_destroy(zc_buffer_t *buf)
{
    if (buf == NULL) {
        return;
    }
    zc_internal_free(buf->data);
    zc_internal_free(buf);
}

zc_error_t zc_buffer_reserve(zc_buffer_t *buf, size_t capacity)
{
    if (buf == NULL) {
        return ZC_ERR_INVALID_ARGUMENT;
    }
    return zc_buffer_grow(buf, capacity);
}

zc_error_t zc_buffer_append(zc_buffer_t *buf, const void *data, size_t size)
{
    zc_error_t err = ZC_OK;

    if (buf == NULL) {
        return ZC_ERR_INVALID_ARGUMENT;
    }
    if (size == 0) {
        return ZC_OK;
    }
    if (data == NULL) {
        return ZC_ERR_INVALID_ARGUMENT;
    }
    if (size > SIZE_MAX - buf->size) {
        return ZC_ERR_OVERFLOW;
    }

    err = zc_buffer_grow(buf, buf->size + size);
    if (err != ZC_OK) {
        return err;
    }

    memcpy(buf->data + buf->size, data, size);
    buf->size += size;
    return ZC_OK;
}

void zc_buffer_clear(zc_buffer_t *buf)
{
    if (buf == NULL) {
        return;
    }
    buf->size = 0;
}

size_t zc_buffer_size(const zc_buffer_t *buf)
{
    return buf == NULL ? 0 : buf->size;
}

size_t zc_buffer_capacity(const zc_buffer_t *buf)
{
    return buf == NULL ? 0 : buf->capacity;
}

const void *zc_buffer_data(const zc_buffer_t *buf)
{
    return buf == NULL ? NULL : buf->data;
}

void *zc_buffer_data_mut(zc_buffer_t *buf)
{
    return buf == NULL ? NULL : buf->data;
}
