add_executable(num_collect_bench_sparse_linear_equation
               sparse_linear_equation_test.cpp)
target_add_to_benchmark(num_collect_bench_sparse_linear_equation)

add_executable(num_collect_bench_ode_exponential exponential_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_exponential)

add_executable(num_collect_bench_ode_external_force_vibration
               external_force_vibration_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_external_force_vibration)

add_executable(num_collect_bench_ode_free_fall_in_resistance
               free_fall_in_resistance_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_free_fall_in_resistance)

add_executable(num_collect_bench_ode_spring_movement spring_movement_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_spring_movement)

add_executable(num_collect_bench_ode_pendulum_movement
               pendulum_movement_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_pendulum_movement)

add_executable(num_collect_bench_ode_kaps_problem kaps_problem_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_kaps_problem)

add_executable(num_collect_bench_ode_implicit_kaps_problem
               implicit_kaps_problem_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_implicit_kaps_problem)

add_executable(num_collect_bench_ode_string_wave_1d string_wave_1d_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_string_wave_1d)

add_executable(num_collect_bench_ode_rosenbrock_equation_solvers
               rosenbrock_equation_solvers_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_rosenbrock_equation_solvers)

add_executable(num_collect_bench_ode_step_size_controllers
               step_size_controllers_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_step_size_controllers)

add_executable(num_collect_bench_ode_fixed_step_spring_movement
               spring_movement_fixed_step_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_fixed_step_spring_movement)

add_executable(num_collect_bench_ode_fixed_step_pendulum_movement
               pendulum_movement_fixed_step_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_fixed_step_pendulum_movement)
