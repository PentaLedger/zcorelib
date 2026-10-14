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
 * @file zcore.hpp
 * @brief Convenience umbrella header for the whole C++ API.
 *
 * Including this pulls in every public C++ header (and transitively the C
 * API). Prefer the individual per-feature headers in translation units you
 * care about recompiling.
 */

#ifndef ZCORE_ZCORE_HPP
#define ZCORE_ZCORE_HPP

#include <zcore/buffer.hpp>
#include <zcore/error.hpp>
#include <zcore/version.hpp>
#include <zcore/zcore.h>

#endif /* ZCORE_ZCORE_HPP */
