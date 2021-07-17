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
    }

Names in this diagram are the names of namespaces.
