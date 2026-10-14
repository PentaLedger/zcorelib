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
 * @file version.h
 * @brief Compile-time and run-time version information.
 *
 * The @c ZCORE_VERSION_* macros come from the header the build generates
 * (`zcore/version_config.h`), so the single source of truth for the version
 * is the `project(... VERSION ...)` call in the top-level CMakeLists.txt.
 *
 * Compare the macros against zc_version() to detect a header/library
 * mismatch at run time.
 */

#ifndef ZCORE_VERSION_H
#define ZCORE_VERSION_H

#include <zcore/export.h>
#include <zcore/version_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A semantic version triple.
 *
 * This struct is intentionally transparent: it holds no resources, callers may
 * allocate it on the stack, and its layout is part of the ABI.
 */
typedef struct zc_version {
    int major; /**< Major version. */
    int minor; /**< Minor version. */
    int patch; /**< Patch version. */
} zc_version_t;

/**
 * @brief Return the version of the library that is actually linked in.
 * @return Version triple of the loaded library.
 */
ZCORE_EXPORT zc_version_t zc_version(void);

/**
 * @brief Return the linked library version as @c "MAJOR.MINOR.PATCH".
 * @return NUL-terminated string with static storage duration.
 */
ZCORE_EXPORT const char *zc_version_string(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ZCORE_VERSION_H */
