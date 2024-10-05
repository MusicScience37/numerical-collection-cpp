add_library(num_collect_cpp_flags INTERFACE)

# Flag for precise calculation of floating-point numbers.
if(MSVC)
    target_compile_options(num_collect_cpp_flags INTERFACE /fp:precise)
else()
    target_compile_options(num_collect_cpp_flags
                           INTERFACE -fno-unsafe-math-optimizations)
endif()

# Workaround error in MSVC
if(MSVC)
    target_compile_options(num_collect_cpp_flags INTERFACE /bigobj /EHsc /utf-8)
endif()

# OpenMP
find_package(OpenMP)
if(OpenMP_FOUND)
    option(NUM_COLLECT_USE_OPENMP "use OpenMP" ON)
    if(NUM_COLLECT_USE_OPENMP)
        target_link_libraries(num_collect_cpp_flags
                              INTERFACE OpenMP::OpenMP_CXX)
    endif()
endif()

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
            target_compile_options(num_collect_cpp_flags INTERFACE /arch:AVX2)
        else()
            # FMA can be used if AVX2 can be used, but individual flags are
            # needed.
            target_compile_options(num_collect_cpp_flags INTERFACE -mfma -mavx2)
        endif()
    endif()
endif()
