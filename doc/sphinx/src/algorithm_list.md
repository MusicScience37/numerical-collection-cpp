# Algorithms

## Automatic Differentiation (`auto_diff` Module)

| Algorithm                                                    | Namespace                          |
| :----------------------------------------------------------- | :--------------------------------- |
| Backward-mode automatic differentiation {cite:p}`Kubota1998` | `num_collect::auto_diff::backward` |
| Forward-mode automatic differentiation {cite:p}`Kubota1998`  | `num_collect::auto_diff::forward`  |

## Optimization (`opt` Module)

| Algorithm                                                                  | Class                                            |
| :------------------------------------------------------------------------- | :----------------------------------------------- |
| Adaptive diagonal curves (ADC) {cite:p}`Sergeyev2006`                      | `num_collect::opt::adaptive_diagonal_curves`     |
| Backtracking line search                                                   | `num_collect::opt::backtracking_line_searcher`   |
| Conjugate gradient method                                                  | `num_collect::opt::conjugate_gradient_optimizer` |
| Dividing rectangles (DIRECT) method {cite:p}`Jones1993`                    | `num_collect::opt::dividing_rectangles`          |
| Downhill simplex method {cite:p}`Press2007`                                | `num_collect::opt::downhill_simplex`             |
| Downhill simplex method with simulated annealing {cite:p}`Press2007`       | `num_collect::opt::annealing_downhill_simplex`   |
| Firefly algorithm {cite:p}`Yang2009,Yang2010`                              | `num_collect::opt::firefly_optimizer`            |
| Gaussian process optimization {cite:p}`Srinivas2010,Brochu2010`            | `num_collect::opt::gaussian_process_optimizer`   |
| Genetic algorithm {cite:p}`Iba1994` for functions of real-valued variables | `num_collect::opt::real_value_genetic_optimizer` |
| Quasi-Newton method with Broyden-Fletcher-Goldfarb-Shanno (BFGS) formula   | `num_collect::opt::bfgs_optimizer`               |
| Quasi-Newton method with Davidon-Fletcher-Powell (DFP) formula             | `num_collect::opt::dfp_optimizer`                |

## Regularization (`regularization` Module)

### Computation of Regularized Solutions

| Algorithm                                                                                                                                             | Class                                                                                     |
| :---------------------------------------------------------------------------------------------------------------------------------------------------- | :---------------------------------------------------------------------------------------- |
| Fast iterative shrinkage-thresholding algorithm (FISTA) {cite:p}`Beck2009`                                                                            | `num_collect::regularization::fista`                                                      |
| Tikhonov regularization {cite:p}`Elden1982,Hansen1994`                                                                                                | `num_collect::regularization::tikhonov`, `num_collect::regularization::full_gen_tikhonov` |
| Total generalized variation (TGV) regularization in 2nd order using alternating direction method of multipliers (ADMM) {cite:p}`Bredies2010,Boyd2010` | `num_collect::regularization::tgv2_admm`                                                  |
| Total variation (TV) regularization using alternating direction method of multipliers (ADMM) {cite:p}`Boyd2010`                                       | `num_collect::regularization::tv_admm`                                                    |

### Selection of Regularization Parameters

| Algorithm                          | Class                                                                                                                                         |
| :--------------------------------- | :-------------------------------------------------------------------------------------------------------------------------------------------- |
| Generalized cross validation (GCV) | `num_collect::regularization::explicit_gcv`, `num_collect::regularization::implicit_gcv`                                                      |
| L-curve                            | `num_collect::regularization::explicit_l_curve`, `num_collect::regularization::approx_l_curve`, `num_collect::regularization::spline_l_curve` |

## RBF Interpolation (`rbf` Module)

### Interpolator Using RBF Interpolation

| Type of Length Parameter | Type of RBF | Use of Polynomial Terms | Alias for Class                                                                                |
| :----------------------- | :---------- | :---------------------- | :--------------------------------------------------------------------------------------------- |
| Local                    | Global      | No                      | `num_collect::rbf::local_rbf_interpolator`                                                     |
| Global                   | Global      | No                      | `num_collect::rbf::global_rbf_interpolator`, `num_collect::rbf::gaussian_process_interpolator` |
| Local                    | Compact     | No                      | `num_collect::rbf::local_csrbf_interpolator`                                                   |
| Global                   | Global      | Yes                     | `num_collect::rbf::global_rbf_polynomial_interpolator`                                         |

### Selection of Length Parameters

| Algorithm                                                                        | Class / Function                                                     |
| :------------------------------------------------------------------------------- | :------------------------------------------------------------------- |
| Maximum likelihood estimator (MLE) for length parameters {cite:p}`Scheuerer2011` | `optimize_length_parameter_scale` member functions in interpolators. |

### Selection of Points

| Algorithm                           | Class / Function                          |
| :---------------------------------- | :---------------------------------------- |
| Halton nodes {cite:p}`Fornberg2015` | `num_collect::rbf::generate_halton_nodes` |

### RBFs

| RBF                                                               | Class                                               |
| :---------------------------------------------------------------- | :-------------------------------------------------- |
| Gaussian                                                          | `num_collect::rbf::rbfs::gaussian_rbf`              |
| Inverse multi-quadric                                             | `num_collect::rbf::rbfs::inverse_multi_quadric_rbf` |
| Inverse quadratic                                                 | `num_collect::rbf::rbfs::inverse_quadratic_rbf`     |
| Multi-quadric                                                     | `num_collect::rbf::rbfs::multi_quadric_rbf`         |
| Polyharmonic spline {cite:p}`Fornberg2015`                        | `num_collect::rbf::rbfs::polyharmonic_spline_rbf`   |
| Sech                                                              | `num_collect::rbf::rbfs::sech_rbf`                  |
| Wendland's Compactly Supported RBF (CSRBF) {cite:p}`Wendland1995` | `num_collect::rbf::rbfs::wendland_csrbf`            |
| RBF of thin plate spline {cite:p}`Ghosh2010`                      | `num_collect::rbf::rbfs::thin_plate_spline_rbf`     |

### Operators

| Operator                                                  | Class                                                          |
| :-------------------------------------------------------- | :------------------------------------------------------------- |
| Biharmonic operator $\triangle^2$                         | `num_collect::rbf::operators::biharmonic_operator`             |
| Function value operator (identity)                        | `num_collect::rbf::operators::function_value_operator`         |
| Gradient operator $\nabla$                                | `num_collect::rbf::operators::gradient_operator`               |
| Gradient of Laplacian operator $\nabla \triangle$         | `num_collect::rbf::operators::laplacian_gradient_operator`     |
| Hessian operator $\nabla^2$                               | `num_collect::rbf::operators::hessian_operator`                |
| Laplacian operator $\triangle$                            | `num_collect::rbf::operators::laplacian_operator`              |
| Partial derivative operator $\partial / \partial r_i$     | `num_collect::rbf::operators::partial_derivative_operator`     |
| Third-order derivative operator $d^3 / dx^3$ (univariate) | `num_collect::rbf::operators::third_order_derivative_operator` |

### Splines using RBF Interpolation

| Spline                                | Class                                              |
| :------------------------------------ | :------------------------------------------------- |
| Thin plate spline {cite:p}`Ghosh2010` | `num_collect::rbf::thin_plate_spline_interpolator` |

## Root-Finding Algorithms (`roots` Module)

| Algorithm             | Class                                |
| :-------------------- | :----------------------------------- |
| Newton-Raphson method | `num_collect::roots::newton_raphson` |

## Special functions (`functions` Module)

| Function          | Class                              |
| :---------------- | :--------------------------------- |
| Gamma function    | `num_collect::functions::gamma`    |
| Legendre function | `num_collect::functions::legendre` |

## Numerical Integration (`integration` Module)

| Algorithm                                           | Range               | Class                                                         |
| :-------------------------------------------------- | :------------------ | :------------------------------------------------------------ |
| Double Exponential (DE) rule                        | $(a,b)$             | `num_collect::integration::de_finite_integrator`              |
| Double Exponential (DE) rule                        | $(-\infty, \infty)$ | `num_collect::integration::de_infinite_integrator`            |
| Double Exponential (DE) rule                        | $(0, \infty)$       | `num_collect::integration::de_semi_infinite_integrator`       |
| Gauss-Legendre formula                              | $(a,b)$             | `num_collect::integration::gauss_legendre_integrator`         |
| Gauss-Legendre-Kronrod formula {cite:p}`Laurie1997` | $(a,b)$             | `num_collect::integration::gauss_legendre_kronrod_integrator` |
| TANH rule                                           | $(a,b)$             | `num_collect::integration::tanh_finite_integrator`            |

## ODE Solvers (`ode` Module)

### Formulas of Runge-Kutta method

| Formula                                             | Class                                                 |
| :-------------------------------------------------- | :---------------------------------------------------- |
| ARK4(3)6L[2]SA-ERK formula {cite:p}`Kennedy2003`    | `num_collect::ode::runge_kutta::ark43_erk_formula`    |
| ARK4(3)6L[2]SA-ESDIRK formula {cite:p}`Kennedy2003` | `num_collect::ode::runge_kutta::ark43_esdirk_formula` |
| ARK5(4)8L[2]SA-ESDIRK formula {cite:p}`Kennedy2003` | `num_collect::ode::runge_kutta::ark54_esdirk_formula` |
| DOPRI5 formula {cite:p}`Hairer1991`                 | `num_collect::ode::runge_kutta::dopri5_formula`       |
| ESDIRK45c formula {cite:p}`Jorgensen2018`           | `num_collect::ode::runge_kutta::esdirk45_formula`     |
| Runge-Kutta 4 (RK4)                                 | `num_collect::ode::runge_kutta::rk4_formula`          |
| Runge-Kutta-Fehlberg 45 (RKF45)                     | `num_collect::ode::runge_kutta::rkf45_formula`        |
| 4th order SDIRK {cite:p}`Hairer1991`                | `num_collect::ode::runge_kutta::sdirk4_formula`       |
| Tanaka Formula 1                                    | `num_collect::ode::runge_kutta::tanaka1_formula`      |
| Tanaka Formula 2                                    | `num_collect::ode::runge_kutta::tanaka2_formula`      |

### Formulas of Rosenbrock method

| Formula                             | Class                                            |
| :---------------------------------- | :----------------------------------------------- |
| RODASP formula                      | `num_collect::ode::rosenbrock::rodasp_formula`   |
| RODASPR formula {cite:p}`Rang2015`  | `num_collect::ode::rosenbrock::rodaspr_formula`  |
| ROS3w formula {cite:p}`Rang2015`    | `num_collect::ode::rosenbrock::ros3w_formula`    |
| ROS34PW3 formula {cite:p}`Rang2015` | `num_collect::ode::rosenbrock::ros34pw3_formula` |

### Average vector field (AVF) method {cite:p}`Quispel2008`

| Order | Class                                 |
| ----: | :------------------------------------ |
|     2 | `num_collect::ode::avf::avf2_formula` |
|     3 | `num_collect::ode::avf::avf3_formula` |
|     4 | `num_collect::ode::avf::avf4_formula` |

### Symplectic formulas

| Formula                                                             | Class                                                      |
| :------------------------------------------------------------------ | :--------------------------------------------------------- |
| Leap-frog formula                                                   | `num_collect::ode::symplectic::leap_frog_formula`          |
| Fourth-order symplectic integration formula in {cite:p}`Forest1990` | `num_collect::ode::symplectic::symplectic_forest4_formula` |

### Other formulas

| Formula                | Class                                                   |
| :--------------------- | :------------------------------------------------------ |
| Implicit Euler formula | `num_collect::ode::runge_kutta::implicit_euler_formula` |

### Controllers of step sizes

| Algorithm                               | Class                                          |
| :-------------------------------------- | :--------------------------------------------- |
| Basic algorithm in {cite:p}`Hairer1993` | `num_collect::ode::basic_step_size_controller` |
| PI controller {cite:p}`Gustafsson1991`  | `num_collect::ode::pi_step_size_controller`    |

## Solvers of Linear Equations (`linear` Module)

### Solvers of Linear Equations

| Algorithm                                                | Class                                                                                                                             |
| :------------------------------------------------------- | :-------------------------------------------------------------------------------------------------------------------------------- |
| Algebraic multigrid (AMG) method {cite:p}`Ruge1987`      | `num_collect::linear::algebraic_multigrid_solver`                                                                                 |
| Gauss-Seidel iteration {cite:p}`Golub2013`               | `num_collect::linear::gauss_seidel_iterative_solver`                                                                              |
| Symmetric successive over-relaxation {cite:p}`Golub2013` | `num_collect::linear::symmetric_successive_over_relaxation`, `num_collect::linear::parallel_symmetric_successive_over_relaxation` |

### Ordering Algorithms

| Algorithm                                                            | Class                                                 |
| :------------------------------------------------------------------- | :---------------------------------------------------- |
| Cuthill-McKee (CM) ordering {cite:p}`Golub2013,Knabner2003`          | `num_collect::linear::cuthill_mckee_ordering`         |
| Reverse Cuthill-McKee (RCM) ordering {cite:p}`Golub2013,Knabner2003` | `num_collect::linear::reverse_cuthill_mckee_ordering` |

### Preconditioners

| Algorithm                                                   | Class                                     |
| :---------------------------------------------------------- | :---------------------------------------- |
| Algebraic multigrid (AMG) preconditioner {cite:p}`Ruge1987` | `num_collect::linear::amg_preconditioner` |

## Common Algorithms (`util` Module)

| Algorithm                           | Class                                                                              |
| :---------------------------------- | :--------------------------------------------------------------------------------- |
| Combination                         | `num_collect::util::combination`                                                   |
| Kahan summation {cite:p}`Kahan1965` | `num_collect::util::kahan_adder`                                                   |
| Gray code                           | `num_collect::util::binary_to_gray_code`, `num_collect::util::gray_code_to_binary` |
| Greatest common divisor (GCD)       | `num_collect::util::greatest_common_divisor`                                       |
| Least common multiple (LCM)         | `num_collect::util::least_common_multiple`                                         |
