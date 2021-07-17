Root-Finding Algorithms
===========================

Equations with Non-Linear Real-Valued Function
-----------------------------------------------------

Algorithms for equations with non-linear real-valued function
:math:`f(\bm{x}) = \bm{0}`.

.. uml::

    title Abstract Model of Non-Linear Equation

    package problems {
        class function {
            + using variable_type = xxx
            + evaluate_on(variable: variable_type)
            + value() : variable_type
        }

        class differentiable_function {
            + using jacobian_type = xxx
            + jacobian() : jacobian_type
        }
        function <|-- differentiable_function
    }

    package algorithms {
        class function_root_finder {
            + init(algorithm_dependent_arguments ...)
            + iterate()
            + solve()
            + variable() : variable_type
            + value() : variable_type
            + iterations() : index_type
            + evaluations() : index_type
        }
        function_root_finder o-- function

        class differentiable_function_root_finder {
            + jacobian() : jacobian_type
        }
        function_root_finder <|-- differentiable_function_root_finder
        differentiable_function_root_finder o-- differentiable_function
    }
