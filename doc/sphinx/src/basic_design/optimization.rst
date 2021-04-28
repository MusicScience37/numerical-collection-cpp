Optimization
=================

.. note::
    Optimization can be minimization or maximization,
    but minimization will be implemented all in this library
    to unify usage of classes for all algorithms.

Unconstrained Optimization
-----------------------------------

.. uml::

    title Abstract Model of Unconstrained Optimization

    package problems {
        class objective_function {
            + using variable_type = xxx
            + using value_type = xxx
            + evaluate_on(variable: variable_type)
            + value() : value_type
        }

        class differentiable_objective_function {
            + gradient() : variable_type
        }
        objective_function <|-- differentiable_objective_function

        class twice_differentiable_objective_function {
            + using hessian_type = xxx
            + hessian() : hessian_type
        }
        differentiable_objective_function <|-- twice_differentiable_objective_function
    }

    package algorithms {
        class optimizer {
            + init(algorithm_dependent_arguments ...)
            + iterate()
            + solve()
            + opt_variable() : variable_type
            + opt_value() : value_type
            + iterations() : index_type
            + evaluations() : index_type
        }
        optimizer o-- objective_function

        class line_searcher {
            + using objective_function_type = xxx
            + init(init_variable: variable_type)
            + search(direction: variable_type)
            + obj_fun() : differentiable_objective_function&
            + opt_variable() : variable_type
            + opt_value() : value_type
            + gradient() : variable_type
            + evaluations() : index_type
        }
        line_searcher o-- differentiable_objective_function

        class descent_method_optimizer {
            + gradient() : variable_type
        }
        optimizer <|-- descent_method_optimizer
        descent_method_optimizer o-- line_searcher
    }

.. note::
    ``twice_differentiable_objective_function`` in the model
    will be used in Newton method,
    which is a ``descent_method_optimizer`` but requires Hessian.

.. note::
    In actual implementations,
    template classes and Curiously Recurring Template Pattern (CRTP)
    will be applied to classes
    for preventing overheads of virtual functions.

.. code-block:: cpp
    :caption: Usage

    auto obj_fun = objective_function();
    // create an optimizer with an objective function
    auto optimizer = num_collect::opt::steepest_descent<objective_function>(obj_fun);
    // set constants in algorithms if necessary
    optimizer.line_searcher().armijo_coeff(0.3);
    // initialize with the initial variable
    optimizer.init(initial_variable);
    // call iterate() multiple times or call solve() to minimize the objective function
    if (use_iterate) {
        for (std::size_t i = 0; i < max_iteration; ++i) {
            optimizer.iterate()
        }
    }
    else {
        optimizer.solve();
    }
    // check results
    std::cout << optimizer.opt_variable() << std::endl;
    std::cout << optimizer.opt_value() << std::endl;
