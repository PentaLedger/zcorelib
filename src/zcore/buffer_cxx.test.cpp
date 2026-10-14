/*
 * zcorelib - foundational C library with C++ wrappers
 * Copyright (C) 2026 Joe Turner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <zcore/buffer.hpp>
#include <zcore/error.hpp>

#include <string>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace {

std::string Contents(const zcore::Buffer &buf)
{
    const char *data = static_cast<const char *>(buf.data());
    return std::string(data, data + buf.size());
}

TEST(BufferCxx, IsMoveOnlyAndNothrow)
{
    static_assert(!std::is_copy_constructible_v<zcore::Buffer>, "must not be copyable");
    static_assert(!std::is_copy_assignable_v<zcore::Buffer>, "must not be copy-assignable");
    static_assert(std::is_nothrow_move_constructible_v<zcore::Buffer>, "moves must not throw");
    static_assert(std::is_nothrow_move_assignable_v<zcore::Buffer>, "moves must not throw");
    static_assert(std::is_nothrow_default_constructible_v<zcore::Buffer>, "default must not throw");
}

TEST(BufferCxx, DefaultConstructedOwnsNothing)
{
    const zcore::Buffer buf;
    EXPECT_FALSE(static_cast<bool>(buf));
    EXPECT_EQ(nullptr, buf.get());
    EXPECT_EQ(0u, buf.size());
    EXPECT_TRUE(buf.empty());
}

TEST(BufferCxx, CreateProducesOwningHandle)
{
    zcore::Buffer buf;
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&buf));
    EXPECT_TRUE(static_cast<bool>(buf));
    EXPECT_NE(nullptr, buf.get());
}

TEST(BufferCxx, CreateRejectsNullOutParam)
{
    EXPECT_EQ(ZC_ERR_INVALID_ARGUMENT, zcore::Buffer::create(nullptr));
    EXPECT_EQ(ZC_ERR_INVALID_ARGUMENT, zcore::Buffer::create(32, nullptr));
}

TEST(BufferCxx, CreateWithCapacityReserves)
{
    zcore::Buffer buf;
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(64, &buf));
    EXPECT_GE(buf.capacity(), 64u);
    EXPECT_TRUE(buf.empty());
}

TEST(BufferCxx, CreateReplacesAnExistingBuffer)
{
    zcore::Buffer buf;
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&buf));
    ASSERT_EQ(ZC_OK, buf.append("abc", 3));
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&buf)); // must not leak the first buffer
    EXPECT_EQ(0u, buf.size());
}

TEST(BufferCxx, AppendAndClearMirrorTheCApi)
{
    zcore::Buffer buf;
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&buf));
    ASSERT_EQ(ZC_OK, buf.append("hello", 5));
    EXPECT_EQ("hello", Contents(buf));
    buf.clear();
    EXPECT_TRUE(buf.empty());
}

TEST(BufferCxx, OperationsOnAnEmptyHandleReportInvalidArgument)
{
    zcore::Buffer buf;
    EXPECT_EQ(ZC_ERR_INVALID_ARGUMENT, buf.append("a", 1));
    EXPECT_EQ(ZC_ERR_INVALID_ARGUMENT, buf.reserve(8));
    buf.clear(); // must not crash
}

TEST(BufferCxx, MoveConstructionTransfersOwnership)
{
    zcore::Buffer src;
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&src));
    ASSERT_EQ(ZC_OK, src.append("abc", 3));
    const zc_buffer_t *raw = src.get();

    const zcore::Buffer dst(std::move(src));
    EXPECT_EQ(raw, dst.get());
    EXPECT_EQ("abc", Contents(dst));
    EXPECT_FALSE(static_cast<bool>(src));
    EXPECT_EQ(nullptr, src.get());
}

TEST(BufferCxx, MoveAssignmentReleasesTheOldBuffer)
{
    zcore::Buffer src;
    zcore::Buffer dst;
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&src));
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&dst));
    ASSERT_EQ(ZC_OK, src.append("src", 3));
    ASSERT_EQ(ZC_OK, dst.append("dst", 3));

    dst = std::move(src);
    EXPECT_EQ("src", Contents(dst));
    EXPECT_FALSE(static_cast<bool>(src));
}

TEST(BufferCxx, SelfMoveAssignmentIsSafe)
{
    zcore::Buffer buf;
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&buf));
    ASSERT_EQ(ZC_OK, buf.append("keep", 4));

    zcore::Buffer &alias = buf;
    buf = std::move(alias);

    EXPECT_TRUE(static_cast<bool>(buf));
    EXPECT_EQ("keep", Contents(buf));
}

TEST(BufferCxx, ReleaseHandsOffOwnership)
{
    zcore::Buffer buf;
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&buf));
    zc_buffer_t *raw = buf.release();
    ASSERT_NE(nullptr, raw);
    EXPECT_FALSE(static_cast<bool>(buf));
    zc_buffer_destroy(raw);
}

TEST(BufferCxx, ResetWithSamePointerDoesNotDoubleFree)
{
    zcore::Buffer buf;
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&buf));
    zc_buffer_t *raw = buf.get();
    buf.reset(raw);
    EXPECT_EQ(raw, buf.get());
    ASSERT_EQ(ZC_OK, buf.append("still alive", 11));
}

TEST(BufferCxx, ResetToNullDestroys)
{
    zcore::Buffer buf;
    ASSERT_EQ(ZC_OK, zcore::Buffer::create(&buf));
    buf.reset();
    EXPECT_FALSE(static_cast<bool>(buf));
}

TEST(BufferCxx, AdoptingConstructorTakesOwnership)
{
    zc_buffer_t *raw = nullptr;
    ASSERT_EQ(ZC_OK, zc_buffer_create(&raw));
    const zcore::Buffer buf(raw);
    EXPECT_EQ(raw, buf.get());
}

} // namespace
