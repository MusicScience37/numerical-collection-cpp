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
 * \brief Definition of rosenbrock_equation_solver concept.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/concepts/const_reference_of.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/problem.h"              // IWYU pragma: keep

namespace num_collect::ode::concepts {

template <typename T>
concept rosenbrock_equation_solver = requires() {
    typename T::problem_type;
    requires problem<typename T::problem_type>;

    typename T::variable_type;
    requires std::is_same_v<typename T::variable_type,
        typename T::problem_type::variable_type>;

    typename T::scalar_type;
    requires std::is_same_v<typename T::scalar_type,
        typename T::problem_type::scalar_type>;

    typename T::jacobian_type;
    requires std::is_same_v<typename T::jacobian_type,
        typename T::problem_type::jacobian_type>;

    requires requires(const typename T::scalar_type& inverted_jacobian_coeff) {
        T(inverted_jacobian_coeff);
    };

    requires requires(T & obj, typename T::problem_type & problem,
        const typename T::scalar_type& time,
        const typename T::scalar_type& step_size,
        const typename T::variable_type& variable) {
        obj.evaluate_and_update_jacobian(problem, time, step_size, variable);
    };

    requires requires(const T& obj) {
        {
            obj.jacobian()
            } -> base::concepts::const_reference_of<typename T::jacobian_type>;
    };

    requires requires(T & obj, const typename T::variable_type& rhs,
        typename T::variable_type& result) {
        obj.solve(rhs, result);
    };
};

}  // namespace num_collect::ode::concepts
