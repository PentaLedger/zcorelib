/*
 * Consumer smoke test: use the installed zcore package from C.
 *
 * Deliberately includes via <zcore/...>, exactly as a downstream project
 * would, to prove the installed include directory is wired up correctly.
 */

#include <stdio.h>

#include <zcore/zcore.h>

int main(void)
{
    zc_buffer_t *buf = NULL;

    if (zc_buffer_create(&buf) != ZC_OK) {
        fprintf(stderr, "zc_buffer_create failed\n");
        return 1;
    }
    if (zc_buffer_append(buf, "installed", 9) != ZC_OK) {
        fprintf(stderr, "zc_buffer_append failed\n");
        zc_buffer_destroy(buf);
        return 1;
    }
    if (zc_buffer_size(buf) != 9u) {
        fprintf(stderr, "unexpected size\n");
        zc_buffer_destroy(buf);
        return 1;
    }

    printf("consume_c: linked zcore %s\n", zc_version_string());
    zc_buffer_destroy(buf);
    return 0;
}
