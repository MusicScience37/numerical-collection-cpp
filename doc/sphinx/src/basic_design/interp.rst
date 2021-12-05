Interpolation
=================

Kernel Interpolation
---------------------------

.. uml::

    title Abstract Model of Kernel Interpolation

    interface distance <<Concept>> {
        {abstract} + using variable_type = <type1>
        {abstract} + using value_type = <type2>
        {abstract} + operator()(var1: variable_type, var2: variable_type) : distance_type
    }

    class euclidean_distance<Variable>
    distance <|.. euclidean_distance

    interface rbf <<Concept>> {
        {abstract} + using arg_type = <type2>
        {abstract} + using value_type = <type3>
        {abstract} + operator()(distance_rate: arg_type) : value_type
    }

    class gaussian_rbf<Scalar>
    rbf <|.. gaussian_rbf

    note as type23_note
        <type2> and <type3> will be same.
    endnote
    rbf .. type23_note

    interface kernel <<Concept>> {
        {abstract} + using variable_type = <type1>
        {abstract} + using value_type = <type3>
        {abstract} + using kernel_param_type = <type4>
        {abstract} + operator()(var1: variable_type, var2: variable_type) : value_type
        {abstract} + kernel_param(value: kernel_param_type)
        {abstract} + kernel_param_search_region(variable_list: vector<variable_type>) : pair<kernel_param_type, kernel_param_type>
    }

    note as kernel_param_type_note
        kernel_param_type can be void,
        which means "no parameters".
    endnote
    kernel .. kernel_param_type_note

    class rbf_kernel<Distance, Rbf> {
        + using distance_type = Distance
        + using rbf_type = Rbf
        + distance() : distance_type
        + rbf() : rbf_type
    }
    kernel <|.. rbf_kernel
    rbf_kernel o-- distance
    rbf_kernel o-- rbf

    class kernel_interpolator<Kernel> {
        + using kernel_type = Kernel
        + using variable_type = kernel_type::variable_type
        + using value_type = kernel_type::value_type
        + using kernel_param_type = kernel_type::kernel_param_type
        + regularize_with(reg_param: value_type)
        + regularize_automatically()
        + disable_regularization()
        + fix_kernel_param(kernel_param: kernel_param_type)
        + search_kernel_param_auto()
        + compute(variable_list: vector<variable_type>, value_list: Eigen::VectorX<value_type>)
        + mle_objective_function_value() : value_type
        + interpolate_on(variable: variable_type) : value_type
        + operator()(variable: variable_type) : value_type
        + kernel() : kernel_type
    }
    kernel_interpolator o-- kernel
