cmake_minimum_required (VERSION 3.15)

project (ParaCL LANGUAGES C CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

option (SANITIZE "Enable Address and Undefined sanitizers" OFF)
option (BUILD_TESTS "Build tests" ON)
option (BUILD_COMPILER "Build LLVM-based compiler target" OFF)

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

if(BUILD_COMPILER)
    find_package (LLVM REQUIRED CONFIG)
    message (STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
endif()

set (COMPILE_NAME ${PROJECT_NAME})
set (INTERP_NAME "${PROJECT_NAME}i")

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

set (PARACL_FRONTEND_SOURCES
    ${BISON_ParaCLParser_OUTPUTS}
    ${FLEX_ParaCLLexer_OUTPUTS}
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hh
)

set (PARACL_CODEGEN_SOURCES
    src/codegen/codegen_context.cpp
    src/codegen/expr_codegen.cpp
    src/codegen/stmt_codegen.cpp
    src/codegen/module_codegen.cpp
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

function (paracl_apply_common_target_settings target_name)
    target_include_directories (${target_name} PRIVATE ${PARACL_INCLUDE_DIRS})

    target_compile_options (${target_name} PRIVATE
        ${COMMON_COMPILE_OPTIONS}
        ${SANITIZE_FLAGS}
    )

    target_link_options (${target_name} PRIVATE
        ${SANITIZE_FLAGS}
    )
endfunction()

add_executable (${INTERP_NAME}
    src/main.cpp
    ${PARACL_FRONTEND_SOURCES}
)

set_target_properties (${INTERP_NAME} PROPERTIES OUTPUT_NAME paracli)

target_compile_definitions (${INTERP_NAME} PRIVATE PARACL_INTERP=1)
paracl_apply_common_target_settings (${INTERP_NAME})

if(BUILD_COMPILER)
    llvm_map_components_to_libnames (PARACL_LLVM_LIBS support core codegen irreader)

    add_executable (${COMPILE_NAME}
        src/main.cpp
        ${PARACL_FRONTEND_SOURCES}
        ${PARACL_CODEGEN_SOURCES}
    )

    set_target_properties (${COMPILE_NAME} PROPERTIES OUTPUT_NAME paracl)

    target_compile_definitions (${COMPILE_NAME} PRIVATE
        PARACL_CODEGEN=1
        $<$<CONFIG:Debug>:DUMPALL>
    )

    paracl_apply_common_target_settings (${COMPILE_NAME})

    target_include_directories (${COMPILE_NAME} SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
    target_link_libraries (${COMPILE_NAME} PRIVATE ${PARACL_LLVM_LIBS})
endif()

if(BUILD_TESTS)
    enable_testing()

    add_executable (paracl_tests
        tests/unit/test_main.cpp
        tests/unit/test_errors.cpp
        tests/unit/test_context.cpp
        tests/unit/test_ast_nodes.cpp
        tests/unit/test_ast_builder.cpp
        tests/unit/test_syntax.cpp
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
