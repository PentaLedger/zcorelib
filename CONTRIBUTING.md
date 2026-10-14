# Contributing to zcorelib

This file records the decisions that are expensive to change later. They are
not style preferences — breaking one of them breaks every project that depends
on this library, so they are enforced in review.

## The design decisions, and why

| Decision | Choice | Why it is fixed |
| --- | --- | --- |
| Implementation language | C11 core, C++17 wrappers | The C core is the portable, ABI-stable artifact. The wrappers are a thin convenience layer. |
| Error handling | `zc_error_t` return codes everywhere | One model across the whole API. The C++ layer does **not** translate to exceptions or `expected`. |
| Exceptions | The library never throws | Keeps the library usable from C, from `-fno-exceptions` builds, and from freestanding targets. |
| Public struct layout | Opaque for anything owning resources | Lets the implementation change without an ABI break. |
| Allocation | All through `src/zcore/internal/alloc.h` | A caller-supplied allocator can be added later without touching a single public signature. A direct `malloc` in `src/` is a review defect. |
| Layout | Pitchfork (PFL), separate headers, merged tests | `include/zcore/` installs; `src/zcore/` holds sources and `*.test.cpp` next to them. |
| Library target | One monolithic `zcore` | Simple today. Consumers link `zcore::zcore` so a future split into `libs/` submodules stays source-compatible. |
| Test framework | GoogleTest | Unit tests are C++ even for the C API; `tests/integration/c_api.c` is what proves the headers are valid C. |
| Consumption | Installed package via `find_package(zcore)` | Verified by `tools/verify-install.sh` in CI, not assumed. |

## API conventions

Every public function follows these. Read `include/zcore/buffer.h` as the
reference implementation.

1. **Fallible functions return `zc_error_t`.** `ZC_OK` is 0, every failure is
   non-zero, so `if (err)` is a valid test.
2. **Results come back through a trailing out-parameter**, set to a safe empty
   value (usually `NULL`) on failure.
3. **Failures are atomic.** A failed call leaves its operands exactly as they
   were. Tests assert this explicitly.
4. **`_create` pairs with `_destroy`.** The receiver owns the object.
5. **`_destroy` and infallible mutators accept `NULL`.** Fallible functions
   reject a `NULL` subject with `ZC_ERR_INVALID_ARGUMENT`.
6. **Infallible accessors return their value directly** and tolerate `NULL`
   by returning a zero/empty result.
7. **Naming.** C: `zc_` on functions and types, `ZC_` on macros and
   enumerators (C has no namespaces, so the prefix is the namespace). C++:
   everything in `namespace zcore`.
8. **Thread safety is documented per type.** The default contract: distinct
   instances are independent, a single instance needs external
   synchronisation even through a `const` pointer.

### Error codes are ABI

The numeric values in `zc_error_t` are frozen. Append new codes at the end;
never renumber or reuse one. `src/zcore/error.test.cpp` pins the values so a
renumbering fails the test suite rather than silently breaking a consumer that
stored a code.

Adding a code also means updating both `zc_strerror` and `zc_error_name`.

### Adding a public header

1. Create `include/zcore/thing.h` (and `thing.hpp` if it needs a wrapper).
2. Annotate every exported function with `ZCORE_EXPORT`. Hidden visibility is
   the default, so an unannotated function will not be in the shared library.
3. Add the file to the `HEADERS` file set in the top-level `CMakeLists.txt` —
   that one list drives both the include path and the install rules.
4. Add it to the `zcore/zcore.h` or `zcore/zcore.hpp` umbrella.
5. Document everything. Doxygen runs with `WARN_IF_UNDOCUMENTED` and
   `WARN_AS_ERROR`, so undocumented public API fails CI.

### Adding a source file

Add it to the `PRIVATE` sources list in the top-level `CMakeLists.txt`.
Sources are listed explicitly, never globbed: a glob makes CMake miss new
files until something else forces a reconfigure.

### Adding tests

Unit tests go next to the code as `src/zcore/thing.test.cpp`, then get listed
in `tests/CMakeLists.txt`. Tests exercise the **public surface only** — if a
test needs a private header, the behaviour should be observable through the
public API instead.

`tests/integration/` is for what a unit test cannot see: C-compilability of
the headers, process-level behaviour, link-time properties.

## Versioning

Semantic versioning with one caveat: **while the major version is 0, the minor
version carries breaking changes.** This is encoded in two places, and both
change together at 1.0.0:

- `find_package` compatibility is `SameMinorVersion` (becomes
  `SameMajorVersion`).
- The soname is `0.MINOR` (becomes `MAJOR`).

The version lives only in the `project()` call in the top-level
`CMakeLists.txt`; `cmake/version_config.h.in` propagates it into the headers.

## Building

```sh
cmake --workflow --preset dev      # configure + build + test, warnings as errors
cmake --workflow --preset asan     # same, under ASan + UBSan
cmake --build --preset docs        # Doxygen into build/docs/docs/html
./tools/format.sh                  # clang-format every tracked source file
./tools/verify-install.sh          # install + find_package round trip
```

Doxygen writes to `<binary-dir>/docs/html`, so the preset lands it in
`build/docs/docs/html` and a plain `cmake -B build` in `build/docs/html`.

## What CI gates

A change must pass all of these:

- Builds on macOS/AppleClang, Linux/GCC, Linux/Clang, Windows/MSVC, plus a
  shared-library build, all with `-DZCORE_WARNINGS_AS_ERRORS=ON`.
- Clean under ASan + UBSan with `detect_leaks=1`.
- `tools/verify-install.sh` — the `find_package(zcore)` round trip.
- Doxygen with warnings as errors.
- `clang-format --dry-run --Werror`.

## License

zcorelib is GPL-2.0-or-later. Contributions are accepted under the same
license. Note that this means every project linking zcorelib must also be
GPL-2.0-compatible — including statically linked and internal projects, and
excluding anything that depends on Apache-2.0 code.
