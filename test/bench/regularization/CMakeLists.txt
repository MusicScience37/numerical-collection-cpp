add_executable(num_collect_bench_reg_blur_sine blur_sine_test.cpp)
target_add_to_benchmark(num_collect_bench_reg_blur_sine)

add_executable(num_collect_bench_reg_shrinkage_operator
               shrinkage_operator_test.cpp)
target_add_to_benchmark(num_collect_bench_reg_shrinkage_operator)
