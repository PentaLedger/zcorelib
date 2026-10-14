#
# Buildable examples. These are compiled by default when zcorelib is the
# top-level project, so a sample that stops compiling breaks the build rather
# than rotting silently in the docs.
#

add_executable(zcore_example_buffer_c buffer_c.c)
target_link_libraries(zcore_example_buffer_c PRIVATE zcore::zcore)
zcore_set_warnings(zcore_example_buffer_c ${ZCORE_WARNINGS_AS_ERRORS})

add_executable(zcore_example_buffer_cxx buffer_cxx.cpp)
target_link_libraries(zcore_example_buffer_cxx PRIVATE zcore::zcore)
zcore_set_warnings(zcore_example_buffer_cxx ${ZCORE_WARNINGS_AS_ERRORS})

set_target_properties(zcore_example_buffer_c zcore_example_buffer_cxx
                      PROPERTIES FOLDER examples)
