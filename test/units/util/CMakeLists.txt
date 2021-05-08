add_executable(
    num_collect_test_units_util
    exception_test.cpp iteration_logger_test.cpp assert_test.cpp
    is_eigen_matrix_test.cpp is_eigen_vector_test.cpp)
target_add_catch2(num_collect_test_units_util)
