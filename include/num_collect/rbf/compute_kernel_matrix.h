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
 * \brief Definition of compute_kernel_matrix function.
 */
#pragma once

#include <type_traits>
#include <vector>

#include "num_collect/base/concepts/dense_matrix.h"  // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "num_collect/constants/zero.h"
#include "num_collect/rbf/concepts/distance_function.h"  // IWYU pragma: keep
#include "num_collect/rbf/concepts/length_parameter_calculator.h"  // IWYU pragma: keep
#include "num_collect/rbf/concepts/rbf.h"  // IWYU pragma: keep
#include "num_collect/util/assert.h"

namespace num_collect::rbf {

/*!
 * \brief Compute a kernel matrix.
 *
 * \tparam DistanceFunction Type of the distance function.
 * \tparam RBF Type of the RBF.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 * \tparam KernelMatrix Type of the kernel matrix.
 * \param[in] distance_function Distance function.
 * \param[in] rbf RBF.
 * \param[in,out] length_parameter_calculator Calculator of length parameters.
 * \param[in] variables Variables.
 * \param[out] kernel_matrix Kernel matrix.
 */
template <concepts::distance_function DistanceFunction, concepts::rbf RBF,
    concepts::length_parameter_calculator LengthParameterCalculator,
    base::concepts::dense_matrix KernelMatrix>
    requires std::is_same_v<
                 typename LengthParameterCalculator::distance_function_type,
                 DistanceFunction> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename RBF::scalar_type> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename KernelMatrix::Scalar> &&
    LengthParameterCalculator::uses_global_length_parameter
inline void compute_kernel_matrix(const DistanceFunction& distance_function,
    const RBF& rbf, LengthParameterCalculator& length_parameter_calculator,
    const std::vector<typename DistanceFunction::variable_type>& variables,
    KernelMatrix& kernel_matrix) {
    using scalar_type = typename KernelMatrix::Scalar;

    length_parameter_calculator.compute(variables, distance_function);

    const scalar_type length_parameter =
        length_parameter_calculator.length_parameter_at(
            static_cast<index_type>(0));
    NUM_COLLECT_ASSERT(length_parameter > static_cast<scalar_type>(0));

    const scalar_type diagonal_coeff = rbf(constants::zero<scalar_type>);

    const std::size_t num_variables = variables.size();
    kernel_matrix.resize(static_cast<index_type>(num_variables),
        static_cast<index_type>(num_variables));

    for (std::size_t i = 0; i < num_variables; ++i) {
        kernel_matrix(static_cast<index_type>(i), static_cast<index_type>(i)) =
            diagonal_coeff;
        for (std::size_t j = i + 1; j < num_variables; ++j) {
            const scalar_type value =
                rbf(distance_function(variables[i], variables[j]) /
                    length_parameter);
            kernel_matrix(
                static_cast<index_type>(i), static_cast<index_type>(j)) = value;
            kernel_matrix(
                static_cast<index_type>(j), static_cast<index_type>(i)) = value;
        }
    }
}

}  // namespace num_collect::rbf