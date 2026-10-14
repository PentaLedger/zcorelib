/*
 * zcorelib example: the C API's house style.
 *
 * Note the shape of every fallible call: it returns zc_error_t, the result
 * comes back through a trailing out-parameter, and cleanup is unconditional
 * and NULL-safe. That is the pattern every zcorelib C function follows.
 */

#include <stdio.h>
#include <string.h>

#include <zcore/zcore.h>

int main(void)
{
    static const char *const lines[] = {"alpha\n", "beta\n", "gamma\n"};
    const size_t line_count = sizeof(lines) / sizeof(lines[0]);

    zc_buffer_t *buf = NULL;
    zc_error_t err = ZC_OK;
    size_t i = 0;

    /* Reserve up front when the final size is roughly known. */
    err = zc_buffer_create_with_capacity(64, &buf);
    if (err != ZC_OK) {
        fprintf(stderr, "could not create buffer: %s\n", zc_strerror(err));
        return 1;
    }

    for (i = 0; i < line_count; ++i) {
        err = zc_buffer_append(buf, lines[i], strlen(lines[i]));
        if (err != ZC_OK) {
            /* On failure the buffer is unchanged, so it is still safe to use. */
            fprintf(stderr, "append failed: %s\n", zc_strerror(err));
            zc_buffer_destroy(buf);
            return 1;
        }
    }

    printf("zcore %s\n", zc_version_string());
    printf("size=%zu capacity=%zu\n", zc_buffer_size(buf), zc_buffer_capacity(buf));
    printf("contents:\n%.*s", (int)zc_buffer_size(buf), (const char *)zc_buffer_data(buf));

    zc_buffer_destroy(buf);
    return 0;
}
