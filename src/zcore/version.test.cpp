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
#include <zcore/version.hpp>

#include <string>

#include <gtest/gtest.h>

namespace {

TEST(Version, LinkedLibraryMatchesHeaders)
{
    const zc_version_t v = zc_version();
    EXPECT_EQ(ZCORE_VERSION_MAJOR, v.major);
    EXPECT_EQ(ZCORE_VERSION_MINOR, v.minor);
    EXPECT_EQ(ZCORE_VERSION_PATCH, v.patch);
    EXPECT_TRUE(zcore::version_is_compatible());
}

TEST(Version, StringMatchesTriple)
{
    const zc_version_t v = zc_version();
    const std::string expected =
        std::to_string(v.major) + '.' + std::to_string(v.minor) + '.' + std::to_string(v.patch);
    EXPECT_EQ(expected, zc_version_string());
    EXPECT_EQ(expected, zcore::version_string());
}

TEST(Version, ComponentsAreNonNegative)
{
    const zc_version_t v = zc_version();
    EXPECT_GE(v.major, 0);
    EXPECT_GE(v.minor, 0);
    EXPECT_GE(v.patch, 0);
}

} // namespace
