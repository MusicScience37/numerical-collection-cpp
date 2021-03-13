cmake_minimum_required(VERSION 3.12)

if(DEFINED PROJECT_NAME)
    set(NUM_COLLECT_SUBPROJECT ON)
else()
    set(NUM_COLLECT_SUBPROJECT OFF)
endif()

project(
    num_collect
    VERSION 0.0.0
    DESCRIPTION
        "A collection of algorithms in numerical analysis implemented in C++"
    LANGUAGES CXX)
message(STATUS "numerical-collection-cpp version ${PROJECT_VERSION}")
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")

set(BUILDNAME
    "${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}"
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake
               ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
include(CTest)

option(NUM_COLLECT_BUILD_DOC "build documentation of numerical-collection-cpp"
       OFF)
option(NUM_COLLECT_TESTING "enable tests of numerical-collection-cpp" OFF)

set(CMAKE_CXX_STANDARD
    17
    CACHE STRING "C++ standard")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

# add_subdirectory(extern)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(TargetLinkSystemLibrary)
include(ConfigureClangTidy)
include(CppWarningFlags)

set(NUM_COLLECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(NUM_COLLECT_BUILD_DOC)
    # add_subdirectory(doc)
endif()

if(NUM_COLLECT_TESTING)
    # add_subdirectory(test)
endif()
