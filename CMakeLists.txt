cmake_minimum_required(VERSION 3.12)

project(
    num_collect
    VERSION 0.6.0
    DESCRIPTION
        "A collection of algorithms in numerical analysis implemented in C++"
    LANGUAGES CXX)
set(FULL_PROJECT_NAME "numerical-collection-cpp")
message(STATUS "${FULL_PROJECT_NAME} version ${PROJECT_VERSION}")
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")

set(BUILDNAME
    "${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}"
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake
               ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
include(CTest)

option(NUM_COLLECT_BUILD_DOC "build documentation of ${FULL_PROJECT_NAME}" OFF)
option(NUM_COLLECT_TESTING "enable tests of ${FULL_PROJECT_NAME}" OFF)
option(NUM_COLLECT_BUILD_EXAMPLES "build examples of ${FULL_PROJECT_NAME}" OFF)
option(NUM_COLLECT_ENABLE_PROFILING "enable profiling" OFF)

set(CMAKE_CXX_STANDARD
    20
    CACHE STRING "C++ standard")

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(cpp-hash-tables REQUIRED)
find_package(tomlplusplus REQUIRED)
if(NUM_COLLECT_TESTING)
    find_package(Catch2 REQUIRED)
    find_package(trompeloeil REQUIRED)
    find_package(cpp-stat-bench REQUIRED)
    find_package(lyra REQUIRED)

    # ApprovalTests.
    find_path(APPROVAL_TESTS_CPP_INCLUDE_DIRS "ApprovalTests.hpp")
    add_library(ApprovalTests_ApprovalTests INTERFACE)
    target_include_directories(ApprovalTests_ApprovalTests
                               INTERFACE ${APPROVAL_TESTS_CPP_INCLUDE_DIRS})
    add_library(ApprovalTests::ApprovalTests ALIAS ApprovalTests_ApprovalTests)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "export compile commands" FORCE)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(TargetLinkSystemLibrary)
include(CppWarningFlags)
include(CppFlags)
include(poetry)
include(ConfigureSanitizer)
include(ConfigureCcache)

target_compile_options(
    num_collect_cpp_warnings INTERFACE # For macro of fmt library.
                                       -Wno-unused-local-typedef)

find_program(
    PYTHON_EXECUTABLE
    NAMES python3 python
    PATHS ${POETRY_VENV_DIR}/bin ${POETRY_VENV_DIR}/Scripts
    NO_DEFAULT_PATH)
if(PYTHON_EXECUTABLE)
    message(STATUS "Found Python: ${PYTHON_EXECUTABLE}")
    find_package(pybind11 REQUIRED)
else()
    message(STATUS "Python not found for Python interface")
endif()

set(NUM_COLLECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(num_collect INTERFACE)
target_include_directories(num_collect
                           INTERFACE ${NUM_COLLECT_SOURCE_DIR}/include)
target_link_libraries(
    num_collect
    INTERFACE fmt::fmt Eigen3::Eigen cpp_hash_tables::hash_tables
              tomlplusplus::tomlplusplus num_collect_cpp_warnings
              num_collect_cpp_flags)

if(NUM_COLLECT_BUILD_DOC)
    add_subdirectory(doc)
endif()

if(NUM_COLLECT_TESTING OR NUM_COLLECT_BUILD_EXAMPLES)
    add_subdirectory(problems)
endif()

if(NUM_COLLECT_TESTING)
    add_subdirectory(test)
endif()

if(NUM_COLLECT_BUILD_EXAMPLES)
    # pngpp
    find_path(PNGPP_INCLUDE_DIRS "png++/color.hpp")
    add_library(pngpp_pngpp INTERFACE)
    target_include_directories(pngpp_pngpp SYSTEM
                               INTERFACE ${PNGPP_INCLUDE_DIRS})
    find_package(libpng REQUIRED)
    target_link_libraries(pngpp_pngpp INTERFACE png_static)
    add_library(pngpp::pngpp ALIAS pngpp_pngpp)

    add_subdirectory(examples)
endif()

add_subdirectory(config_schema)

if(NUM_COLLECT_ENABLE_PROFILING)
    add_subdirectory(profiling)
endif()
