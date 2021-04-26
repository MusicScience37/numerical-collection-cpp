cmake_minimum_required(VERSION 3.12)

project(
    num_collect_test
    VERSION 0.0.0
    DESCRIPTION "test of num_collect package for Conan"
    LANGUAGES CXX)
message(STATUS "num_collect_test version ${PROJECT_VERSION}")
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")

# for find_package
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD
    17
    CACHE STRING "C++ standard")

find_package(num_collect REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example num_collect::num_collect)
enable_testing()
add_test(example example)
