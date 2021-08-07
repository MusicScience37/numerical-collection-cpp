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

        component opt
        opt ..> util

        component multi_double

        component constants

        component roots
        roots ..> util

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
    }

Names in this diagram are the names of namespaces.
