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
        class explicit_problem {
            + using variable_type = xxx
            + using scalar_type = xxx
            + evaluate_on(time: scalar_type, variable: variable_type)
            + diff_coeff() : variable_type
        }
    }

    package runge_kutta {
        class explicit_formula<Problem> {
            + using problem_type = Problem
            + stages : index_type
            + step(time: scalar_type, step_size: scalar_type,\n\tcurrent: const variable_type&, estimate: variable_type&)
            + problem() : problem_type&
        }
        explicit_formula o-- explicit_problem

        class explicit_embedded_formula<Problem> {
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
        class implicit_problem {
            + using variable_type = xxx
            + using scalar_type = xxx
            + using jacobian_type = xxx
            + evaluate_on(time: scalar_type, variable: variable_type, diff_coeff: variable_type)
            + value() : variable_type
            + jacobian() : jacobian_type
        }
    }

    package roots {
        class function_root_finder {
            See Root-Finding Algorithms.
        }
        hide function_root_finder methods
    }

    package runge_kutta {
        class implicit_formula<Problem> {
            + using problem_type = Problem
            + stages : index_type
            + step(time: scalar_type, step_size: scalar_type,\n\tcurrent: const variable_type&, estimate: variable_type&)
            + problem() : problem_type&
            + root_finder() : function_root_finder&
        }
        implicit_formula o-- implicit_problem
        implicit_formula o-- function_root_finder

        class implicit_embedded_formula<Problem> {
            + step_embedded(time: scalar_type, step_size: scalar_type,\n\tcurrent: const variable_type&, estimate: variable_type&, error: variable_type&)
        }
        implicit_formula <|- implicit_embedded_formula

        note as implicit_solver_note
            Solvers are common with explicit Runge-Kutta method.
        end note
        implicit_solver_note .. implicit_formula
        implicit_solver_note .. implicit_embedded_formula
    }
