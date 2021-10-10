set(SOURCE_FILES
    auto_diff/backward/create_diff_variable_test.cpp
    auto_diff/backward/differentiate_test.cpp
    auto_diff/backward/graph/node_differentiator_test.cpp
    auto_diff/backward/graph/node_test.cpp
    auto_diff/backward/variable_math_test.cpp
    auto_diff/backward/variable_test.cpp
    auto_diff/forward/create_diff_variable_test.cpp
    auto_diff/forward/make_jacobian_test.cpp
    auto_diff/forward/variable_math_test.cpp
    auto_diff/forward/variable_test.cpp
    constants/cbrt_test.cpp
    constants/ceil_test.cpp
    constants/exp_test.cpp
    constants/expm1_test.cpp
    constants/floor_test.cpp
    constants/half_test.cpp
    constants/impl/exp_maclaurin_test.cpp
    constants/impl/expm1_maclaurin_test.cpp
    constants/impl/log1m_maclaurin_test.cpp
    constants/impl/pow_pos_int_test.cpp
    constants/log1p_test.cpp
    constants/log_test.cpp
    constants/napier_test.cpp
    constants/one_test.cpp
    constants/pi_test.cpp
    constants/pow_test.cpp
    constants/root_test.cpp
    constants/sqrt_test.cpp
    constants/trunc_test.cpp
    constants/zero_test.cpp
    functions/legendre_roots_test.cpp
    functions/legendre_test.cpp
    integration/de_finite_integrator_test.cpp
    integration/de_infinite_integrator_test.cpp
    integration/de_semi_infinite_integrator_test.cpp
    integration/gauss_legendre_integrator_test.cpp
    integration/gauss_legendre_kronrod_integrator_test.cpp
    interp/kernel/calc_kernel_mat_test.cpp
    interp/kernel/euclidean_distance_test.cpp
    interp/kernel/gaussian_rbf_test.cpp
    interp/kernel/impl/auto_regularizer_test.cpp
    interp/kernel/impl/kernel_interpolator_impl_test.cpp
    interp/kernel/impl/kernel_parameter_optimizer_test.cpp
    interp/kernel/impl/self_adjoint_kernel_solver_test.cpp
    interp/kernel/kernel_interpolator_test.cpp
    interp/kernel/rbf_kernel_test.cpp
    multi_double/basic_operations_test.cpp
    multi_double/quad_test.cpp
    ode/avf/avf2_formula_test.cpp
    ode/avf/avf3_formula_test.cpp
    ode/avf/avf4_formula_test.cpp
    ode/embedded_solver_test.cpp
    ode/non_embedded_formula_wrapper_test.cpp
    ode/runge_kutta/implicit_euler_formula_test.cpp
    ode/runge_kutta/rk4_formula_test.cpp
    ode/runge_kutta/rkf45_formula_test.cpp
    ode/runge_kutta/ros34pw3_formula_test.cpp
    ode/runge_kutta/ros3w_formula_test.cpp
    ode/runge_kutta/semi_implicit_formula_solver_test.cpp
    ode/runge_kutta/tanaka1_formula_test.cpp
    ode/runge_kutta/tanaka2_formula_test.cpp
    ode/simple_solver_test.cpp
    opt/adaptive_diagonal_curves_test.cpp
    opt/backtracking_line_searcher_test.cpp
    opt/bfgs_optimizer_test.cpp
    opt/conjugate_gradient_optimizer_test.cpp
    opt/dfp_optimizer_test.cpp
    opt/dividing_rectangles_test.cpp
    opt/downhill_simplex_test.cpp
    opt/function_object_wrapper_test.cpp
    opt/golden_section_search_test.cpp
    opt/heuristic_1dim_optimizer_test.cpp
    opt/impl/ternary_vector_test.cpp
    opt/newton_optimizer_test.cpp
    opt/sampling_optimizer_test.cpp
    opt/steepest_descent_test.cpp
    regularization/explicit_gcv_test.cpp
    regularization/explicit_l_curve_test.cpp
    regularization/full_gen_tikhonov_test.cpp
    regularization/tikhonov_test.cpp
    roots/newton_raphson_test.cpp
    util/assert_test.cpp
    util/exception_test.cpp
    util/get_size_test.cpp
    util/get_stack_trace_test.cpp
    util/is_eigen_matrix_test.cpp
    util/is_eigen_vector_test.cpp
    util/iteration_logger_test.cpp
    util/kahan_adder_test.cpp
    util/norm_test.cpp
    util/safe_cast_test.cpp
)
