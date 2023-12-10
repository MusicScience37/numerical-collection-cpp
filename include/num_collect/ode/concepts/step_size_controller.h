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
 * \brief Definition of step_size_controller concept.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/concepts/const_reference_of.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/decayed_to.h"          // IWYU pragma: keep
#include "num_collect/ode/concepts/formula.h"              // IWYU pragma: keep
#include "num_collect/ode/concepts/problem.h"              // IWYU pragma: keep
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/step_size_limits.h"

namespace num_collect::ode::concepts {

/*!
 * \brief Concept of classes to control step sizes.
 *
 * \tparam T Type.
 */
template <typename T>
concept step_size_controller = requires() {
    typename T::formula_type;
    requires formula<typename T::formula_type>;

    typename T::problem_type;
    requires problem<typename T::problem_type>;
    requires std::is_same_v<typename T::formula_type::problem_type,
        typename T::problem_type>;

    typename T::variable_type;
    requires std::is_same_v<typename T::problem_type::variable_type,
        typename T::variable_type>;

    typename T::scalar_type;
    requires std::is_same_v<typename T::problem_type::scalar_type,
        typename T::scalar_type>;

    requires requires() { T(); };

    requires requires(T& obj) { obj.init(); };

    requires requires(T& obj, typename T::scalar_type& step_size,
        const typename T::variable_type& variable,
        const typename T::variable_type& error) {
        {
            obj.check_and_calc_next(step_size, variable, error)
        } -> base::concepts::decayed_to<bool>;
    };

    requires requires(
        T& obj, const step_size_limits<typename T::scalar_type>& limits) {
        obj.limits(limits);
    };

    requires requires(const T& obj) {
        {
            obj.limits()
        } -> base::concepts::const_reference_of<
            step_size_limits<typename T::scalar_type>>;
    };

    requires requires(
        T& obj, const error_tolerances<typename T::variable_type>& tolerances) {
        obj.tolerances(tolerances);
    };

    requires requires(const T& obj) {
        {
            obj.tolerances()
        } -> base::concepts::const_reference_of<
            error_tolerances<typename T::variable_type>>;
    };
};

}  // namespace num_collect::ode::concepts
