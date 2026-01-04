cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ParseVersion)
parse_version(${CMAKE_CURRENT_SOURCE_DIR}/include/num_collect/version.h
              NUM_COLLECT)

project(
    num_collect
    VERSION ${NUM_COLLECT_VERSION}
    DESCRIPTION
        "A collection of algorithms in numerical analysis implemented in C++"
    LANGUAGES CXX)
set(FULL_PROJECT_NAME "numerical-collection-cpp")
message(STATUS "${FULL_PROJECT_NAME} version ${PROJECT_VERSION}")
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")

# ##############################################################################
# Configuration of CTest
# ##############################################################################
set(BUILDNAME
    "${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}"
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake
               ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
include(CTest)

# ##############################################################################
# Options
# ##############################################################################
option(NUM_COLLECT_BUILD_DOC "build documentation of ${FULL_PROJECT_NAME}" OFF)
option(NUM_COLLECT_TESTING "enable tests of ${FULL_PROJECT_NAME}" OFF)
option(NUM_COLLECT_BUILD_EXAMPLES "build examples of ${FULL_PROJECT_NAME}" OFF)
option(NUM_COLLECT_BUILD_EXPERIMENTS
       "build experiments of ${FULL_PROJECT_NAME}" OFF)
option(NUM_COLLECT_ENABLE_PROFILING "enable profiling" OFF)
option(NUM_COLLECT_ENABLE_INSTALL
       "enable CMake target to install ${FULL_PROJECT_NAME}" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "export compile commands" FORCE)
set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "build shared libraries")

# ##############################################################################
# Directory of binaries
# ##############################################################################
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

# ##############################################################################
# Dependencies
# ##############################################################################
find_package(Threads REQUIRED)
find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(cpp-hash-tables REQUIRED)
find_package(concurrentqueue REQUIRED)
find_package(nanoflann REQUIRED)

# toml++
find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
add_library(tomlplusplus::tomlplusplus ALIAS PkgConfig::tomlplusplus)

if(NUM_COLLECT_TESTING)
    find_package(Catch2 REQUIRED)
    find_package(trompeloeil REQUIRED)
    find_package(cpp-stat-bench REQUIRED)
    find_package(cpp-msgpack-light REQUIRED)
    find_package(ZLIB REQUIRED)

    # ApprovalTests.
    find_path(APPROVAL_TESTS_CPP_INCLUDE_DIRS "ApprovalTests.hpp")
    add_library(ApprovalTests_ApprovalTests INTERFACE)
    target_include_directories(ApprovalTests_ApprovalTests
                               INTERFACE ${APPROVAL_TESTS_CPP_INCLUDE_DIRS})
    add_library(ApprovalTests::ApprovalTests ALIAS ApprovalTests_ApprovalTests)
endif()

if(NUM_COLLECT_BUILD_EXAMPLES OR NUM_COLLECT_BUILD_EXPERIMENTS)
    # libpng
    find_package(PNG REQUIRED)
endif()

if(NUM_COLLECT_BUILD_EXAMPLES
   OR NUM_COLLECT_BUILD_EXPERIMENTS
   OR NUM_COLLECT_TESTING)
    find_package(cpp-plotly-plotter REQUIRED)
    find_package(lyra REQUIRED)
    find_package(metis REQUIRED)
endif()

# ##############################################################################
# Import modules for configurations
# ##############################################################################
include(TargetLinkSystemLibrary)
include(CppWarningFlags)
include(poetry)
include(ConfigureSanitizer)
include(ConfigureCcache)

# Fix a configuration in a module
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        num_collect_cpp_warnings INTERFACE # For macro of fmt library.
                                           -Wno-unused-local-typedef)
endif()

# ##############################################################################
# Configure Python
# ##############################################################################
find_program(
    PYTHON_EXECUTABLE
    NAMES python3 python
    PATHS ${POETRY_VENV_DIR}/bin ${POETRY_VENV_DIR}/Scripts
    NO_DEFAULT_PATH)
if(PYTHON_EXECUTABLE AND POETRY_EXECUTABLE)
    message(STATUS "Found Python: ${PYTHON_EXECUTABLE}")
else()
    message(STATUS "Python not found for Python interface")
endif()

# ##############################################################################
# Directories of source codes
# ##############################################################################
set(NUM_COLLECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(NUM_COLLECT_GENERATED_HEADER_DIR ${CMAKE_BINARY_DIR}/include)

# ##############################################################################
# Include subdirectories
# ##############################################################################
add_subdirectory(src)

if(NUM_COLLECT_BUILD_DOC)
    add_subdirectory(doc)
endif()

if(NUM_COLLECT_TESTING OR NUM_COLLECT_BUILD_EXAMPLES)
    add_subdirectory(problems)
endif()

if(NUM_COLLECT_TESTING)
    add_subdirectory(test)
endif()

if(NUM_COLLECT_BUILD_EXAMPLES OR NUM_COLLECT_BUILD_EXPERIMENTS)
    add_subdirectory(examples)
endif()

if(NUM_COLLECT_BUILD_EXPERIMENTS)
    add_subdirectory(experiments)
endif()

add_subdirectory(config_schema)

if(NUM_COLLECT_ENABLE_PROFILING)
    add_subdirectory(profiling)
endif()

# ##############################################################################
# Installation
# ##############################################################################
if(NUM_COLLECT_ENABLE_INSTALL)
    install(
        TARGETS num_collect
        EXPORT num-collect-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_PREFIX})

    install(
        DIRECTORY ${NUM_COLLECT_SOURCE_DIR}/include/num_collect
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
        PATTERN "*.h*")

    install(
        DIRECTORY ${NUM_COLLECT_GENERATED_HEADER_DIR}/num_collect
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
        PATTERN "*.h*")

    install(
        EXPORT num-collect-targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/num-collect
        NAMESPACE num_collect::)

    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        cmake/num-collect-config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/num-collect-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/num-collect")

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/num-collect-config.cmake"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/num-collect")
endif()
