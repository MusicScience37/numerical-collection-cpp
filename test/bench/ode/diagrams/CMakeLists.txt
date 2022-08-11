add_executable(num_collect_bench_ode_diagram_exponential exponential_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_diagram_exponential)

add_executable(num_collect_bench_ode_diagram_external_force_vibration
               external_force_vibration_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_diagram_external_force_vibration)

add_executable(num_collect_bench_ode_diagram_free_fall_in_resistance
               free_fall_in_resistance_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_diagram_free_fall_in_resistance)

add_executable(num_collect_bench_ode_diagram_spring_movement
               spring_movement_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_diagram_spring_movement)

add_executable(num_collect_bench_ode_diagram_kaps_problem kaps_problem_test.cpp)
add_bench_using_pybind11(num_collect_bench_ode_diagram_kaps_problem)
