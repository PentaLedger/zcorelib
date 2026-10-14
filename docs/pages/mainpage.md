zcorelib {#mainpage}
========

Foundational C library with C++ wrappers, shared across all Agave Mountain
projects.

Getting started
---------------

Link against the namespaced target and include the umbrella header for the
language you are using:

~~~{.cmake}
find_package(zcore 0.1 REQUIRED)
target_link_libraries(myapp PRIVATE zcore::zcore)
~~~

~~~{.c}
#include <zcore/zcore.h>    /* C API  */
~~~

~~~{.cpp}
#include <zcore/zcore.hpp>  // C++ API (pulls in the C API too)
~~~

Prefer the per-feature headers (`zcore/buffer.h`, `zcore/error.h`, …) in
translation units whose rebuild time you care about.

API conventions
---------------

These hold across the entire library. They are deliberately uniform so that
reading one header tells you how all of them behave.

| Topic | Rule |
| ----- | ---- |
| Errors | Every fallible function returns ::zc_error_t. ::ZC_OK is 0. The library never throws, not even from the C++ wrappers. |
| Results | Returned through a trailing out-parameter, which is set to a safe empty value on failure. |
| Failure atomicity | A failed call leaves its operands exactly as they were. |
| Ownership | Whoever receives an object from a `_create` function destroys it with the matching `_destroy`. |
| NULL | `_destroy` and infallible mutators accept NULL. Fallible functions reject a NULL subject with ::ZC_ERR_INVALID_ARGUMENT. |
| Opaque types | Public structs that own resources are opaque, so their layout is not ABI. |
| Allocation | All internal allocation goes through one indirection layer, so a caller-supplied allocator can be added without changing signatures. |
| Thread safety | Distinct instances are independent; a single instance needs external synchronisation, even through a `const` pointer. |
| Naming | C: `zc_` for functions and types, `ZC_` for macros and enumerators. C++: namespace `zcore`. |

Error handling in practice
--------------------------

The C++ layer adds lifetime management and nothing else, so both languages
read almost identically:

@include buffer_c.c

@include buffer_cxx.cpp

Versioning
----------

Semantic versioning, with one caveat: while the major version is 0, the
**minor** version carries breaking changes. `find_package` enforces this
(`SameMinorVersion` compatibility) and the soname is `0.MINOR`. At 1.0.0 both
switch to the usual major-version rules.

Check at run time that the loaded library matches your headers with
zcore::version_is_compatible().

Components
----------

- @ref error.h "zcore/error.h" — library-wide error codes.
- @ref version.h "zcore/version.h" — compile-time and run-time version.
- @ref buffer.h "zcore/buffer.h" — growable byte buffer, and the reference
  example of every convention above.
