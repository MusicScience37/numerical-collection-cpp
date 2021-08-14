#include "auto_diff/backward/create_diff_variable_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "auto_diff/backward/differentiate_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "auto_diff/backward/graph/node_differentiator_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "auto_diff/backward/graph/node_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "auto_diff/backward/variable_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "auto_diff/forward/create_diff_variable_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "auto_diff/forward/make_jacobian_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "auto_diff/forward/variable_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "constants/cbrt_test.cpp"   // NOLINT(bugprone-suspicious-include)
#include "constants/ceil_test.cpp"   // NOLINT(bugprone-suspicious-include)
#include "constants/exp_test.cpp"    // NOLINT(bugprone-suspicious-include)
#include "constants/expm1_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "constants/floor_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "constants/half_test.cpp"   // NOLINT(bugprone-suspicious-include)
#include "constants/impl/exp_maclaurin_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "constants/impl/expm1_maclaurin_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "constants/impl/log1m_maclaurin_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "constants/impl/pow_pos_int_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "constants/log1p_test.cpp"   // NOLINT(bugprone-suspicious-include)
#include "constants/log_test.cpp"     // NOLINT(bugprone-suspicious-include)
#include "constants/napier_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "constants/one_test.cpp"     // NOLINT(bugprone-suspicious-include)
#include "constants/pi_test.cpp"      // NOLINT(bugprone-suspicious-include)
#include "constants/pow_test.cpp"     // NOLINT(bugprone-suspicious-include)
#include "constants/root_test.cpp"    // NOLINT(bugprone-suspicious-include)
#include "constants/sqrt_test.cpp"    // NOLINT(bugprone-suspicious-include)
#include "constants/trunc_test.cpp"   // NOLINT(bugprone-suspicious-include)
#include "constants/zero_test.cpp"    // NOLINT(bugprone-suspicious-include)
#include "functions/legendre_roots_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "functions/legendre_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "integration/de_finite_integrator_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "integration/de_infinite_integrator_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "integration/de_semi_infinite_integrator_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "integration/gauss_legendre_integrator_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "integration/gauss_legendre_kronrod_integrator_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "multi_double/basic_operations_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "multi_double/quad_test.cpp"     // NOLINT(bugprone-suspicious-include)
#include "ode/avf/avf2_formula_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/avf/avf3_formula_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/avf/avf4_formula_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/embedded_solver_test.cpp"   // NOLINT(bugprone-suspicious-include)
#include "ode/non_embedded_formula_wrapper_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/runge_kutta/implicit_euler_formula_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/runge_kutta/rk4_formula_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/runge_kutta/rkf45_formula_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/runge_kutta/ros34pw3_formula_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/runge_kutta/ros3w_formula_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/runge_kutta/semi_implicit_formula_solver_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/runge_kutta/tanaka1_formula_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/runge_kutta/tanaka2_formula_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "ode/simple_solver_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/adaptive_diagonal_curves_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/backtracking_line_searcher_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/bfgs_optimizer_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/conjugate_gradient_optimizer_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/dfp_optimizer_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/dividing_rectangles_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/downhill_simplex_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/golden_section_search_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/impl/ternary_vector_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/newton_optimizer_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "opt/steepest_descent_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "roots/newton_raphson_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "util/assert_test.cpp"           // NOLINT(bugprone-suspicious-include)
#include "util/exception_test.cpp"        // NOLINT(bugprone-suspicious-include)
#include "util/is_eigen_matrix_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "util/is_eigen_vector_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "util/iteration_logger_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "util/kahan_adder_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "util/norm_test.cpp"         // NOLINT(bugprone-suspicious-include)
