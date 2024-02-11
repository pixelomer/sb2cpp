cmake_minimum_required(VERSION 3.14)
project(sb2cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_CXX_FLAGS "-O3 -Wall -Werror=return-type")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Werror=return-type")
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_custom_command(
    OUTPUT runtime_list
    COMMAND ./generate_runtime_list.py > stdlib/Runtime_generated.cc
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS generate_runtime_list.py
)
add_custom_target(
    stdlib
    DEPENDS runtime_list
)

add_library(
    sb2cpp_lib
    libsb2cpp/ast.cc
    libsb2cpp/parser.cc
    libsb2cpp/source.cc
    libsb2cpp/transpiler.cc
    libsb2cpp/code_writer.cc
    libsb2cpp/tokenize.cc
    libsb2cpp/util.cc
    libsb2cpp/interpreter.cc
    stdlib/Runtime_generated.cc
    stdlib/Runtime.cc
    stdlib/Stack.cc
    stdlib/util.cc
    stdlib/Array.cc
    stdlib/RunLoop.cc
    stdlib/GraphicsWindow.cc
    stdlib/SBColor.cc
    stdlib/SDL2_gfx/SDL2_gfxPrimitives.c
    stdlib/SDL2_gfx/SDL2_rotozoom.c
)
add_dependencies(sb2cpp_lib stdlib)
target_link_libraries(sb2cpp_lib ${SDL2_LIBRARIES})

add_executable(sb2cpp main.cc)
target_link_libraries(sb2cpp sb2cpp_lib)

if(TEST)
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
        cmake_policy(SET CMP0135 NEW)
    endif()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(
        sb2cpp_test
        tests/tokenize_test.cc
        tests/parser_test.cc
        tests/source_test.cc
        tests/object_test.cc
        tests/interpreter_test.cc
        tests/stdlib_test.cc
    )
    target_link_libraries(sb2cpp_test GTest::gtest_main)
    target_link_libraries(sb2cpp_test sb2cpp_lib)

    include(GoogleTest)
    gtest_discover_tests(sb2cpp_test)
endif()