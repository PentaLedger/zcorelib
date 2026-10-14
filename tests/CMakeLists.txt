#
# zcorelib test suite.
#
# PFL puts unit tests next to the code they cover, so the *.test.cpp files
# live in src/zcore/ and are listed here. This directory owns the test
# plumbing and any integration test that does not fit the unit model.
#

# ---------------------------------------------------------------------------
# GoogleTest
# ---------------------------------------------------------------------------
#
# Prefer an installed GoogleTest (fast, no network); fall back to fetching a
# pinned release so a clean clone builds without manual setup.

set(ZCORE_GTEST_TAG
    "v1.17.0"
    CACHE STRING "GoogleTest git tag to fetch when none is installed")

find_package(GTest 1.14 QUIET)

if(NOT GTest_FOUND)
  message(STATUS "zcore: GoogleTest not found locally, fetching ${ZCORE_GTEST_TAG}")

  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG ${ZCORE_GTEST_TAG}
    GIT_SHALLOW TRUE
    EXCLUDE_FROM_ALL SYSTEM)

  # Keep GoogleTest out of our install tree and match the MSVC runtime.
  set(INSTALL_GTEST OFF)
  set(BUILD_GMOCK ON)
  set(gtest_force_shared_crt ON)

  FetchContent_MakeAvailable(googletest)
endif()

# Normalise the target names: an installed GTest gives GTest::gtest_main,
# FetchContent gives gtest_main.
if(TARGET GTest::gtest_main)
  set(ZCORE_GTEST_MAIN GTest::gtest_main)
else()
  set(ZCORE_GTEST_MAIN gtest_main)
endif()

include(GoogleTest)

# ---------------------------------------------------------------------------
# Unit tests (merged with the sources, per PFL)
# ---------------------------------------------------------------------------

add_executable(
  zcore_unit_tests
  "${PROJECT_SOURCE_DIR}/src/zcore/buffer.test.cpp"
  "${PROJECT_SOURCE_DIR}/src/zcore/buffer_cxx.test.cpp"
  "${PROJECT_SOURCE_DIR}/src/zcore/error.test.cpp"
  "${PROJECT_SOURCE_DIR}/src/zcore/version.test.cpp")

target_link_libraries(zcore_unit_tests PRIVATE zcore::zcore ${ZCORE_GTEST_MAIN})

# Tests exercise the installed public surface only. If a test ever needs a
# private header from src/, that is a signal the behaviour should be
# observable through the public API instead.
zcore_set_warnings(zcore_unit_tests ${ZCORE_WARNINGS_AS_ERRORS})

set_target_properties(zcore_unit_tests PROPERTIES CXX_STANDARD 17
                                                  CXX_STANDARD_REQUIRED ON)

# Register each TEST() individually so ctest -R can select at test granularity
# and a crash does not take down the whole suite's reporting.
gtest_discover_tests(
  zcore_unit_tests
  DISCOVERY_MODE PRE_TEST
  PROPERTIES LABELS unit)

# ---------------------------------------------------------------------------
# Integration tests
# ---------------------------------------------------------------------------

add_subdirectory(integration)

# ---------------------------------------------------------------------------
# Package consumption test
# ---------------------------------------------------------------------------
#
# tests/package/ is a standalone project that calls find_package(zcore). It
# cannot be configured from inside this build (the package does not exist
# until zcorelib is installed), so it is driven by tools/verify-install.sh and
# by CI rather than by ctest. See that script for the full round trip.
