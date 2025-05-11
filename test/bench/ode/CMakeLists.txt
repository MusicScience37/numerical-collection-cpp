add_executable(num_collect_bench_sparse_linear_equation
               sparse_linear_equation_test.cpp)
target_add_to_benchmark(num_collect_bench_sparse_linear_equation)

macro(add_bench_ode PROBLEM)
    add_executable(
        num_collect_bench_ode_${PROBLEM} ${PROBLEM}_test.cpp diagram_common.cpp
                                         fixed_step_bench_executor.cpp)
    target_add_custom_benchmark(num_collect_bench_ode_${PROBLEM})
endmacro()

add_bench_ode(exponential)
add_bench_ode(external_force_vibration)
add_bench_ode(free_fall_in_resistance)
add_bench_ode(spring_movement)
add_bench_ode(pendulum_movement)
add_bench_ode(kaps_problem)
add_bench_ode(implicit_kaps_problem)
add_bench_ode(string_wave_1d)
add_bench_ode(rosenbrock_equation_solvers)
add_bench_ode(step_size_controllers)
add_bench_ode(spring_movement_fixed_step)
add_bench_ode(pendulum_movement_fixed_step)
