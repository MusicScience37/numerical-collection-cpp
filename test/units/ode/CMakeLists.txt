add_executable(
    num_collect_test_units_ode
    runge_kutta/rk4_formula_test.cpp runge_kutta/simple_solver_test.cpp
    runge_kutta/rkf45_formula_test.cpp runge_kutta/embedded_solver_test.cpp
    runge_kutta/semi_implicit_formula_solver_test.cpp)
target_add_catch2(num_collect_test_units_ode)
