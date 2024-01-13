/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of solve_and_check function.
 */
#pragma once

#include <limits>
#include <string_view>

#include "num_collect/base/concepts/invocable_as.h"
#include "num_collect/base/index_type.h"
#include "ode_approvals.h"

template <typename Solver,
    num_collect::concepts::invocable_as<typename Solver::variable_type(
        typename Solver::scalar_type)>
        ReferenceFunction>
inline void solve_and_check_with_reference(Solver& solver,
    typename Solver::scalar_type init_time,
    typename Solver::scalar_type finish_time,
    num_collect::index_type num_time_samples,
    const ReferenceFunction& reference_function,
    std::string_view problem_condition = "",
    num_collect::index_type precision =
        (std::numeric_limits<typename Solver::scalar_type>::digits10 / 2)) {
    using scalar_type = typename Solver::scalar_type;
    using variable_type = typename Solver::variable_type;

    std::vector<scalar_type> time_list;
    std::vector<variable_type> actual_variable_list;
    std::vector<variable_type> reference_variable_list;
    time_list.reserve(num_time_samples + 1);
    actual_variable_list.reserve(num_time_samples + 1);
    reference_variable_list.reserve(num_time_samples + 1);

    {
        // initial solution.
        time_list.push_back(init_time);
        actual_variable_list.push_back(solver.variable());
        reference_variable_list.push_back(reference_function(init_time));
    }

    for (num_collect::index_type i = 0; i < num_time_samples; ++i) {
        const scalar_type time_rate = static_cast<scalar_type>(i + 1) /
            static_cast<scalar_type>(num_time_samples);
        const scalar_type time =
            time_rate * (finish_time - init_time) + init_time;
        REQUIRE_NOTHROW(solver.solve_till(time));
        time_list.push_back(time);
        actual_variable_list.push_back(solver.variable());
        reference_variable_list.push_back(reference_function(time));
    }

    ode_approvals::verify_with_reference<Solver>(time_list,
        actual_variable_list, reference_variable_list, problem_condition,
        precision);
}
