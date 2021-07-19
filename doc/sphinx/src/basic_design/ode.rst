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

    title Abstract Model of Explicit Runge-Kutta Method

    package problems {
        class problem {
            + using variable_type = xxx
            + using scalar_type = xxx
            + evaluate_on(time: scalar_type, variable: variable_type)
            + diff_coeff() : variable_type
        }
    }

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
        solver o-- explicit_formula

        class embedded_solver<Formula> {
            + tol_rel_error(val: scalar_type)
            + tol_abs_error(val: scalar_type)
            + step_size_reduction_rate(val: scalar_type)
            + max_step_size(val: scalar_type)
        }
        solver <|- embedded_solver
        embedded_solver o-- explicit_embedded_formula
    }

.. uml::

    title Abstract Model of Implicit Runge-Kutta Method

    package problems {
        class differentiable_problem {
            + using variable_type = xxx
            + using scalar_type = xxx
            + using jacobian_type = xxx
            + evaluate_on(time: scalar_type, variable: variable_type,\n\tupdate_jacobian: bool = false)
            + diff_coeff() : variable_type
            + jacobian() : variable_type
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
            + tol_residual_norm(val: scalar_type)
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
