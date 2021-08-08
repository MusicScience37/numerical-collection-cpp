add_subdirectory(util)
add_subdirectory(multi_double)
add_subdirectory(auto_diff)
add_subdirectory(opt)
add_subdirectory(constants)
add_subdirectory(roots)
add_subdirectory(functions)
add_subdirectory(integration)
add_subdirectory(ode)

add_executable(num_collect_test_units_unity EXCLUDE_FROM_ALL unity_source.cpp)
target_link_libraries(
    num_collect_test_units_unity PRIVATE Catch2::Catch2WithMain num_collect
                                         num_prob_collect)
target_include_directories(num_collect_test_units_unity
                           PRIVATE ${NUM_COLLECT_TEST_INCLUDE_DIR})
target_check_clang_tidy(num_collect_test_units_unity)
