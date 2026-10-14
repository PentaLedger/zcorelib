#
# Integration tests: scenarios that span several components or check
# behaviour a unit test cannot see (process-level behaviour, link-time
# properties, API usability from plain C).
#

# Verifies the public headers are actually valid C11 when compiled by a C
# compiler, which a C++-only test suite can never catch.
add_executable(zcore_c_api_integration c_api.c)
target_link_libraries(zcore_c_api_integration PRIVATE zcore::zcore)
zcore_set_warnings(zcore_c_api_integration ${ZCORE_WARNINGS_AS_ERRORS})
set_target_properties(zcore_c_api_integration PROPERTIES C_STANDARD 11
                                                         C_STANDARD_REQUIRED ON)

add_test(NAME zcore.integration.c_api COMMAND zcore_c_api_integration)
set_tests_properties(zcore.integration.c_api PROPERTIES LABELS integration)
