/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of rbf_fd_operator_with concept.
 */
#pragma once

#include "num_collect/base/get_compile_time_size.h"
#include "num_collect/rbf/concepts/operator_with.h"
#include "num_collect/rbf/polynomial_term.h"

namespace num_collect::rbf::concepts {

/*!
 * \brief Concept of operators usable with RBF-FD method.
 *
 * \tparam T Type of the operator.
 * \tparam RBF Type of the RBF.
 * \tparam DistanceFunction Type of the distance function.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 */
template <typename T, typename RBF, typename DistanceFunction,
    typename LengthParameterCalculator>
concept rbf_fd_operator_with =
    operator_with<T, RBF, DistanceFunction, LengthParameterCalculator,
        Eigen::VectorX<typename RBF::scalar_type>> &&
    requires(const DistanceFunction& distance_function, const RBF& rbf,
        const typename RBF::scalar_type& length_parameter,
        const T& target_operator,
        const typename DistanceFunction::variable_type& sample_variable,
        const typename RBF::scalar_type& kernel_coefficient) {
        {
            operators::operator_evaluator<T, RBF,
                DistanceFunction>::evaluate_for_one_sample(distance_function,
                rbf, length_parameter, target_operator, sample_variable,
                kernel_coefficient)
        };
    } &&
    requires(const T& target_operator,
        const polynomial_term<
            get_compile_time_size<typename DistanceFunction::variable_type>()>&
            term) {
        {
            operators::operator_evaluator<T, RBF,
                DistanceFunction>::evaluate_polynomial_term(target_operator,
                term)
        };
    };

}  // namespace num_collect::rbf::concepts
