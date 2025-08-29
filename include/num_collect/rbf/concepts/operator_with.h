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
 * \brief Definition of operator_with concept.
 */
#pragma once

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/rbf/concepts/distance_function.h"
#include "num_collect/rbf/concepts/length_parameter_calculator.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::rbf::concepts {

/*!
 * \brief Concept of operators usable with the given types.
 *
 * \tparam T Type.
 * \tparam RBF Type of the RBF.
 * \tparam DistanceFunction Type of the distance function.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 * \tparam KernelCoeffVector Type of the vector of coefficients of kernels.
 */
template <typename T, typename RBF, typename DistanceFunction,
    typename LengthParameterCalculator, typename KernelCoeffVector>
concept operator_with =
    concepts::rbf<RBF> && concepts::distance_function<DistanceFunction> &&
    concepts::length_parameter_calculator<LengthParameterCalculator> &&
    base::concepts::dense_vector<KernelCoeffVector> &&
    requires(const DistanceFunction& distance_function, const RBF& rbf,
        const LengthParameterCalculator& length_parameter_calculator,
        const T& target_operator,
        util::vector_view<const typename DistanceFunction::variable_type>
            sample_variables,
        const KernelCoeffVector& kernel_coefficients) {
        {
            operators::operator_evaluator<T, RBF, DistanceFunction>::evaluate(
                distance_function, rbf, length_parameter_calculator,
                target_operator, sample_variables, kernel_coefficients)
        };
    };

}  // namespace num_collect::rbf::concepts
