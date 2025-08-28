/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief Definition of gaussian_process_interpolator class.
 */
#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/concepts/distance_function.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/impl/get_default_scalar_type.h"
#include "num_collect/rbf/impl/get_variable_type.h"
#include "num_collect/rbf/kernel_matrix_type.h"
#include "num_collect/rbf/rbf_interpolator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::rbf {

/*!
 * \brief Class to interpolate using Gaussian process.
 *
 * \tparam FunctionSignature Signature of the function to interpolate.
 * (Example: `double(double)`, `double(Eigen::Vector3d)`, ...)
 * \tparam RBF Type of the RBF.
 * \tparam KernelMatrixType Type of kernel matrices.
 * \tparam DistanceFunction Type of the distance function.
 *
 * \note This class adds some functions for Gaussian process to
 * num_collect::rbf::rbf_interpolator class.
 */
template <typename FunctionSignature,
    concepts::rbf RBF =
        rbfs::gaussian_rbf<impl::get_default_scalar_type<FunctionSignature>>,
    kernel_matrix_type KernelMatrixType = kernel_matrix_type::dense,
    concepts::distance_function DistanceFunction =
        distance_functions::euclidean_distance_function<
            impl::get_variable_type_t<FunctionSignature>>>
class gaussian_process_interpolator
    : public global_rbf_interpolator<FunctionSignature, RBF, KernelMatrixType,
          DistanceFunction> {
public:
    static_assert(KernelMatrixType == kernel_matrix_type::dense,
        "Current implementation does not support sparse kernel matrices.");

    //! Type of the base class.
    using base_type = global_rbf_interpolator<FunctionSignature, RBF,
        KernelMatrixType, DistanceFunction>;

    using base_type::coeffs;
    using base_type::distance_function;
    using base_type::length_parameter_calculator;
    using base_type::rbf;
    using typename base_type::function_value_type;
    using typename base_type::function_value_vector_type;
    using typename base_type::kernel_value_type;
    using typename base_type::variable_type;

    /*!
     * \brief Compute parameters for interpolation.
     *
     * \param[in] variables Variables.
     * \param[in] function_values Function values.
     *
     * \note Pointer to the variables are saved internally,
     * so do not destruct it.
     */
    void compute(util::vector_view<const variable_type> variables,
        const function_value_vector_type& function_values) {
        base_type::compute(variables, function_values);
        common_coeff_ = function_values.dot(coeffs()) /
            static_cast<function_value_type>(function_values.size());
    }

    /*!
     * \brief Evaluate mean and variance in the gaussian process for a variable.
     *
     * \param[in] variable Variable on which the function value is interpolated.
     * \return Mean and variance.
     */
    [[nodiscard]] auto evaluate_mean_and_variance_on(
        const variable_type& variable) const
        -> std::pair<function_value_type, function_value_type> {
        Eigen::VectorXd kernel_vec;
        kernel_vec.resize(static_cast<index_type>(variables().size()));
        for (index_type i = 0; i < variables().size(); ++i) {
            kernel_vec(static_cast<index_type>(i)) =
                rbf()(distance_function()(variable, variables()[i]) /
                    length_parameter_calculator().length_parameter_at(
                        static_cast<index_type>(i)));
        }

        const function_value_type mean = kernel_vec.dot(coeffs());
        const function_value_type center_rbf_value =
            rbf()(static_cast<kernel_value_type>(0));
        const function_value_type variance = common_coeff_ *
            std::max<function_value_type>(center_rbf_value -
                    kernel_matrix_solver().calc_reg_term(kernel_vec, reg_param),
                static_cast<function_value_type>(0));
        return {mean, variance};
    }

private:
    using base_type::kernel_matrix_solver;
    using base_type::variables;

    //! Regularization parameter.
    static constexpr auto reg_param = static_cast<kernel_value_type>(0);

    // TODO Implementation of regularization.

    //! Common coefficients for RBF.
    function_value_type common_coeff_{};
};

}  // namespace num_collect::rbf
