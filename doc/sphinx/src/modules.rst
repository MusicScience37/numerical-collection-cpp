Modules
===============

.. uml::

    title Module Relation

    component fmt
    component Eigen

    component num_collect {
        component util
        util ..> fmt
        util ..> Eigen

        component auto_diff
        auto_diff ..> util
        auto_diff ..> Eigen

        component opt
        opt ..> util
        opt ..> Eigen

        component regularization
        regularization ..> opt

        component interp
        interp ..> util

        component multi_double

        component constants

        component roots
        roots ..> util
        roots ..> Eigen

        component functions
        functions ..> util
        functions ..> constants
        functions ..> roots

        component integration
        integration ..> util
        integration ..> constants
        integration ..> functions

        component ode
        ode ..> util
        ode ..> constants
        ode ..> roots
        ode ..> Eigen
    }

Names in this diagram are the names of namespaces,
except for ``util`` which has no namespace
and implemented in ``num_collect`` namespace.

util
    Utility common in this project.

auto_diff
    Automatic differentiation.

opt
    Optimization algorithms.

regularization
    Regularization algorithms.

interp
    Interpolation algorithms.

multi_double
    Multiple precision numbers with double numbers.

constants
    ``constexpr`` variables and functions.

roots
    Root-finding algorithms.

functions
    Special functions.

integration
    Numerical integration.

ode
    Solvers of ordinary differential equations (ODE).
