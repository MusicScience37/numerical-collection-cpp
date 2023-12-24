add_executable(num_collect_bench_opt_multi_quadratic_function
               multi_quadratic_function_test.cpp)
target_add_to_benchmark(num_collect_bench_opt_multi_quadratic_function)

add_executable(num_collect_bench_opt_powell4_function powell4_function_test.cpp)
target_add_to_benchmark(num_collect_bench_opt_powell4_function)

add_executable(num_collect_bench_opt_rosenbrock_function
               rosenbrock_function_test.cpp)
target_add_to_benchmark(num_collect_bench_opt_rosenbrock_function)

add_executable(num_collect_bench_opt_shekel_function shekel_function_test.cpp)
target_add_to_benchmark(num_collect_bench_opt_shekel_function)

add_executable(num_collect_bench_opt_quadratic_function
               quadratic_function_test.cpp)
target_add_to_benchmark(num_collect_bench_opt_quadratic_function)

add_executable(num_collect_bench_opt_random_quadratic_function
               random_quadratic_function_test.cpp)
target_add_to_benchmark(num_collect_bench_opt_random_quadratic_function)

add_executable(num_collect_bench_opt_vibrated_quadratic_function
               vibrated_quadratic_function_test.cpp)
target_add_to_benchmark(num_collect_bench_opt_vibrated_quadratic_function)

add_executable(num_collect_bench_opt_single_variate_multi_optima_function
               single_variate_multi_optima_function_test.cpp)
target_add_to_benchmark(
    num_collect_bench_opt_single_variate_multi_optima_function)
