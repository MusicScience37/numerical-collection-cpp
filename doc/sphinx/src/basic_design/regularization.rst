Regularization
==================

Regularization Using L2 Norm
--------------------------------------

.. uml::

    title Abstract Model of Regularization Using L2 Norm

    interface regularized_solver_base<Scalar> {
        + using scalar_type = Scalar
        + residual_norm(param: scalar_type) : scalar_type
        + regularization_norm(param: scalar_type) : scalar_type
        + param_search_region() : pair<scalar_type, scalar_type>
    }

    interface explicit_regularized_solver_base<Scalar> {
        + first_derivative_of_residual_norm(param: scalar_type) : scalar_type
        + first_derivative_of_regularization_norm(param: scalar_type) : scalar_type
        + second_derivative_of_residual_norm(param: scalar_type) : scalar_type
        + second_derivative_of_regularization_norm(param: scalar_type) : scalar_type
        + sum_of_filter_factor(param: scalar_type) : scalar_type
    }
    regularized_solver_base <|-- explicit_regularized_solver_base

    class tikhonov<Coeff, Data> {
        + using coeff_type = Coeff
        + using data_type = Data
        + compute(coeff: const coeff_type&, data: const data_type&)
        + solve(param: scalar_type) : expression
    }
    explicit_regularized_solver_base <|-- tikhonov

    class general_tikhonov<Coeff, Data> {
        + using coeff_type = Coeff
        + using data_type = Data
        + compute(coeff: const coeff_type&, data: const data_type&, reg_coeff: const coeff_type&)
        + solve(param: scalar_type) : expression
    }
    explicit_regularized_solver_base <|-- general_tikhonov
    general_tikhonov o-- tikhonov

    interface param_searcher_base<Solver> {
        + using solver_type = Solver
        + using scalar_type = Solver::scalar_type
        + search()
        + opt_param() : scalar_type
    }
    param_searcher_base o-left- regularized_solver_base

    class l_curve_point<Scalar> {
        + using scalar_type = Scalar
        + param() : scalar_type
        + residual_norm() : scalar_type
        + regularization_norm() : scalar_type
        + curvature() : scalar_type
    }

    class explicit_l_curve<Solver, Optimizer> {
        + using optimizer_type = Optimizer
        + generate_table(param_list: const vector<scalar>&) : vector<l_curve_point>
        + generate_table() : vector<l_curve_point>
        + curvature(param: scalar_type) : scalar_type
    }
    param_searcher_base <|-- explicit_l_curve
    explicit_l_curve ..> l_curve_point

    class explicit_gcv<Solver, Optimizer> {
        + using optimizer_type = Optimizer
        + evaluate(param: scalar_type) : scalar_type
    }
    param_searcher_base <|-- explicit_gcv
