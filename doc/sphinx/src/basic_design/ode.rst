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

    package problems {
        class problem {
            + using variable_type = xxx
            + using scalar_type = xxx
            + evaluate_on(time: scalar_type, variable: variable_type)
        }

        class explicit_problem {
            + diff_coeff() : variable_type
        }
        problem <|-- explicit_problem

        class implicit_problem {
            + using jacobian_type = xxx
            + value() : variable_type
            + jacobian() : jacobian_type
        }
        problem <|-- implicit_problem

        note as problem_type_note
            explicit Runge-Kutta methods uses explicit_problem,
            and implicit Runge-Kutta methods uses implicit_problem.
        end note
        explicit_problem .. problem_type_note
        implicit_problem .. problem_type_note
    }

    package runge_kutta {
        class formula<Problem> {
            + using problem_type = Problem
            + stages : index_type
            + step(current: const variable_type&, estimate: variable_type&)
            + problem() : problem_type&
        }
        formula o-- problem

        class embedded_formula<Problem> {
            + step(current: const variable_type&, estimate: variable_type&, weak_estimate: variable_type&)
        }
        formula <|-- embedded_formula
        embedded_formula o-- problem

        class solver<Problem, Formula> {
            + init(variable: variable_type)
            + step()
            + solve_till(end_time: scalar_type)
            + time() : scalar_type
            + variable() : const variable_type&
            + diff_coeff() : const variable_type&
            + step_size() : scalar_type
            + steps() : index_type
            + evaluations() : index_type
            + step_size(val : scalar_type)
        }
        solver o-- formula

        class embedded_solver<Problem, Formula> {
            + tol_rel_error(val: scalar_type)
            + tol_abs_error(val: scalar_type)
            + step_size_reduction_rate(val: scalar_type)
            + max_step_size(val: scalar_type)
        }
        solver <|-- embedded_solver
        embedded_solver o-- embedded_formula
    }
