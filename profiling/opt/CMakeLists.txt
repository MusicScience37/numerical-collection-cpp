add_executable(num_collect_prof_adaptive_diagonal_curves
               adaptive_diagonal_curves.cpp)
target_link_libraries(
    num_collect_prof_adaptive_diagonal_curves
    PRIVATE gperftools::libprofiler num_collect num_prob_collect)
# -fno-omit-frame-pointer is required for better profiling results.
target_compile_options(num_collect_prof_adaptive_diagonal_curves
                       PRIVATE -fno-omit-frame-pointer)
