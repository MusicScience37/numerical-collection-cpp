# Find libprofiler
find_library(
    LIBPROFILER_LIBRARY
    NAMES libprofiler.so
    HINTS /usr/lib /usr/local/lib REQUIRED)
message(STATUS "libprofiler found at ${LIBPROFILER_LIBRARY}")
add_library(gperftools::libprofiler SHARED IMPORTED)
set_target_properties(gperftools::libprofiler PROPERTIES IMPORTED_LOCATION
                                                         ${LIBPROFILER_LIBRARY})
# -fno-omit-frame-pointer is required for better profiling results.
target_compile_options(gperftools::libprofiler
                       INTERFACE -fno-omit-frame-pointer)

function(add_profiling TARGET)
    add_executable(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PRIVATE gperftools::libprofiler num_collect
                                            num_prob_collect)
endfunction()

add_subdirectory(logging)
add_subdirectory(rbf)
add_subdirectory(opt)
add_subdirectory(ode)
