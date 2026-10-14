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
 * @file buffer.h
 * @brief Growable byte buffer.
 *
 * ::zc_buffer_t is the reference example of zcorelib's C conventions:
 *
 * - The type is **opaque**; its layout is private, so it can change without
 *   breaking ABI.
 * - Constructors return ::zc_error_t and write the new object through a
 *   trailing out-parameter.
 * - Destruction is infallible and accepts NULL.
 * - Accessors that cannot fail return their value directly.
 *
 * @par Thread safety
 * Distinct instances may be used concurrently without synchronisation. A
 * single instance must not be used concurrently from multiple threads, even
 * through const-qualified pointers.
 */

#ifndef ZCORE_BUFFER_H
#define ZCORE_BUFFER_H

#include <stddef.h>

#include <zcore/error.h>
#include <zcore/export.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque growable byte buffer.
 */
typedef struct zc_buffer zc_buffer_t;

/**
 * @brief Create an empty buffer.
 *
 * @param[out] out Receives the new buffer. Set to NULL on failure.
 * @return ::ZC_OK, ::ZC_ERR_INVALID_ARGUMENT if @p out is NULL, or
 *         ::ZC_ERR_OUT_OF_MEMORY.
 */
ZCORE_EXPORT zc_error_t zc_buffer_create(zc_buffer_t **out);

/**
 * @brief Create an empty buffer with room for @p capacity bytes.
 *
 * @param capacity Bytes to reserve up front. Zero behaves like zc_buffer_create().
 * @param[out] out Receives the new buffer. Set to NULL on failure.
 * @return ::ZC_OK, ::ZC_ERR_INVALID_ARGUMENT if @p out is NULL, or
 *         ::ZC_ERR_OUT_OF_MEMORY.
 */
ZCORE_EXPORT zc_error_t zc_buffer_create_with_capacity(size_t capacity, zc_buffer_t **out);

/**
 * @brief Destroy @p buf and release its storage.
 *
 * Destroying NULL is a no-op.
 *
 * @param buf Buffer to destroy, or NULL.
 */
ZCORE_EXPORT void zc_buffer_destroy(zc_buffer_t *buf);

/**
 * @brief Ensure @p buf can hold at least @p capacity bytes without reallocating.
 *
 * Never shrinks the buffer.
 *
 * @param buf Buffer to grow.
 * @param capacity Minimum capacity required.
 * @return ::ZC_OK, ::ZC_ERR_INVALID_ARGUMENT if @p buf is NULL,
 *         ::ZC_ERR_OVERFLOW if @p capacity is not representable, or
 *         ::ZC_ERR_OUT_OF_MEMORY.
 */
ZCORE_EXPORT zc_error_t zc_buffer_reserve(zc_buffer_t *buf, size_t capacity);

/**
 * @brief Append @p size bytes from @p data to @p buf.
 *
 * Appending zero bytes succeeds and leaves @p buf unchanged; @p data may be
 * NULL in that case only. On failure @p buf is left exactly as it was.
 *
 * @param buf Buffer to append to.
 * @param data Source bytes.
 * @param size Number of bytes to copy from @p data.
 * @return ::ZC_OK, ::ZC_ERR_INVALID_ARGUMENT, ::ZC_ERR_OVERFLOW, or
 *         ::ZC_ERR_OUT_OF_MEMORY.
 */
ZCORE_EXPORT zc_error_t zc_buffer_append(zc_buffer_t *buf, const void *data, size_t size);

/**
 * @brief Discard the contents of @p buf, keeping its capacity.
 *
 * Clearing NULL is a no-op.
 *
 * @param buf Buffer to clear, or NULL.
 */
ZCORE_EXPORT void zc_buffer_clear(zc_buffer_t *buf);

/**
 * @brief Number of bytes currently stored in @p buf.
 * @param buf Buffer to query, or NULL.
 * @return Byte count, or 0 if @p buf is NULL.
 */
ZCORE_EXPORT size_t zc_buffer_size(const zc_buffer_t *buf);

/**
 * @brief Number of bytes @p buf can hold before it must reallocate.
 * @param buf Buffer to query, or NULL.
 * @return Capacity in bytes, or 0 if @p buf is NULL.
 */
ZCORE_EXPORT size_t zc_buffer_capacity(const zc_buffer_t *buf);

/**
 * @brief Read-only pointer to the bytes in @p buf.
 *
 * The pointer is invalidated by any call that may reallocate (
 * zc_buffer_append(), zc_buffer_reserve()) and by zc_buffer_destroy().
 * May be NULL when the buffer has never allocated.
 *
 * @param buf Buffer to query, or NULL.
 * @return Pointer to zc_buffer_size() readable bytes, or NULL.
 */
ZCORE_EXPORT const void *zc_buffer_data(const zc_buffer_t *buf);

/**
 * @brief Mutable pointer to the bytes in @p buf.
 *
 * Same invalidation rules as zc_buffer_data().
 *
 * @param buf Buffer to query, or NULL.
 * @return Pointer to zc_buffer_size() writable bytes, or NULL.
 */
ZCORE_EXPORT void *zc_buffer_data_mut(zc_buffer_t *buf);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ZCORE_BUFFER_H */
