Ordinary Differential Equation (ODE) Solver
===============================================

Runge-Kutta Method
---------------------------

Here solves the following explicit initial-value problem:

.. math::

    \begin{cases}
        \dot{\bm{y}} = \bm{f}(t, \bm{y}) \\
        \bm{y}(0) = \bm{y}_0
    \end{cases}

.. uml::

    title Abstract Model of Runge-Kutta Method

    package runge_kutta {
        struct evaluation_type {
            + diff_coeff: bool
            + jacobian: bool
            + mass: bool
        }
    }

    package problems {
        class problem {
            + using variable_type = xxx
            + using scalar_type = xxx
            {static} + allowed_evaluations: evaluation_type
            + evaluate_on(time: scalar_type, variable: variable_type,\n\tevaluations: evaluation_type)
            + diff_coeff() : variable_type
        }
        problem ..> evaluation_type
    }

    package runge_kutta {
        class formula<Problem> {
            + using problem_type = Problem
            {static} + stages : index_type
            {static} + order : index_type
            + step(time: scalar_type, step_size: scalar_type,\n\tcurrent: const variable_type&, estimate: variable_type&)
            + problem() : problem_type&
        }
        formula o-- problem

        class embedded_formula<Problem> {
            {static} + lesser_order : index_type
            + step_embedded(time: scalar_type, step_size: scalar_type,\n\tcurrent: const variable_type&, estimate: variable_type&, error: variable_type&)
        }
        formula <|-- embedded_formula

        note as lesser_order_note
            lesser_order can be omitted if its information can't be found.
        end note
        lesser_order_note .up. embedded_formula

        class solver<Formula> {
            + init(time: scalar_type, variable: variable_type)
            + step()
            + solve_till(end_time: scalar_type)
            + time() : scalar_type
            + variable() : const variable_type&
            + step_size() : scalar_type
            + steps() : index_type
            + step_size(val : scalar_type)
        }
        solver o-- formula

        class step_size_limits<Scalar> {
            + upper_limit() : scalar_type
            + lower_limit() : scalar_type
            + upper_limit(val: scalar_type)
            + lower_limit(val: scalar_type)
            + limit(val: scalar_type) : scalar_type
        }

        class error_tolerances<Variable> {
            + check(var: variable_type, err: variable_type) : bool
            + calc_norm_of(var: variable_type, err: variable_type) : scalar_type
            + tol_rel_error(val: variable_type)
            + tol_abs_error(val: variable_type)
        }

        class step_size_controller<Formula> {
            + check_and_calc_next(step_size: scalar_type&, var: variable_type, err: variable_type) : bool
            + limits() : step_size_limits
            + tolerances() : error_tolerances
            + step_size_reduction_rate(val: scalar_type)
        }
        step_size_controller o-- step_size_limits
        step_size_controller o-- error_tolerances

        class embedded_solver<Formula> {
        }
        solver <|-- embedded_solver
        embedded_solver o-- embedded_formula
        embedded_solver o-- step_size_controller
    }

.. uml::

    title Abstract Model of Explicit Runge-Kutta Method

    package runge_kutta {
        class explicit_formula<Problem> {
            + using problem_type = Problem
            {static} + stages : index_type
            {static} + order : index_type
            + step(time: scalar_type, step_size: scalar_type,\n\tcurrent: const variable_type&, estimate: variable_type&)
            + problem() : problem_type&
        }
        explicit_formula o-- problem

        class explicit_embedded_formula<Problem> {
            {static} + lesser_order : index_type
            + step_embedded(time: scalar_type, step_size: scalar_type,\n\tcurrent: const variable_type&, estimate: variable_type&, error: variable_type&)
        }
        explicit_formula <|- explicit_embedded_formula
    }

.. uml::

    title Abstract Model of Implicit Runge-Kutta Method

    package problems {
        class differentiable_problem {
            + using variable_type = xxx
            + using scalar_type = xxx
            + using jacobian_type = xxx
            {static} + allowed_evaluations: evaluation_type
            + evaluate_on(time: scalar_type, variable: variable_type,\n\tevaluations: evaluation_type)
            + diff_coeff() : variable_type
            + jacobian() : jacobian_type
        }
    }

    package runge_kutta {
        package implicit_formula_solver_strategy {
            class newton_raphson_tag
            class modified_newton_raphson_tag
        }

        class semi_implicit_formula_solver<Problem, StrategyTag> {
            + solve(time: scalar_type, step_size: scalar_type,\n\tvariable: const variable_type&, k_coeff: scalar_type)
            + problem() : problem_type&
            + k() : const variable_type&
            + tol_residual_norm(val: scalar_type)
        }
        semi_implicit_formula_solver o-- differentiable_problem
        semi_implicit_formula_solver ..> implicit_formula_solver_strategy

        class implicit_formula_solver_coeffs {
            {static} + stages: index_type
            {static} + a: array<array<scalar_type, stages>, stages>
            {static} + b: array<scalar_type, stages>
        }

        class full_implicit_formula_solver<Problem, Coeffs, StrategyTag> {
            + solve(time: scalar_type, step_size: scalar_type,\n\tvariable: const variable_type&)
            + problem() : problem_type&
            + k(index: index_type) : const variable_type&
            + tol_residual_norm(val: scalar_type)
        }
        full_implicit_formula_solver o-- differentiable_problem
        full_implicit_formula_solver ..> implicit_formula_solver_coeffs
        full_implicit_formula_solver ..> implicit_formula_solver_strategy

        class implicit_formula<Problem, StrategyTag> {
            + using problem_type = Problem
            {static} + stages : index_type
            {static} + order : index_type
            + step(time: scalar_type, step_size: scalar_type,\n\tcurrent: const variable_type&, estimate: variable_type&)
            + problem() : problem_type&
            + tol_rel_residual_norm(val: scalar_type)
            + tol_abs_residual_norm(val: scalar_type)
        }
        implicit_formula o-- semi_implicit_formula_solver
        implicit_formula o-- full_implicit_formula_solver

        class implicit_embedded_formula<Problem, StrategyTag> {
            {static} + lesser_order : index_type
            + step_embedded(time: scalar_type, step_size: scalar_type,\n\tcurrent: const variable_type&, estimate: variable_type&, error: variable_type&)
        }
        implicit_formula <|- implicit_embedded_formula

        note as implicit_solver_note
            Solvers using formulas are common with explicit Runge-Kutta method.
        end note
        implicit_solver_note .. implicit_formula
        implicit_solver_note .. implicit_embedded_formula

        note as implicit_formula_solver_selection_note
            Actually one of these are used in implicit_formula
            depending on formulas.
        end note
        implicit_formula_solver_selection_note .. semi_implicit_formula_solver
        implicit_formula_solver_selection_note .. full_implicit_formula_solver
        implicit_formula_solver_selection_note .up. implicit_formula
    }

.. uml::

    title Abstract Model of Problems with Mass

    package problems {
        class mass_problem {
            + using variable_type = xxx
            + using scalar_type = xxx
            + using mass_matrix_type = xxx
            {static} + allowed_evaluations: evaluation_type
            + evaluate_on(time: scalar_type, variable: variable_type,\n\tevaluations: evaluation_type)
            + diff_coeff() : variable_type
            + mass() : mass_matrix_type
        }
    }
