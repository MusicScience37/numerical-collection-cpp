/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of general_operator_evaluator struct.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/concepts/length_parameter_calculator.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::rbf::operators {

/*!
 * \brief Struct of common implementation for
 * num_collect::rbf::operators::operator_evaluator.
 *
 * \tparam Operator Type of the operator.
 * \tparam RBF Type of the RBF.
 * \tparam DistanceFunction Type of the distance function.
 *
 *
 */
template <typename Derived, typename Operator, concepts::rbf RBF,
    concepts::distance_function DistanceFunction>
struct general_operator_evaluator {
    //! Type of the RBF.
    using rbf_type = RBF;

    //! Type of the distance function.
    using distance_function_type = DistanceFunction;

    //! Type of variables.
    using variable_type = typename DistanceFunction::variable_type;

    //! Type of kernel values.
    using kernel_value_type = typename rbf_type::scalar_type;

    //! Type of the operator.
    using operator_type = Operator;

    /*!
     * \brief Evaluate an operator.
     *
     * \tparam LengthParameterCalculator Type of the calculator of length
     * parameters.
     * \tparam KernelCoeffVector Type of the vector of coefficients of kernels.
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     * \param[in] length_parameter_calculator Calculator of length parameters.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] sample_variables Variables of samples.
     * \param[in] kernel_coefficients Coefficients of kernels.
     * \return
     */
    template <concepts::length_parameter_calculator LengthParameterCalculator,
        base::concepts::dense_vector KernelCoeffVector>
    static auto evaluate(const distance_function_type& distance_function,
        const rbf_type& rbf,
        const LengthParameterCalculator& length_parameter_calculator,
        const operator_type& target_operator,
        util::vector_view<const variable_type> sample_variables,
        const KernelCoeffVector& kernel_coefficients) ->
        typename KernelCoeffVector::Scalar {
        auto value = static_cast<typename KernelCoeffVector::Scalar>(0);
        for (index_type i = 0; i < sample_variables.size(); ++i) {
            value += Derived::evaluate_for_one_sample(distance_function, rbf,
                length_parameter_calculator.length_parameter_at(i),
                target_operator, sample_variables[i], kernel_coefficients(i));
        }
        return value;
    }
};

}  // namespace num_collect::rbf::operators
