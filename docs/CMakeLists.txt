#
# Doxygen documentation.
#
# Builds the `docs` target. Not part of ALL: documentation is generated on
# demand and in CI, not on every incremental build.
#

find_package(Doxygen 1.9)

if(NOT DOXYGEN_FOUND)
  message(STATUS "zcore: Doxygen not found, the 'docs' target is unavailable")
  return()
endif()

set(ZCORE_DOXYGEN_INPUT
    "${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/pages"
)
set(ZCORE_DOXYGEN_OUTPUT_DIR "${PROJECT_BINARY_DIR}/docs")
set(ZCORE_DOXYGEN_EXAMPLE_PATH "${PROJECT_SOURCE_DIR}/examples")
set(ZCORE_DOXYGEN_STRIP_PREFIX "${PROJECT_SOURCE_DIR}")

if(ZCORE_DOCS_WARNINGS_AS_ERRORS)
  set(ZCORE_DOXYGEN_WARN_AS_ERROR "FAIL_ON_WARNINGS")
else()
  set(ZCORE_DOXYGEN_WARN_AS_ERROR "NO")
endif()

configure_file(Doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

add_custom_target(
  docs
  COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  COMMENT "Generating API documentation with Doxygen"
  VERBATIM)

# Installing docs is opt-in: most consumers want the library, not the HTML.
if(ZCORE_INSTALL AND ZCORE_INSTALL_DOCS)
  install(
    DIRECTORY "${ZCORE_DOXYGEN_OUTPUT_DIR}/html/"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/html"
    OPTIONAL)
endif()
