#
# zcorelib - foundational C library with C++ wrappers
# Copyright (C) 2026 Joe Turner
#
# Licensed under the GNU General Public License v2.0 or later. See LICENSE.
#
# Layout follows the Pitchfork Layout (PFL):
#   include/zcore/   installed public headers (separate header placement)
#   src/zcore/       implementation + merged unit tests (*.test.cpp)
#   tests/           test plumbing and integration tests
#   examples/        buildable sample programs
#   docs/            Doxygen configuration
#   cmake/           package config template and helper modules
#   tools/           developer scripts
#   external/        reserved for submodule dependencies (currently unused)
#

# 3.23 is the floor for target_sources(FILE_SET HEADERS), which is what makes
# header installation and the build/install include-dir split automatic.
cmake_minimum_required(VERSION 3.23)

project(
  zcorelib
  VERSION 0.1.0
  DESCRIPTION "Foundational C library with C++ wrappers"
  HOMEPAGE_URL "https://github.com/agavemountain/zcorelib"
  LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(GenerateExportHeader)
include(CompilerWarnings)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------

option(ZCORE_BUILD_TESTS "Build the zcorelib test suite" ${PROJECT_IS_TOP_LEVEL})
option(ZCORE_BUILD_EXAMPLES "Build the zcorelib examples" ${PROJECT_IS_TOP_LEVEL})
option(ZCORE_BUILD_DOCS "Build the Doxygen documentation target" ${PROJECT_IS_TOP_LEVEL})
option(ZCORE_INSTALL "Generate zcorelib install and package rules" ${PROJECT_IS_TOP_LEVEL})
option(ZCORE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(ZCORE_ENABLE_SANITIZERS "Build with address and undefined-behaviour sanitizers" OFF)
option(ZCORE_DOCS_WARNINGS_AS_ERRORS "Fail the docs build on Doxygen warnings" ON)

if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clangd / clang-tidy
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE
        RelWithDebInfo
        CACHE STRING "Build type" FORCE)
  endif()
endif()

# ---------------------------------------------------------------------------
# The library target
# ---------------------------------------------------------------------------
#
# Deliberately a single monolithic target. If this ever splits into PFL
# submodules under libs/, consumers keep working only if they were already
# linking the namespaced alias zcore::zcore, so always document that form.
#
# Sources are listed explicitly rather than globbed: a glob makes CMake miss
# newly added files until something else forces a reconfigure.

add_library(zcore)
add_library(zcore::zcore ALIAS zcore)

target_sources(
  zcore
  PRIVATE src/zcore/buffer.c
          src/zcore/error.c
          src/zcore/internal/alloc.c
          src/zcore/version.c)

# Hand-written public headers. FILE_SET gives us the BUILD_INTERFACE /
# INSTALL_INTERFACE include-dir split and the install rules for free.
target_sources(
  zcore
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         "${PROJECT_SOURCE_DIR}/include"
         FILES
         include/zcore/buffer.h
         include/zcore/buffer.hpp
         include/zcore/error.h
         include/zcore/error.hpp
         include/zcore/version.h
         include/zcore/version.hpp
         include/zcore/zcore.h
         include/zcore/zcore.hpp)

# Generated public headers live in the build tree but install alongside the
# hand-written ones, so they need their own file set with a different BASE_DIR.
set(ZCORE_GENERATED_INCLUDE_DIR "${PROJECT_BINARY_DIR}/include")

configure_file(cmake/version_config.h.in
               "${ZCORE_GENERATED_INCLUDE_DIR}/zcore/version_config.h" @ONLY)

generate_export_header(
  zcore
  BASE_NAME ZCORE
  EXPORT_FILE_NAME "${ZCORE_GENERATED_INCLUDE_DIR}/zcore/export.h")

target_sources(
  zcore
  PUBLIC FILE_SET
         zcore_generated_headers
         TYPE
         HEADERS
         BASE_DIRS
         "${ZCORE_GENERATED_INCLUDE_DIR}"
         FILES
         "${ZCORE_GENERATED_INCLUDE_DIR}/zcore/export.h"
         "${ZCORE_GENERATED_INCLUDE_DIR}/zcore/version_config.h")

# Private headers under src/ are reachable as "zcore/internal/...".
target_include_directories(zcore PRIVATE "${PROJECT_SOURCE_DIR}/src")

target_compile_features(zcore PUBLIC c_std_11 cxx_std_17)

zcore_set_warnings(zcore ${ZCORE_WARNINGS_AS_ERRORS})
zcore_set_sanitizers(zcore ${ZCORE_ENABLE_SANITIZERS})

# Hidden visibility plus the generated export header means the exported symbol
# set is exactly what is annotated ZCORE_EXPORT, on every platform.
set_target_properties(
  zcore
  PROPERTIES OUTPUT_NAME zcore
             DEBUG_POSTFIX d
             C_VISIBILITY_PRESET hidden
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN ON
             POSITION_INDEPENDENT_CODE ON
             VERSION ${PROJECT_VERSION})

# During 0.x the minor version carries breaking changes, so it belongs in the
# soname. Switch to a bare major at 1.0.0.
if(PROJECT_VERSION_MAJOR EQUAL 0)
  set_target_properties(zcore PROPERTIES SOVERSION
                                         "0.${PROJECT_VERSION_MINOR}")
else()
  set_target_properties(zcore PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
endif()

# ---------------------------------------------------------------------------
# Subdirectories
# ---------------------------------------------------------------------------

if(ZCORE_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(ZCORE_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(ZCORE_BUILD_DOCS)
  add_subdirectory(docs)
endif()

# ---------------------------------------------------------------------------
# Install / package
# ---------------------------------------------------------------------------
#
# This section is what makes `find_package(zcore)` work for consumers. The
# three pieces that have to agree: install(TARGETS ... EXPORT),
# install(EXPORT ... NAMESPACE), and the generated zcoreConfig.cmake.

if(ZCORE_INSTALL)
  set(ZCORE_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/zcore")

  install(
    TARGETS zcore
    EXPORT zcoreTargets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILE_SET zcore_generated_headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

  install(
    EXPORT zcoreTargets
    FILE zcoreTargets.cmake
    NAMESPACE zcore::
    DESTINATION "${ZCORE_CMAKE_INSTALL_DIR}")

  configure_package_config_file(
    cmake/zcoreConfig.cmake.in "${PROJECT_BINARY_DIR}/zcoreConfig.cmake"
    INSTALL_DESTINATION "${ZCORE_CMAKE_INSTALL_DIR}")

  # SameMinorVersion while major is 0, because 0.x minors are breaking.
  if(PROJECT_VERSION_MAJOR EQUAL 0)
    set(ZCORE_VERSION_COMPATIBILITY SameMinorVersion)
  else()
    set(ZCORE_VERSION_COMPATIBILITY SameMajorVersion)
  endif()

  write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/zcoreConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ${ZCORE_VERSION_COMPATIBILITY})

  install(FILES "${PROJECT_BINARY_DIR}/zcoreConfig.cmake"
                "${PROJECT_BINARY_DIR}/zcoreConfigVersion.cmake"
          DESTINATION "${ZCORE_CMAKE_INSTALL_DIR}")

  install(FILES "${PROJECT_SOURCE_DIR}/LICENSE"
          DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/licenses/zcore")
endif()
