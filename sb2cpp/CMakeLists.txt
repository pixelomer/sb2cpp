cmake_minimum_required(VERSION 3.14)
project(sb2cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_CXX_FLAGS "-O3 -Wall")
else()
    set(CMAKE_CXX_FLAGS "-Wall")
endif()

add_library(
    sb2cpp_lib
    lib/ast.cc
    lib/parse.cc
    lib/tokenize.cc
)
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
    )
    target_link_libraries(sb2cpp_test GTest::gtest_main)
    target_link_libraries(sb2cpp_test sb2cpp_lib)

    include(GoogleTest)
    gtest_discover_tests(sb2cpp_test)
endif()