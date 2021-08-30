Interpolation
=================

RBF Interpolation
---------------------------

.. uml::

    title Abstract Model of RBF Interpolation

    interface distance {
        + using variable_type = <type1>
        + using value_type = <type2>
        + operator()(var1: variable_type, var2: variable_type) : distance_type
    }

    class euclidean_distance<Variable>
    distance <|.. euclidean_distance

    interface rbf {
        + using arg_type = <type2>
        + using value_type = <type3>
        + operator()(distance_rate: arg_type) : value_type
    }

    class gaussian_rbf<Scalar>
    rbf <|.. gaussian_rbf

    note as type23_note
        <type2> and <type3> will be same.
    endnote
    rbf .. type23_note

    class rbf_interpolator<Distance, Rbf> {
        + using distance_type = Distance
        + using rbf_type = Rbf
        + using variable_type = distance_type::variable_type
        + using distance_value_type = distance_type::value_type
        + using value_type = rbf::value_type
        + regularize_with(reg_param: value_type)
        + regularize_automatically()
        + disable_regularization()
        + fix_length_param(length_param: distance_value_type)
        + search_length_param_auto()
        + compute(variable_list: vector<variable_type>, value_list: vector<value_type>)
        + value(variable: variable_type) : value_type
        + operator()(variable: variable_type) : value_type
    }
    rbf_interpolator o-- distance
    rbf_interpolator o-- rbf
