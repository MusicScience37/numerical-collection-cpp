add_executable(num_collect_bench_ode_spring_movement spring_movement_test.cpp)
target_add_to_benchmark(num_collect_bench_ode_spring_movement)

add_executable(num_collect_bench_ode_external_force_vibration
               external_force_vibration_test.cpp)
target_add_to_benchmark(num_collect_bench_ode_external_force_vibration)

add_executable(num_collect_bench_ode_free_fall_in_resistance
               free_fall_in_resistance_test.cpp)
target_add_to_benchmark(num_collect_bench_ode_free_fall_in_resistance)

add_executable(num_collect_bench_ode_pendulum_movement
               pendulum_movement_test.cpp)
target_add_to_benchmark(num_collect_bench_ode_pendulum_movement)

add_executable(num_collect_bench_ode_step_size_controllers
               step_size_controllers_test.cpp)
target_add_to_benchmark(num_collect_bench_ode_step_size_controllers)

add_subdirectory(diagrams)
