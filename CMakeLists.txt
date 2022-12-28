cmake_minimum_required(VERSION 3.12)

project(
    num_collect
    VERSION 0.5.0
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

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
set(CONAN_COMPILER ${CONAN_SETTINGS_COMPILER})
set(CONAN_COMPILER_VERSION ${CONAN_SETTINGS_COMPILER_VERSION})
set(CONAN_LIBCXX ${CONAN_SETTINGS_COMPILER_LIBCXX})
conan_basic_setup()

# find packages installed with Conan
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(cpp_hash_tables REQUIRED)
find_package(tomlplusplus REQUIRED)
if(NUM_COLLECT_TESTING)
    find_package(Catch2 REQUIRED)
    find_package(trompeloeil REQUIRED)
    find_package(ApprovalTests REQUIRED)
    find_package(cpp_stat_bench REQUIRED)
    find_package(lyra REQUIRED)
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
    PATHS ${PIPENV_VENV_DIR}/bin ${PIPENV_VENV_DIR}/Scripts
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
    INTERFACE fmt::fmt Eigen3::Eigen cpp_hash_tables::cpp_hash_tables
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
    find_package(pngpp REQUIRED)
    add_subdirectory(examples)
endif()

add_subdirectory(config_schema)

if(NUM_COLLECT_ENABLE_PROFILING)
    add_subdirectory(profiling)
endif()
