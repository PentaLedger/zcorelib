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
 * @file version.hpp
 * @brief C++ view of the library version.
 */

#ifndef ZCORE_VERSION_HPP
#define ZCORE_VERSION_HPP

#include <string_view>

#include <zcore/version.h>

namespace zcore {

/**
 * @brief Version of the linked library.
 * @return Version triple reported by the loaded library.
 * @see zc_version
 */
[[nodiscard]] inline zc_version_t version() noexcept { return zc_version(); }

/**
 * @brief Version of the linked library as a string.
 * @return View over a static @c "MAJOR.MINOR.PATCH" string.
 * @see zc_version_string
 */
[[nodiscard]] inline std::string_view version_string() noexcept { return zc_version_string(); }

/**
 * @brief Whether the linked library matches the headers this was compiled against.
 *
 * During the 0.x series the minor version carries breaking changes, so an
 * exact major/minor match is required.
 *
 * @return True when the loaded library is compatible with these headers.
 */
[[nodiscard]] inline bool version_is_compatible() noexcept
{
    const zc_version_t linked = zc_version();
    if (ZCORE_VERSION_MAJOR == 0) {
        return linked.major == ZCORE_VERSION_MAJOR && linked.minor == ZCORE_VERSION_MINOR;
    }
    return linked.major == ZCORE_VERSION_MAJOR;
}

} // namespace zcore

#endif /* ZCORE_VERSION_HPP */
