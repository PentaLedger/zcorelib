# zcorelib

Foundational C library with C++ wrappers, shared across all my future
projects.

The core is C11 so it stays portable and ABI-stable; the C++17 layer adds RAII
and nothing else. Errors are return codes in both languages — the library
never throws.

## Status

Version 0.1.0. **While the major version is 0, the minor version carries
breaking changes** (both the `find_package` compatibility mode and the soname
encode this).

## Using it

Install it, then consume the package:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_INSTALL_PREFIX=/your/prefix \
  -DZCORE_BUILD_TESTS=OFF -DZCORE_BUILD_EXAMPLES=OFF -DZCORE_BUILD_DOCS=OFF
cmake --build build
cmake --install build
```

```cmake
find_package(zcore 0.1 REQUIRED)
target_link_libraries(myapp PRIVATE zcore::zcore)
```

Always link the namespaced `zcore::zcore` alias. It is the one form that keeps
working if the library is ever split into components.

```c
#include <zcore/zcore.h>    /* whole C API */
#include <zcore/buffer.h>   /* or just what you need */
```

```cpp
#include <zcore/zcore.hpp>  // whole C++ API (pulls in the C API too)
```

### What the two layers look like

```c
zc_buffer_t *buf = NULL;
if (zc_buffer_create_with_capacity(64, &buf) != ZC_OK) {
    return 1;
}
if (zc_buffer_append(buf, "hello", 5) != ZC_OK) {
    zc_buffer_destroy(buf);
    return 1;
}
zc_buffer_destroy(buf);
```

```cpp
zcore::Buffer buf;
if (!zcore::ok(zcore::Buffer::create(64, &buf))) { return 1; }
if (!zcore::ok(buf.append("hello", 5))) { return 1; }  // destructor cleans up
```

Runnable versions are in [`examples/`](examples/).

## Building for development

```sh
cmake --workflow --preset dev      # configure + build + test, warnings as errors
cmake --workflow --preset asan     # same, under ASan + UBSan
cmake --build --preset docs        # Doxygen HTML into build/docs/docs/html
./tools/format.sh                  # clang-format every tracked source file
./tools/verify-install.sh          # install + find_package(zcore) round trip
```

Doxygen writes to `<binary-dir>/docs/html`, so the path depends on the build
directory: `build/docs/docs/html` via the preset, `build/docs/html` for a
plain `cmake -B build`.

Requires CMake 3.23+ (for `FILE_SET HEADERS`), a C11 and C++17 compiler, and
optionally Doxygen 1.9+, Graphviz, and clang-format. GoogleTest is found if
installed and fetched if not.

### Build options

| Option | Default | Effect |
| --- | --- | --- |
| `ZCORE_BUILD_TESTS` | ON when top-level | Build and register the test suite |
| `ZCORE_BUILD_EXAMPLES` | ON when top-level | Build the sample programs |
| `ZCORE_BUILD_DOCS` | ON when top-level | Add the `docs` target |
| `ZCORE_INSTALL` | ON when top-level | Generate install and package rules |
| `ZCORE_WARNINGS_AS_ERRORS` | OFF | `-Werror` / `/WX` |
| `ZCORE_ENABLE_SANITIZERS` | OFF | ASan + UBSan |
| `ZCORE_DOCS_WARNINGS_AS_ERRORS` | ON | Doxygen warnings fail the docs build |
| `BUILD_SHARED_LIBS` | OFF | Build as a shared library |

## Layout

Pitchfork ([PFL](https://joholl.github.io/pitchfork-website/)), with separate
header placement and merged unit tests:

```
include/zcore/      installed public headers (C and C++)
src/zcore/          implementation, plus *.test.cpp next to the code it covers
src/zcore/internal/ private headers, not installed
tests/              test plumbing, integration tests, package-consumption test
examples/           buildable sample programs
docs/               Doxygen configuration and hand-written pages
cmake/              package config template, warning/sanitizer helpers
tools/              developer scripts
external/           reserved for submodule dependencies (currently unused)
build/              all generated output (gitignored)
```

## Conventions

Every public function returns `zc_error_t`, writes results through a trailing
out-parameter, leaves its operands untouched on failure, and pairs `_create`
with `_destroy`. `include/zcore/buffer.h` is the reference example.

The full set of rules, and the reasoning behind each, is in
[CONTRIBUTING.md](CONTRIBUTING.md).

## License

GPL-2.0-or-later — see [LICENSE](LICENSE). This is strong copyleft with no
linking exception: **every project that links zcorelib must itself be
GPL-2.0-compatible**, statically or dynamically, including internal projects,
and excluding anything that depends on Apache-2.0 code.
