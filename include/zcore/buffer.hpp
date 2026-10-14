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
 * @file buffer.hpp
 * @brief RAII wrapper around ::zc_buffer_t.
 *
 * The C++ layer adds lifetime management and nothing else. Per the library's
 * error policy it **never throws**: fallible operations return ::zc_error_t
 * exactly as the C API does, and construction is done through static
 * factories rather than throwing constructors.
 */

#ifndef ZCORE_BUFFER_HPP
#define ZCORE_BUFFER_HPP

#include <cstddef>
#include <utility>

#include <zcore/buffer.h>
#include <zcore/error.h>

namespace zcore {

/**
 * @brief Move-only owning handle for a ::zc_buffer_t.
 *
 * A default-constructed Buffer owns nothing and is falsy. Use create() to
 * allocate one.
 *
 * @par Thread safety
 * Inherits the C type's contract: distinct instances are independent, a single
 * instance must not be shared across threads without external synchronisation.
 */
class Buffer {
public:
    /** @brief Construct an empty handle that owns nothing. */
    Buffer() noexcept = default;

    /**
     * @brief Adopt ownership of an existing C buffer.
     * @param raw Buffer to take ownership of, or nullptr.
     */
    explicit Buffer(zc_buffer_t *raw) noexcept : buf_(raw) {}

    /** @brief Destroy the owned buffer, if any. */
    ~Buffer() { zc_buffer_destroy(buf_); }

    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;

    /**
     * @brief Take ownership from @p other, leaving it empty.
     * @param other Handle to move from.
     */
    Buffer(Buffer &&other) noexcept : buf_(std::exchange(other.buf_, nullptr)) {}

    /**
     * @brief Take ownership from @p other, destroying any buffer held first.
     * @param other Handle to move from. Self-assignment is a no-op.
     * @return Reference to this handle.
     */
    Buffer &operator=(Buffer &&other) noexcept
    {
        if (this != &other) {
            zc_buffer_destroy(buf_);
            buf_ = std::exchange(other.buf_, nullptr);
        }
        return *this;
    }

    /**
     * @brief Create an empty buffer into @p out.
     * @param[out] out Handle to populate. Any buffer it already holds is destroyed.
     * @return ::ZC_OK, ::ZC_ERR_INVALID_ARGUMENT if @p out is nullptr, or
     *         ::ZC_ERR_OUT_OF_MEMORY.
     */
    [[nodiscard]] static zc_error_t create(Buffer *out) noexcept
    {
        return create(0, out);
    }

    /**
     * @brief Create a buffer with reserved capacity into @p out.
     * @param capacity Bytes to reserve up front.
     * @param[out] out Handle to populate. Any buffer it already holds is destroyed.
     * @return ::ZC_OK, ::ZC_ERR_INVALID_ARGUMENT if @p out is nullptr, or
     *         ::ZC_ERR_OUT_OF_MEMORY.
     */
    [[nodiscard]] static zc_error_t create(std::size_t capacity, Buffer *out) noexcept
    {
        if (out == nullptr) {
            return ZC_ERR_INVALID_ARGUMENT;
        }
        zc_buffer_t *raw = nullptr;
        const zc_error_t err = zc_buffer_create_with_capacity(capacity, &raw);
        if (err != ZC_OK) {
            return err;
        }
        out->reset(raw);
        return ZC_OK;
    }

    /**
     * @brief Ensure at least @p capacity bytes fit without reallocating.
     * @param capacity Minimum capacity required.
     * @return ::ZC_OK, ::ZC_ERR_INVALID_ARGUMENT when this handle is empty,
     *         ::ZC_ERR_OVERFLOW, or ::ZC_ERR_OUT_OF_MEMORY.
     * @see zc_buffer_reserve
     */
    [[nodiscard]] zc_error_t reserve(std::size_t capacity) noexcept
    {
        return zc_buffer_reserve(buf_, capacity);
    }

    /**
     * @brief Append @p size bytes from @p data.
     * @param data Source bytes; may be nullptr only when @p size is 0.
     * @param size Number of bytes to copy.
     * @return ::ZC_OK, ::ZC_ERR_INVALID_ARGUMENT, ::ZC_ERR_OVERFLOW, or
     *         ::ZC_ERR_OUT_OF_MEMORY. The buffer is unchanged on failure.
     * @see zc_buffer_append
     */
    [[nodiscard]] zc_error_t append(const void *data, std::size_t size) noexcept
    {
        return zc_buffer_append(buf_, data, size);
    }

    /**
     * @brief Discard the contents, keeping the capacity.
     * @see zc_buffer_clear
     */
    void clear() noexcept { zc_buffer_clear(buf_); }

    /**
     * @brief Number of bytes currently stored.
     * @return Byte count, or 0 when this handle is empty.
     * @see zc_buffer_size
     */
    [[nodiscard]] std::size_t size() const noexcept { return zc_buffer_size(buf_); }

    /**
     * @brief Number of bytes that fit before the next reallocation.
     * @return Capacity in bytes, or 0 when this handle is empty.
     * @see zc_buffer_capacity
     */
    [[nodiscard]] std::size_t capacity() const noexcept { return zc_buffer_capacity(buf_); }

    /**
     * @brief Whether the buffer holds no bytes.
     * @return True when size() is 0, including for an empty handle.
     */
    [[nodiscard]] bool empty() const noexcept { return size() == 0; }

    /**
     * @brief Read-only pointer to the stored bytes.
     * @return Pointer to size() readable bytes, or nullptr. Invalidated by
     *         append(), reserve() and destruction.
     * @see zc_buffer_data
     */
    [[nodiscard]] const void *data() const noexcept { return zc_buffer_data(buf_); }

    /**
     * @brief Mutable pointer to the stored bytes.
     * @return Pointer to size() writable bytes, or nullptr. Same invalidation
     *         rules as the const overload.
     * @see zc_buffer_data_mut
     */
    [[nodiscard]] void *data() noexcept { return zc_buffer_data_mut(buf_); }

    /**
     * @brief Borrow the underlying C handle without transferring ownership.
     * @return The owned ::zc_buffer_t, or nullptr. This handle still owns it.
     */
    [[nodiscard]] zc_buffer_t *get() const noexcept { return buf_; }

    /**
     * @brief Relinquish ownership and return the raw handle.
     * @return The previously owned ::zc_buffer_t, or nullptr. The caller is
     *         now responsible for zc_buffer_destroy().
     */
    [[nodiscard]] zc_buffer_t *release() noexcept { return std::exchange(buf_, nullptr); }

    /**
     * @brief Destroy any owned buffer and optionally adopt @p raw.
     * @param raw Buffer to adopt, or nullptr to end up empty.
     */
    void reset(zc_buffer_t *raw = nullptr) noexcept
    {
        if (buf_ != raw) {
            zc_buffer_destroy(buf_);
            buf_ = raw;
        }
    }

    /**
     * @brief Whether this handle owns a buffer.
     * @return True when a buffer is owned.
     */
    explicit operator bool() const noexcept { return buf_ != nullptr; }

private:
    zc_buffer_t *buf_ = nullptr;
};

} // namespace zcore

#endif /* ZCORE_BUFFER_HPP */
