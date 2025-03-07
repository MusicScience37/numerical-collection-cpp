add_library(num_collect_bench_opt_common STATIC
            function_value_history_writer.cpp)
target_link_libraries(
    num_collect_bench_opt_common
    PUBLIC cpp_stat_bench::stat_bench num_collect num_prob_collect
           num_collect_test_utils num_collect_bench_common)

function(target_add_to_opt_benchmark TARGET)
    target_link_libraries(${TARGET} PRIVATE num_collect_bench_opt_common)

    if(NUM_COLLECT_TEST_BENCHMARKS AND PYTHON_EXECUTABLE)
        add_test(
            NAME ${TARGET}
            COMMAND
                ${POETRY_EXECUTABLE} run
                ${CMAKE_CURRENT_SOURCE_DIR}/run_with_function_value_history.py
                $<TARGET_FILE:${TARGET}>)
    endif()
endfunction()

add_executable(num_collect_bench_opt_random_multi_quadratic_function
               random_multi_quadratic_function_test.cpp)
target_add_to_opt_benchmark(
    num_collect_bench_opt_random_multi_quadratic_function)

add_executable(num_collect_bench_opt_multi_variate_multi_optima_function
               multi_variate_multi_optima_function_test.cpp)
target_add_to_opt_benchmark(
    num_collect_bench_opt_multi_variate_multi_optima_function)

add_executable(
    num_collect_bench_opt_multi_variate_difficult_multi_optima_function
    multi_variate_difficult_multi_optima_function_test.cpp)
target_add_to_opt_benchmark(
    num_collect_bench_opt_multi_variate_difficult_multi_optima_function)

add_executable(num_collect_bench_opt_powell4_function powell4_function_test.cpp)
target_add_to_opt_benchmark(num_collect_bench_opt_powell4_function)

add_executable(num_collect_bench_opt_rosenbrock_function
               rosenbrock_function_test.cpp)
target_add_to_opt_benchmark(num_collect_bench_opt_rosenbrock_function)

add_executable(num_collect_bench_opt_shekel_function shekel_function_test.cpp)
target_add_to_opt_benchmark(num_collect_bench_opt_shekel_function)

add_executable(num_collect_bench_opt_random_quadratic_function
               random_quadratic_function_test.cpp)
target_add_to_opt_benchmark(num_collect_bench_opt_random_quadratic_function)

add_executable(num_collect_bench_opt_single_variate_multi_optima_function
               single_variate_multi_optima_function_test.cpp)
target_add_to_opt_benchmark(
    num_collect_bench_opt_single_variate_multi_optima_function)
