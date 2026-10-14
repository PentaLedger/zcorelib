/*
 * zcorelib - foundational C library with C++ wrappers
 * Copyright (C) 2026 Joe Turner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <zcore/version.h>

zc_version_t zc_version(void)
{
    const zc_version_t v = {ZCORE_VERSION_MAJOR, ZCORE_VERSION_MINOR, ZCORE_VERSION_PATCH};
    return v;
}

const char *zc_version_string(void)
{
    return ZCORE_VERSION_STRING;
}
