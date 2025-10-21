include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_library(num_collect ${SOURCE_FILES})
target_include_directories(
    num_collect
    PUBLIC $<BUILD_INTERFACE:${NUM_COLLECT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${NUM_COLLECT_GENERATED_HEADER_DIR}>
           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(
    num_collect
    PUBLIC Threads::Threads
           fmt::fmt
           Eigen3::Eigen
           cpp_hash_tables::hash_tables
           tomlplusplus::tomlplusplus
           concurrentqueue::concurrentqueue
           nanoflann::nanoflann
           $<BUILD_INTERFACE:num_collect_cpp_warnings>)
target_compile_features(num_collect PUBLIC cxx_std_23)
target_add_ausan(num_collect)

set_target_properties(
    num_collect
    PROPERTIES C_VISIBILITY_PRESET hidden
               CXX_VISIBILITY_PRESET hidden
               VISIBILITY_INLINES_HIDDEN 1
               DEBUG_POSTFIX d
               VERSION ${PROJECT_VERSION})

include(GenerateExportHeader)
generate_export_header(
    num_collect
    EXPORT_FILE_NAME
        "${NUM_COLLECT_GENERATED_HEADER_DIR}/num_collect/impl/num_collect_export.h"
)

add_library(num_collect_unity EXCLUDE_FROM_ALL unity_source.cpp)
target_include_directories(
    num_collect_unity
    PUBLIC $<BUILD_INTERFACE:${NUM_COLLECT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${NUM_COLLECT_GENERATED_HEADER_DIR}>
           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(
    num_collect_unity
    PUBLIC Threads::Threads
           fmt::fmt
           Eigen3::Eigen
           cpp_hash_tables::hash_tables
           tomlplusplus::tomlplusplus
           concurrentqueue::concurrentqueue
           nanoflann::nanoflann
           $<BUILD_INTERFACE:num_collect_cpp_warnings>)
target_compile_features(num_collect_unity PUBLIC cxx_std_23)

# ##############################################################################
# Configure AVX CPU instruction
# ##############################################################################

include(CheckCXXSourceRuns)
set(CMAKE_REQUIRED_FLAGS)

if(MSVC)
    set(CMAKE_REQUIRED_FLAGS "/arch:AVX2")
else()
    set(CMAKE_REQUIRED_FLAGS "-mfma -mavx2")
endif()

check_cxx_source_runs(
    "
    #include <immintrin.h>
    int main()
    {
        __m256i a, b, c;
        const int src[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        int dst[8];
        a =  _mm256_loadu_si256( (__m256i*)src );
        b =  _mm256_loadu_si256( (__m256i*)src );
        c = _mm256_add_epi32( a, b );
        _mm256_storeu_si256( (__m256i*)dst, c );
        for( int i = 0; i < 8; i++ ){
        if( ( src[i] + src[i] ) != dst[i] ){
            return -1;
        }
        }
        return 0;
    }"
    HAVE_AVX2_EXTENSIONS)

if(HAVE_AVX2_EXTENSIONS)
    option(NUM_COLLECT_USE_AVX2 "use AVX2 instruction set" ON)
    if(NUM_COLLECT_USE_AVX2)
        if(MSVC)
            target_compile_options(num_collect PUBLIC /arch:AVX2)
        else()
            # FMA can be used if AVX2 can be used, but individual flags are
            # needed.
            target_compile_options(num_collect PUBLIC -mfma -mavx2)
        endif()
    endif()
endif()

# ##############################################################################
# Configure misc compiler flags
# ##############################################################################

# Flag for precise calculation of floating-point numbers.
if(MSVC)
    target_compile_options(num_collect PUBLIC /fp:precise)
else()
    target_compile_options(num_collect PUBLIC -fno-unsafe-math-optimizations)
endif()

# Workaround error in MSVC
if(MSVC)
    target_compile_options(num_collect PUBLIC /bigobj /EHsc /utf-8)
endif()

# OpenMP
find_package(OpenMP)
if(OpenMP_FOUND)
    option(NUM_COLLECT_USE_OPENMP "use OpenMP" ON)
    if(NUM_COLLECT_USE_OPENMP)
        target_link_libraries(num_collect PUBLIC OpenMP::OpenMP_CXX)
    endif()
endif()

option(NUM_COLLECT_ENABLE_EIGEN_RUNTIME_NO_MALLOC
       "Enable EIGEN_RUNTIME_NO_MALLOC." OFF)
if(NUM_COLLECT_ENABLE_EIGEN_RUNTIME_NO_MALLOC)
    target_compile_definitions(num_collect PUBLIC EIGEN_RUNTIME_NO_MALLOC)
endif()
