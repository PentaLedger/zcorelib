/*
 * zcorelib - foundational C library with C++ wrappers
 * Copyright (C) 2026 Joe Turner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <zcore/error.h>

const char *zc_strerror(zc_error_t err)
{
    switch (err) {
    case ZC_OK:
        return "success";
    case ZC_ERR_INVALID_ARGUMENT:
        return "invalid argument";
    case ZC_ERR_OUT_OF_MEMORY:
        return "out of memory";
    case ZC_ERR_OUT_OF_RANGE:
        return "out of range";
    case ZC_ERR_OVERFLOW:
        return "arithmetic overflow";
    case ZC_ERR_NOT_SUPPORTED:
        return "not supported";
    case ZC_ERR_IO:
        return "I/O error";
    case ZC_ERR_UNKNOWN:
        return "unknown error";
    }
    return "unrecognised error code";
}

const char *zc_error_name(zc_error_t err)
{
    switch (err) {
    case ZC_OK:
        return "ZC_OK";
    case ZC_ERR_INVALID_ARGUMENT:
        return "ZC_ERR_INVALID_ARGUMENT";
    case ZC_ERR_OUT_OF_MEMORY:
        return "ZC_ERR_OUT_OF_MEMORY";
    case ZC_ERR_OUT_OF_RANGE:
        return "ZC_ERR_OUT_OF_RANGE";
    case ZC_ERR_OVERFLOW:
        return "ZC_ERR_OVERFLOW";
    case ZC_ERR_NOT_SUPPORTED:
        return "ZC_ERR_NOT_SUPPORTED";
    case ZC_ERR_IO:
        return "ZC_ERR_IO";
    case ZC_ERR_UNKNOWN:
        return "ZC_ERR_UNKNOWN";
    }
    return "ZC_ERR_UNRECOGNISED";
}
