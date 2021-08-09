Automatic Differentiation
===============================

Backward-Mode Automatic Differentiation
--------------------------------------------------

.. uml::

    title Abstract Model of Backward-Mode Automatic Differentiation

    package graph {
        class child_node<Scalar> {
            + using scalar_type = Scalar
            + node() : shared_ptr<node>
            + sensitivity() : scalar_type
        }

        class node<Scalar> {
            + using scalar_type = Scalar
            + value() : scalar_type
            + children() : vector<child_node>
        }
        node o-- child_node

        class node_differentiator<Scalar> {
            + using scalar_type = Scalar
            + compute(top_node: shared_ptr<node>)
            + coeff(node: shared_ptr<node>) : scalar_type
        }
        node_differentiator o-- node
    }

    class variable<Scalar> {
        + using scalar_type = Scalar
        + node() : shared_ptr<node>
    }
    variable o-- node

    note as variable_note
        This class provides
        many arithmetic operators.
    endnote
    variable .. variable_note

    class "(Global Functions)" as global_func_diff {
        + differentiate(func_value: FuncValue, args: Args) : Result
    }
    global_func_diff ..> variable
    global_func_diff ..> node_differentiator

    note as differentiate_note
        FuncValue and Args types can be
        * variable class,
        * Eigen::Matrix<variable, ...> class.
        Result type varies according to
        FuncValue and Args types.
    endnote
    global_func_diff . differentiate_note
