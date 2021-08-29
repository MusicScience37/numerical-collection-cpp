Regularization
==================

Regularization Using L2 Norm
--------------------------------------

.. uml::

    title Abstract Model of Regularization Using L2 Norm

    interface explicit_regularized_solver_base<Data> {
        + using data_type = Data
        + using scalar_type = data_type::Scalar
        + solve(param: scalar_type, solution: data_type&)
        + residual_norm(param: scalar_type) : scalar_type
        + regularization_term(param: scalar_type) : scalar_type
        + first_derivative_of_residual_norm(param: scalar_type) : scalar_type
        + first_derivative_of_regularization_term(param: scalar_type) : scalar_type
        + second_derivative_of_residual_norm(param: scalar_type) : scalar_type
        + second_derivative_of_regularization_term(param: scalar_type) : scalar_type
        + sum_of_filter_factor(param: scalar_type) : scalar_type
        + data_size() : index_type
        + param_search_region() : pair<scalar_type, scalar_type>
        + l_curve_curvature(param: scalar_type) : scalar_type
        + gcv(param: scalar_type) : scalar_type
    }

    class tikhonov<Coeff, Data> {
        + using coeff_type = Coeff
        + compute(coeff: const coeff_type&, data: const data_type&)
    }
    explicit_regularized_solver_base <|-- tikhonov

    class full_gen_tikhonov<Coeff, Data> {
        + using coeff_type = Coeff
        + compute(coeff: const coeff_type&, data: const data_type&, reg_coeff: const coeff_type&)
    }
    explicit_regularized_solver_base <|-- full_gen_tikhonov
    full_gen_tikhonov o-left- tikhonov

    interface explicit_param_searcher_base<Solver> {
        + using solver_type = Solver
        + using scalar_type = Solver::scalar_type
        + using data_type = Solver::data_type
        + search()
        + opt_param() : scalar_type
        + solve(solution: data_type&)
    }
    explicit_param_searcher_base o-left- explicit_regularized_solver_base
