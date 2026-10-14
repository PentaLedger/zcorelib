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
#include <zcore/error.hpp>

#include <set>
#include <string_view>

#include <gtest/gtest.h>

namespace {

/** Every code the library defines, so tests can sweep the whole enum. */
constexpr zc_error_t kAllCodes[] = {
    ZC_OK,          ZC_ERR_INVALID_ARGUMENT, ZC_ERR_OUT_OF_MEMORY, ZC_ERR_OUT_OF_RANGE,
    ZC_ERR_OVERFLOW, ZC_ERR_NOT_SUPPORTED,   ZC_ERR_IO,            ZC_ERR_UNKNOWN,
};

TEST(Error, OkIsZeroSoTruthinessWorks)
{
    EXPECT_EQ(0, static_cast<int>(ZC_OK));
    for (const zc_error_t code : kAllCodes) {
        if (code != ZC_OK) {
            EXPECT_NE(0, static_cast<int>(code)) << "failure codes must be truthy: " << code;
        }
    }
}

TEST(Error, NumericValuesAreFrozen)
{
    // These values are ABI. Changing one is a breaking change; only append.
    EXPECT_EQ(0, static_cast<int>(ZC_OK));
    EXPECT_EQ(1, static_cast<int>(ZC_ERR_INVALID_ARGUMENT));
    EXPECT_EQ(2, static_cast<int>(ZC_ERR_OUT_OF_MEMORY));
    EXPECT_EQ(3, static_cast<int>(ZC_ERR_OUT_OF_RANGE));
    EXPECT_EQ(4, static_cast<int>(ZC_ERR_OVERFLOW));
    EXPECT_EQ(5, static_cast<int>(ZC_ERR_NOT_SUPPORTED));
    EXPECT_EQ(6, static_cast<int>(ZC_ERR_IO));
    EXPECT_EQ(7, static_cast<int>(ZC_ERR_UNKNOWN));
}

TEST(Error, StrerrorIsNeverNullAndNeverEmpty)
{
    for (const zc_error_t code : kAllCodes) {
        const char *msg = zc_strerror(code);
        ASSERT_NE(nullptr, msg) << code;
        EXPECT_FALSE(std::string_view(msg).empty()) << code;
    }
}

TEST(Error, StrerrorHandlesUnrecognisedValues)
{
    const char *msg = zc_strerror(static_cast<zc_error_t>(9999));
    ASSERT_NE(nullptr, msg);
    EXPECT_FALSE(std::string_view(msg).empty());
}

TEST(Error, NamesAreUniqueAndPrefixed)
{
    std::set<std::string_view> seen;
    for (const zc_error_t code : kAllCodes) {
        const char *name = zc_error_name(code);
        ASSERT_NE(nullptr, name) << code;
        const std::string_view view(name);
        EXPECT_EQ(0u, view.rfind("ZC_", 0)) << "all names carry the ZC_ prefix: " << view;
        EXPECT_TRUE(seen.insert(view).second) << "duplicate name: " << view;
    }
}

TEST(Error, NameHandlesUnrecognisedValues)
{
    EXPECT_STREQ("ZC_ERR_UNRECOGNISED", zc_error_name(static_cast<zc_error_t>(9999)));
}

TEST(Error, CxxHelpersAgreeWithCApi)
{
    EXPECT_TRUE(zcore::ok(ZC_OK));
    EXPECT_FALSE(zcore::ok(ZC_ERR_IO));
    EXPECT_EQ(std::string_view(zc_strerror(ZC_ERR_IO)), zcore::message(ZC_ERR_IO));
    EXPECT_EQ(std::string_view(zc_error_name(ZC_ERR_IO)), zcore::name(ZC_ERR_IO));
}

} // namespace
