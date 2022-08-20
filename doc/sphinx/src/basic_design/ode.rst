Ordinary Differential Equation (ODE) Solver
===============================================

Here solves the following explicit initial-value problem:

.. math::

    \begin{cases}
        \dot{\bm{y}} = \bm{f}(t, \bm{y}) \\
        \bm{y}(0) = \bm{y}_0
    \end{cases}

Initial-value problem with mass matrix will be supported too:

.. math::

    \begin{cases}
        M(\bm{y}) \dot{\bm{y}} = \bm{f}(t, \bm{y}) \\
        \bm{y}(0) = \bm{y}_0
    \end{cases}

.. uml:: uml/ode/formula_and_problems.puml

.. uml:: uml/ode/embedded_formulas.puml

.. uml:: uml/ode/runge-kutta/implicit_formulas.puml

.. uml:: uml/ode/rosenbrock_formula.puml
