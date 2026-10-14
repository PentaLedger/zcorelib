#
# Standalone consumer project used to verify the installed package.
#
# This is NOT part of the zcorelib build. It is configured against an install
# tree by tools/verify-install.sh, which is the only way to prove that
# find_package(zcore) actually works -- the thing most libraries get wrong and
# never notice, because their own build always works in-tree.
#

cmake_minimum_required(VERSION 3.23)
project(zcore_package_test LANGUAGES C CXX)

find_package(zcore 0.1 REQUIRED)

add_executable(consume_c consume.c)
target_link_libraries(consume_c PRIVATE zcore::zcore)

add_executable(consume_cxx consume.cpp)
target_link_libraries(consume_cxx PRIVATE zcore::zcore)

enable_testing()
add_test(NAME consume_c COMMAND consume_c)
add_test(NAME consume_cxx COMMAND consume_cxx)
