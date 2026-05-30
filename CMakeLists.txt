cmake_minimum_required(VERSION 3.27)

project(Playground LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Dependencies.cmake)

# ------------------------------------------------------------------------------
# Build stb
# ------------------------------------------------------------------------------
add_library(stb_lib STATIC src/stb_implementation.cpp)
target_include_directories(stb_lib PUBLIC ${stb_SOURCE_DIR})

# ------------------------------------------------------------------------------
# Build tinyexr
# ------------------------------------------------------------------------------

add_library(tinyexr_lib STATIC
        ${tinyexr_SOURCE_DIR}/tinyexr.cc
        ${tinyexr_SOURCE_DIR}/deps/miniz/miniz.c)
target_include_directories(tinyexr_lib PUBLIC
        ${tinyexr_SOURCE_DIR}
        ${tinyexr_SOURCE_DIR}/deps/miniz)

# ------------------------------------------------------------------------------
# Build LImageWrapper
# ------------------------------------------------------------------------------
add_library(LImageWrapper INTERFACE)
target_include_directories(LImageWrapper INTERFACE include)
target_link_libraries( LImageWrapper INTERFACE stb_lib tinyexr_lib)

# ------------------------------------------------------------------------------
# Build LRandom
# ------------------------------------------------------------------------------
add_library(LRandom INTERFACE)
target_include_directories(LRandom INTERFACE include)

# ------------------------------------------------------------------------------
# Build LMath
# ------------------------------------------------------------------------------
add_library(LMath INTERFACE)
target_include_directories(LMath INTERFACE include)

# ------------------------------------------------------------------------------
# Build LTimer
# ------------------------------------------------------------------------------
add_library(LTimer INTERFACE)
target_include_directories(LTimer INTERFACE include)

# ------------------------------------------------------------------------------
# Build GTest
# ------------------------------------------------------------------------------
enable_testing()
add_executable(Playground
        src/main.cpp
        src/rng_test.cpp
        src/math_test.cpp
        src/unsorted.cpp)
target_link_libraries(Playground PRIVATE GTest::gtest SDL3::SDL3 LRandom LImageWrapper LMath LTimer)

include(GoogleTest)
gtest_discover_tests(Playground)

