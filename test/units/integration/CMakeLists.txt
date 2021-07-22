add_executable(num_collect_test_units_integration
               gauss_legendre_integrator_test.cpp de_finite_integrator_test.cpp)
target_add_catch2(num_collect_test_units_integration)
