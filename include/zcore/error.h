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
 * @file error.h
 * @brief Library-wide error codes.
 *
 * Every zcorelib function that can fail returns ::zc_error_t. ::ZC_OK is
 * guaranteed to be zero; every other code is non-zero, so `if (err)` is a
 * valid failure test.
 *
 * The numeric values are part of the ABI: codes are never renumbered or
 * reused, only appended. Codes live in their own space and deliberately do
 * not alias `errno` values.
 */

#ifndef ZCORE_ERROR_H
#define ZCORE_ERROR_H

#include <zcore/export.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Result of a zcorelib operation.
 */
typedef enum zc_error {
    ZC_OK = 0,                     /**< Operation succeeded. */
    ZC_ERR_INVALID_ARGUMENT = 1,   /**< A caller-supplied argument was invalid (e.g. NULL out-param). */
    ZC_ERR_OUT_OF_MEMORY = 2,      /**< An allocation failed. */
    ZC_ERR_OUT_OF_RANGE = 3,       /**< An index or offset was outside the valid range. */
    ZC_ERR_OVERFLOW = 4,           /**< A size or arithmetic computation overflowed. */
    ZC_ERR_NOT_SUPPORTED = 5,      /**< The operation is not supported on this platform or type. */
    ZC_ERR_IO = 6,                 /**< An underlying I/O operation failed. */
    ZC_ERR_UNKNOWN = 7             /**< Failure with no more specific code. */
} zc_error_t;

/**
 * @brief Return a human-readable description of @p err.
 *
 * The returned string is a static string literal: it is never NULL, must not
 * be freed, and is safe to use from multiple threads. Unrecognised values
 * yield a generic description rather than NULL.
 *
 * @param err Error code to describe.
 * @return Pointer to a NUL-terminated description with static storage duration.
 */
ZCORE_EXPORT const char *zc_strerror(zc_error_t err);

/**
 * @brief Return the identifier of @p err as a string, e.g. @c "ZC_ERR_IO".
 *
 * Intended for logs and test output. Same storage and thread-safety
 * guarantees as zc_strerror().
 *
 * @param err Error code to name.
 * @return Pointer to a NUL-terminated name with static storage duration.
 */
ZCORE_EXPORT const char *zc_error_name(zc_error_t err);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ZCORE_ERROR_H */
