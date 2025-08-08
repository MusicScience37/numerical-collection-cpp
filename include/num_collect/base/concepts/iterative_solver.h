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
 * \brief Definition of iterative_solver concept.
 */
#pragma once

#include "num_collect/base/concepts/decayed_to.h"
#include "num_collect/base/concepts/reference_of.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/logger.h"

namespace num_collect::inline base::concepts {

/*!
 * \brief Concept of iterative solvers.
 *
 * \tparam T Type.
 */
template <typename T>
concept iterative_solver = requires(T& solver, const T& const_solver) {
    { solver.iterate() };

    { const_solver.is_stop_criteria_satisfied() } -> decayed_to<bool>;

    { solver.solve() };

    { solver.logger() } -> reference_of<logging::logger>;
    { const_solver.logger() } -> reference_of<const logging::logger>;

    requires requires(
        logging::iterations::iteration_logger<T>& iteration_logger) {
        { solver.configure_iteration_logger(iteration_logger) };
    };
};

}  // namespace num_collect::inline base::concepts
