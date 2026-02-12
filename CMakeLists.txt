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

add_executable (paracl
    src/main.cpp
    src/syntax/driver.cpp
    ${BISON_ParaCLParser_OUTPUTS}
    ${FLEX_ParaCLLexer_OUTPUTS}
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hh
)

set (PARACL_INCLUDE_DIRS
    src/syntax
    include
    ${CMAKE_CURRENT_BINARY_DIR}
)

if(DEFINED FLEX_INCLUDE_DIRS AND NOT "${FLEX_INCLUDE_DIRS}" MATCHES "NOTFOUND")
    list (APPEND PARACL_INCLUDE_DIRS ${FLEX_INCLUDE_DIRS})
endif()

if(DEFINED FLEX_INCLUDE_DIR AND NOT "${FLEX_INCLUDE_DIR}" MATCHES "NOTFOUND")
    list (APPEND PARACL_INCLUDE_DIRS ${FLEX_INCLUDE_DIR})
endif()

target_include_directories (paracl PRIVATE ${PARACL_INCLUDE_DIRS})

target_compile_options (paracl PRIVATE
    ${COMMON_COMPILE_OPTIONS}
    ${SANITIZE_FLAGS}
)

target_link_options (paracl PRIVATE
    ${SANITIZE_FLAGS}
)

if(BUILD_TESTS)
    enable_testing()

    add_executable (paracl_tests
        tests/unit/test_main.cpp
        tests/unit/test_errors.cpp
        tests/unit/test_context.cpp
        tests/unit/test_ast_nodes.cpp
        tests/unit/test_ast_builder.cpp
        tests/unit/test_syntax.cpp
        src/syntax/driver.cpp
        ${BISON_ParaCLParser_OUTPUTS}
        ${FLEX_ParaCLLexer_OUTPUTS}
    )

    target_include_directories (paracl_tests PRIVATE ${PARACL_INCLUDE_DIRS})

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
