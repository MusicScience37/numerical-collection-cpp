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
 * \brief Definition of rbf_fd_operator_factory concept.
 */
#pragma once

#include "num_collect/rbf/concepts/rbf_fd_operator_with.h"

namespace num_collect::rbf::concepts {

/*!
 * \brief Concept of factory function objects to create operators for RBF-FD
 * method.
 *
 * \tparam T Type of the factory function object.
 * \tparam RBF Type of the RBF.
 * \tparam DistanceFunction Type of the distance function.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 */
template <typename T, typename RBF, typename DistanceFunction,
    typename LengthParameterCalculator>
concept rbf_fd_operator_factory = requires(T& factory,
    const typename DistanceFunction::variable_type& sample_variable) {
    {
        factory(sample_variable)
    } -> rbf_fd_operator_with<RBF, DistanceFunction, LengthParameterCalculator>;
};

}  // namespace num_collect::rbf::concepts
