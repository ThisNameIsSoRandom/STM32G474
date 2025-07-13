cmake_minimum_required(VERSION 3.22)

project(uTests LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find or fetch Google Test
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Create static library
add_library(uTests STATIC
    src/test_runner.cpp
    tests/sample_test.cpp
)

# Include directories
target_include_directories(uTests 
    PUBLIC include
)

# Link Google Test
if(GTest_FOUND)
    target_link_libraries(uTests 
        PUBLIC 
            GTest::gtest 
            GTest::gtest_main
    )
else()
    target_link_libraries(uTests 
        PUBLIC 
            gtest 
            gtest_main
    )
endif()