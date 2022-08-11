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

#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/concepts/const_reference_of.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/iterative_solver.h"    // IWYU pragma: keep
#include "num_collect/ode/concepts/problem.h"              // IWYU pragma: keep
#include "num_collect/ode/error_tolerances.h"

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
concept slope_equation_solver = base::concepts::iterative_solver<T> &&
    requires() {
    typename T::problem_type;
    requires problem<typename T::problem_type>;

    typename T::variable_type;
    requires std::is_same_v<typename T::variable_type,
        typename T::problem_type::variable_type>;

    typename T::scalar_type;
    requires std::is_same_v<typename T::scalar_type,
        typename T::problem_type::scalar_type>;

    T();

    requires requires(T & obj, typename T::problem_type & problem,
        typename T::scalar_type time, typename T::scalar_type step_size,
        const typename T::variable_type& variable,
        typename T::scalar_type solution_coeff) {
        obj.update_jacobian(problem, time, step_size, variable, solution_coeff);
    };

    requires requires(T & obj, typename T::variable_type & solution) {
        obj.init(solution);
    };

    requires requires(
        T & obj, const error_tolerances<typename T::variable_type>& val) {
        obj.tolerances(val);
    };

    requires requires(const T& obj) {
        {
            obj.tolerances()
            } -> base::concepts::const_reference_of<
                error_tolerances<typename T::variable_type>>;
    };
};

}  // namespace num_collect::ode::concepts
