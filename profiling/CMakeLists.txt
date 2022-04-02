# Find libprofiler
find_library(
    LIBPROFILER_LIBRARY
    NAMES libprofiler.so
    HINTS /usr/lib /usr/local/lib REQUIRED)
message(STATUS "libprofiler found at ${LIBPROFILER_LIBRARY}")
add_library(gperftools::libprofiler SHARED IMPORTED)
set_target_properties(gperftools::libprofiler PROPERTIES IMPORTED_LOCATION
                                                         ${LIBPROFILER_LIBRARY})

add_subdirectory(opt)
