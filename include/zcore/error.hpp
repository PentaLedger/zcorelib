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
 * @file error.hpp
 * @brief C++ conveniences for ::zc_error_t.
 *
 * The C++ layer uses the same ::zc_error_t codes as the C API; it does not
 * translate them into exceptions or a result type. These helpers only make
 * codes pleasanter to test and print.
 */

#ifndef ZCORE_ERROR_HPP
#define ZCORE_ERROR_HPP

#include <ostream>
#include <string_view>

#include <zcore/error.h>

namespace zcore {

/** @brief Alias for the C error type, for use inside the @c zcore namespace. */
using Error = zc_error_t;

/**
 * @brief Whether @p err indicates success.
 * @param err Error code to test.
 * @return True when @p err is ::ZC_OK.
 */
[[nodiscard]] inline bool ok(Error err) noexcept { return err == ZC_OK; }

/**
 * @brief Human-readable description of @p err.
 * @param err Error code to describe.
 * @return View over a static string; outlives any caller.
 * @see zc_strerror
 */
[[nodiscard]] inline std::string_view message(Error err) noexcept { return zc_strerror(err); }

/**
 * @brief Identifier of @p err, e.g. @c "ZC_ERR_IO".
 * @param err Error code to name.
 * @return View over a static string; outlives any caller.
 * @see zc_error_name
 */
[[nodiscard]] inline std::string_view name(Error err) noexcept { return zc_error_name(err); }

} // namespace zcore

/**
 * @brief Stream @p err as @c NAME (description) so test failures are readable.
 *
 * Declared in the global namespace because ::zc_error_t is a C type and ADL
 * would not find an operator declared inside @c zcore.
 *
 * @param os Stream to write to.
 * @param err Error code to format.
 * @return Reference to @p os.
 */
inline std::ostream &operator<<(std::ostream &os, zc_error_t err)
{
    return os << zc_error_name(err) << " (" << zc_strerror(err) << ')';
}

#endif /* ZCORE_ERROR_HPP */
