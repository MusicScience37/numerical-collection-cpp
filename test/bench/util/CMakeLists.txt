add_executable(bench_util_summation summation_test.cpp)
target_add_to_benchmark(bench_util_summation)

add_executable(bench_util_stack stack_test.cpp)
target_add_to_benchmark(bench_util_stack)
