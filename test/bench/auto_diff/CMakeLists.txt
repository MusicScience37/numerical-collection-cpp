add_executable(bench_auto_diff quadratic_test.cpp sum_test.cpp
                               simple_jacobian_test.cpp)
target_add_to_benchmark(bench_auto_diff)
