/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of slope_equation_solver concept.
 */
#pragma once

#include "num_collect/ode/concepts/ode_equation_solver.h"

namespace num_collect::ode::concepts {

/*!
 * \brief Concept of classes solve equations of implicit slopes.
 *
 * This type of classes solves following equation:
 * \f[
 *     \boldsymbol{k}_i = \boldsymbol{f}\left(t + b_i h, \boldsymbol{y}(t)
 *         + h \sum_{j = 1}^s a_{ij} \boldsymbol{k}_j \right)
 * \f]
 *
 * \tparam T Type.
 */
template <typename T>
concept slope_equation_solver =
    ode::concepts::ode_equation_solver<T> && requires() {
        requires requires(T& obj, typename T::problem_type& problem,
            typename T::scalar_type time, typename T::scalar_type step_size,
            const typename T::variable_type& variable,
            typename T::scalar_type solution_coeff) {
            obj.update_jacobian(
                problem, time, step_size, variable, solution_coeff);
        };

        requires requires(T& obj, typename T::variable_type& solution) {
            obj.init(solution);
        };
    };

}  // namespace num_collect::ode::concepts
