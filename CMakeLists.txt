cmake_minimum_required(VERSION 3.24)

project(Playground LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0  # use a recent stable tag
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Your test executable
add_executable(Playground
        src/main.cpp
        src/unsorted.cpp)
target_link_libraries(Playground PRIVATE GTest::gtest)

include(GoogleTest)
gtest_discover_tests(Playground)

