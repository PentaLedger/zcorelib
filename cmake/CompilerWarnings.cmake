#
# zcorelib - compiler warning and sanitizer helpers.
#
# Kept in one place so the library, the tests and the examples all build under
# the same diagnostics. Warnings are PRIVATE: they are our standard, not
# something we impose on consumers of the installed package.
#

include_guard(GLOBAL)

#
# zcore_set_warnings(<target> <as_errors>)
#
# Apply the project's warning set to <target>. When <as_errors> is truthy,
# warnings fail the build.
#
function(zcore_set_warnings target as_errors)
  if(MSVC)
    set(warnings
        /W4 # highest practical level; /Wall is unusable on MSVC
        /permissive- # enforce standard conformance
        /w14242 # lossy implicit conversion
        /w14254 # larger bit-field assigned to smaller
        /w14263 # member function hides a virtual
        /w14265 # class has virtual functions but no virtual dtor
        /w14287 # unsigned/negative constant mismatch
        /w14296 # expression is always true/false
        /w14311 # pointer truncation
        /w14545 # malformed expression before comma
        /w14546 # function call before comma missing argument list
        /w14547 # operator before comma has no effect
        /w14549 # operator before comma has no effect
        /w14555 # expression has no effect
        /w14619 # unknown #pragma warning number
        /w14640 # non-thread-safe static initialisation
        /w14826 # sign-extending conversion
        /w14905 # wide string literal cast
        /w14906 # string literal cast
        /w14928 # illegal copy-initialisation
    )
    if(as_errors)
      list(APPEND warnings /WX)
    endif()
  else()
    set(warnings
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wcast-qual
        -Wcast-align
        -Wdouble-promotion
        -Wformat=2
        -Wundef
        -Wnull-dereference
        -Wunused
        -Wwrite-strings)
    # C-only: prototypes are not optional in this codebase.
    list(APPEND warnings $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
         $<$<COMPILE_LANGUAGE:C>:-Wmissing-prototypes>
         $<$<COMPILE_LANGUAGE:C>:-Wbad-function-cast>)
    # C++-only: catch non-virtual destructors and old-style casts.
    list(APPEND warnings $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
         $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
         $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      list(APPEND warnings -Wduplicated-cond -Wduplicated-branches -Wlogical-op
           $<$<COMPILE_LANGUAGE:CXX>:-Wuseless-cast>)
    endif()
    if(as_errors)
      list(APPEND warnings -Werror)
    endif()
  endif()

  target_compile_options(${target} PRIVATE ${warnings})
endfunction()

#
# zcore_set_sanitizers(<target> <enabled>)
#
# Add address + undefined-behaviour sanitizers to <target>. PUBLIC, because
# sanitizers must be applied consistently across everything linked together,
# including the test binaries.
#
function(zcore_set_sanitizers target enabled)
  if(NOT enabled)
    return()
  endif()

  if(MSVC)
    # MSVC ships ASan only; UBSan has no equivalent.
    target_compile_options(${target} PUBLIC /fsanitize=address)
    return()
  endif()

  set(flags -fsanitize=address,undefined -fno-omit-frame-pointer
            -fno-sanitize-recover=all)
  target_compile_options(${target} PUBLIC ${flags})
  target_link_options(${target} PUBLIC -fsanitize=address,undefined)
endfunction()
