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
 * \brief Definition of implicit_regularized_solver concept.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/concepts/decayed_to.h"
#include "num_collect/regularization/concepts/regularized_solver.h"

namespace num_collect::regularization::concepts {

/*!
 * \brief Concept of solvers using implicit formulas for regularization.
 *
 * \tparam T Type.
 *
 * \note For APIs of functions, see
 * \ref num_collect::regularization::implicit_regularized_solver_base class.
 */
template <typename T>
concept implicit_regularized_solver = regularized_solver<T> && requires() {
    requires requires(const T& solver, const typename T::data_type& solution) {
        {
            solver.residual_norm(solution)
        } -> base::concepts::decayed_to<typename T::scalar_type>;
    };

    requires requires(const T& solver, const typename T::data_type& solution) {
        {
            solver.regularization_term(solution)
        } -> base::concepts::decayed_to<typename T::scalar_type>;
    };

    requires requires(T& solver, const typename T::data_type& data) {
        solver.change_data(data);
    };
};

}  // namespace num_collect::regularization::concepts
