/*
 * zcorelib - foundational C library with C++ wrappers
 * Copyright (C) 2026 Joe Turner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Integration test: drive the whole C API from a C11 translation unit.
 *
 * The unit tests are C++ (GoogleTest), so they cannot catch a public header
 * that is only valid as C++ -- a stray // comment in a C header, a bool
 * without <stdbool.h>, a C++ keyword used as an identifier. This file is the
 * guard for that, and it doubles as a smoke test of the realistic call
 * sequence a consumer writes.
 *
 * Returns 0 on success, non-zero on the first failure.
 */

#include <stdio.h>
#include <string.h>

#include <zcore/zcore.h>

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__,    \
                    #cond);                                                    \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static int test_version(void)
{
    const zc_version_t v = zc_version();

    CHECK(v.major == ZCORE_VERSION_MAJOR);
    CHECK(v.minor == ZCORE_VERSION_MINOR);
    CHECK(v.patch == ZCORE_VERSION_PATCH);
    CHECK(strcmp(zc_version_string(), ZCORE_VERSION_STRING) == 0);
    return 0;
}

static int test_error_strings(void)
{
    CHECK(ZC_OK == 0);
    CHECK(zc_strerror(ZC_ERR_IO) != NULL);
    CHECK(strcmp(zc_error_name(ZC_ERR_IO), "ZC_ERR_IO") == 0);
    return 0;
}

static int test_buffer_round_trip(void)
{
    static const char payload[] = "the quick brown fox";
    const size_t payload_len = sizeof(payload) - 1u;

    zc_buffer_t *buf = NULL;
    zc_error_t err = ZC_OK;

    err = zc_buffer_create_with_capacity(8, &buf);
    CHECK(err == ZC_OK);
    CHECK(buf != NULL);
    CHECK(zc_buffer_size(buf) == 0u);
    CHECK(zc_buffer_capacity(buf) >= 8u);

    err = zc_buffer_append(buf, payload, payload_len);
    CHECK(err == ZC_OK);
    CHECK(zc_buffer_size(buf) == payload_len);
    CHECK(memcmp(zc_buffer_data(buf), payload, payload_len) == 0);

    /* Appending again must grow past the initial capacity cleanly. */
    err = zc_buffer_append(buf, payload, payload_len);
    CHECK(err == ZC_OK);
    CHECK(zc_buffer_size(buf) == payload_len * 2u);

    zc_buffer_clear(buf);
    CHECK(zc_buffer_size(buf) == 0u);
    CHECK(zc_buffer_capacity(buf) >= payload_len * 2u);

    zc_buffer_destroy(buf);
    return 0;
}

int main(void)
{
    if (test_version() != 0) {
        return 1;
    }
    if (test_error_strings() != 0) {
        return 1;
    }
    if (test_buffer_round_trip() != 0) {
        return 1;
    }

    printf("zcore C API integration test passed (zcore %s)\n", zc_version_string());
    return 0;
}
