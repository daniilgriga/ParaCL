cmake_minimum_required (VERSION 3.15)

project (ParaCL LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

option (SANITIZE "Enable Address and Undefined sanitizers" OFF)
option (BUILD_TESTS "Build tests" ON)

if(MSVC)
    set (COMMON_COMPILE_OPTIONS /W4 /WX)
else()
    set (COMMON_COMPILE_OPTIONS
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
endif()

if(SANITIZE AND NOT MSVC)
    set (SANITIZE_FLAGS -fsanitize=address,undefined)
endif()

add_executable (paracl src/main.cpp)
target_include_directories (paracl PRIVATE include)

target_compile_options (paracl PRIVATE
    ${COMMON_COMPILE_OPTIONS}
    ${SANITIZE_FLAGS}
)

target_link_options (paracl PRIVATE
    ${SANITIZE_FLAGS}
)

find_package (BISON REQUIRED)
find_package (FLEX REQUIRED)

BISON_TARGET (ParaCLParser
    src/syntax/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hh
)

FLEX_TARGET (ParaCLLexer
    src/syntax/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cc
)

ADD_FLEX_BISON_DEPENDENCY (ParaCLLexer ParaCLParser)

set_source_files_properties (
    ${BISON_ParaCLParser_OUTPUTS}
    ${FLEX_ParaCLLexer_OUTPUTS}
    PROPERTIES
    COMPILE_OPTIONS "-Wno-error=unused-but-set-variable;-Wno-error=sign-compare"
)

add_executable (paracl_syntax
    src/syntax/driver.cpp
    src/syntax/driver.hpp
    ${BISON_ParaCLParser_OUTPUTS}
    ${FLEX_ParaCLLexer_OUTPUTS}
)

target_include_directories (paracl_syntax PRIVATE
    src/syntax
    ${CMAKE_CURRENT_BINARY_DIR}
    ${FLEX_INCLUDE_DIRS}
)

target_compile_options (paracl_syntax PRIVATE
    ${COMMON_COMPILE_OPTIONS}
    $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
)

target_link_options (paracl_syntax PRIVATE
    $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
)

if(BUILD_TESTS)
    enable_testing()

    add_executable (paracl_tests
        tests/unit/test_main.cpp
        tests/unit/test_errors.cpp
        tests/unit/test_context.cpp
    )

    target_include_directories (paracl_tests PRIVATE include)

    target_compile_options (paracl_tests PRIVATE
        ${COMMON_COMPILE_OPTIONS}
        ${SANITIZE_FLAGS}
    )

    target_link_options (paracl_tests PRIVATE
        ${SANITIZE_FLAGS}
    )

    find_package (GTest REQUIRED)
    target_link_libraries (paracl_tests PRIVATE GTest::gtest GTest::gtest_main)

    include (GoogleTest)
    gtest_discover_tests (paracl_tests)
endif()
