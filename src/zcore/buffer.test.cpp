/*
 * zcorelib - foundational C library with C++ wrappers
 * Copyright (C) 2026 Joe Turner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <zcore/buffer.h>
#include <zcore/error.hpp>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <string>

#include <gtest/gtest.h>

namespace {

/** Deleter so tests can hold a C buffer in a unique_ptr. */
struct BufferDeleter {
    void operator()(zc_buffer_t *buf) const noexcept { zc_buffer_destroy(buf); }
};
using BufferPtr = std::unique_ptr<zc_buffer_t, BufferDeleter>;

BufferPtr MakeBuffer()
{
    zc_buffer_t *raw = nullptr;
    EXPECT_EQ(ZC_OK, zc_buffer_create(&raw));
    EXPECT_NE(nullptr, raw);
    return BufferPtr(raw);
}

std::string Contents(const zc_buffer_t *buf)
{
    const char *data = static_cast<const char *>(zc_buffer_data(buf));
    return std::string(data, data + zc_buffer_size(buf));
}

TEST(Buffer, CreateYieldsEmptyBuffer)
{
    const BufferPtr buf = MakeBuffer();
    EXPECT_EQ(0u, zc_buffer_size(buf.get()));
}

TEST(Buffer, CreateRejectsNullOutParam)
{
    EXPECT_EQ(ZC_ERR_INVALID_ARGUMENT, zc_buffer_create(nullptr));
    EXPECT_EQ(ZC_ERR_INVALID_ARGUMENT, zc_buffer_create_with_capacity(16, nullptr));
}

TEST(Buffer, CreateWithCapacityReservesEagerly)
{
    zc_buffer_t *raw = nullptr;
    ASSERT_EQ(ZC_OK, zc_buffer_create_with_capacity(128, &raw));
    const BufferPtr buf(raw);
    EXPECT_EQ(0u, zc_buffer_size(buf.get()));
    EXPECT_GE(zc_buffer_capacity(buf.get()), 128u);
}

TEST(Buffer, DestroyAcceptsNull)
{
    zc_buffer_destroy(nullptr); // must not crash
}

TEST(Buffer, AppendAccumulatesBytes)
{
    const BufferPtr buf = MakeBuffer();
    ASSERT_EQ(ZC_OK, zc_buffer_append(buf.get(), "abc", 3));
    ASSERT_EQ(ZC_OK, zc_buffer_append(buf.get(), "de", 2));
    EXPECT_EQ(5u, zc_buffer_size(buf.get()));
    EXPECT_EQ("abcde", Contents(buf.get()));
}

TEST(Buffer, AppendGrowsAcrossManyReallocations)
{
    const BufferPtr buf = MakeBuffer();
    std::string expected;
    for (int i = 0; i < 1000; ++i) {
        const std::string chunk = std::to_string(i) + ',';
        ASSERT_EQ(ZC_OK, zc_buffer_append(buf.get(), chunk.data(), chunk.size())) << "at " << i;
        expected += chunk;
    }
    EXPECT_EQ(expected.size(), zc_buffer_size(buf.get()));
    EXPECT_EQ(expected, Contents(buf.get()));
    EXPECT_GE(zc_buffer_capacity(buf.get()), zc_buffer_size(buf.get()));
}

TEST(Buffer, AppendZeroBytesIsANoOpAndAllowsNullData)
{
    const BufferPtr buf = MakeBuffer();
    ASSERT_EQ(ZC_OK, zc_buffer_append(buf.get(), "ab", 2));
    EXPECT_EQ(ZC_OK, zc_buffer_append(buf.get(), nullptr, 0));
    EXPECT_EQ(2u, zc_buffer_size(buf.get()));
}

TEST(Buffer, AppendRejectsNullDataWithNonZeroSize)
{
    const BufferPtr buf = MakeBuffer();
    EXPECT_EQ(ZC_ERR_INVALID_ARGUMENT, zc_buffer_append(buf.get(), nullptr, 4));
    EXPECT_EQ(0u, zc_buffer_size(buf.get()));
}

TEST(Buffer, AppendRejectsNullBuffer)
{
    EXPECT_EQ(ZC_ERR_INVALID_ARGUMENT, zc_buffer_append(nullptr, "a", 1));
}

TEST(Buffer, AppendDetectsSizeOverflow)
{
    const BufferPtr buf = MakeBuffer();
    ASSERT_EQ(ZC_OK, zc_buffer_append(buf.get(), "abc", 3));
    EXPECT_EQ(ZC_ERR_OVERFLOW,
              zc_buffer_append(buf.get(), "x", std::numeric_limits<std::size_t>::max()));
    EXPECT_EQ(3u, zc_buffer_size(buf.get())) << "buffer unchanged on failure";
}

TEST(Buffer, ReserveNeverShrinks)
{
    const BufferPtr buf = MakeBuffer();
    ASSERT_EQ(ZC_OK, zc_buffer_reserve(buf.get(), 256));
    const std::size_t grown = zc_buffer_capacity(buf.get());
    ASSERT_GE(grown, 256u);
    ASSERT_EQ(ZC_OK, zc_buffer_reserve(buf.get(), 1));
    EXPECT_EQ(grown, zc_buffer_capacity(buf.get()));
}

TEST(Buffer, ReserveRejectsNullBuffer)
{
    EXPECT_EQ(ZC_ERR_INVALID_ARGUMENT, zc_buffer_reserve(nullptr, 8));
}

TEST(Buffer, ClearKeepsCapacity)
{
    const BufferPtr buf = MakeBuffer();
    ASSERT_EQ(ZC_OK, zc_buffer_append(buf.get(), "payload", 7));
    const std::size_t capacity = zc_buffer_capacity(buf.get());
    zc_buffer_clear(buf.get());
    EXPECT_EQ(0u, zc_buffer_size(buf.get()));
    EXPECT_EQ(capacity, zc_buffer_capacity(buf.get()));
}

TEST(Buffer, ClearAcceptsNull)
{
    zc_buffer_clear(nullptr); // must not crash
}

TEST(Buffer, MutableDataIsWritable)
{
    const BufferPtr buf = MakeBuffer();
    ASSERT_EQ(ZC_OK, zc_buffer_append(buf.get(), "abc", 3));
    auto *bytes = static_cast<unsigned char *>(zc_buffer_data_mut(buf.get()));
    ASSERT_NE(nullptr, bytes);
    bytes[0] = 'X';
    EXPECT_EQ("Xbc", Contents(buf.get()));
}

TEST(Buffer, AccessorsTolerateNull)
{
    EXPECT_EQ(0u, zc_buffer_size(nullptr));
    EXPECT_EQ(0u, zc_buffer_capacity(nullptr));
    EXPECT_EQ(nullptr, zc_buffer_data(nullptr));
    EXPECT_EQ(nullptr, zc_buffer_data_mut(nullptr));
}

} // namespace
