/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of explicit_regularized_solver concept.
 */
#pragma once

#include <type_traits>
#include <utility>

#include "num_collect/base/concepts/decayed_to.h"
#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"

namespace num_collect::regularization::concepts {

/*!
 * \brief Concept of solvers using explicit formulas for regularization.
 *
 * \tparam T Type.
 */
template <typename T>
concept explicit_regularized_solver = requires() {
    typename T::scalar_type;
    requires base::concepts::real_scalar<typename T::scalar_type>;

    typename T::data_type;
    requires base::concepts::dense_matrix<typename T::data_type>;

    requires std::is_same_v<typename T::scalar_type,
        typename Eigen::NumTraits<typename T::data_type::Scalar>::Real>;

    requires requires(T& solver, const typename T::scalar_type& param,
        typename T::data_type& solution) { solver.solve(param, solution); };

    requires requires(T& solver, const typename T::scalar_type& param) {
        {
            solver.residual_norm(param)
        } -> base::concepts::decayed_to<typename T::scalar_type>;
        {
            solver.regularization_term(param)
        } -> base::concepts::decayed_to<typename T::scalar_type>;

        {
            solver.first_derivative_of_residual_norm(param)
        } -> base::concepts::decayed_to<typename T::scalar_type>;
        {
            solver.first_derivative_of_regularization_term(param)
        } -> base::concepts::decayed_to<typename T::scalar_type>;

        {
            solver.second_derivative_of_residual_norm(param)
        } -> base::concepts::decayed_to<typename T::scalar_type>;
        {
            solver.second_derivative_of_regularization_term(param)
        } -> base::concepts::decayed_to<typename T::scalar_type>;

        {
            solver.sum_of_filter_factor(param)
        } -> base::concepts::decayed_to<typename T::scalar_type>;

        { solver.data_size() } -> base::concepts::decayed_to<index_type>;

        {
            solver.param_search_region()
        } -> base::concepts::decayed_to<
            std::pair<typename T::scalar_type, typename T::scalar_type>>;

        {
            solver.l_curve_curvature(param)
        } -> base::concepts::decayed_to<typename T::scalar_type>;
        {
            solver.gcv(param)
        } -> base::concepts::decayed_to<typename T::scalar_type>;
    };
};

}  // namespace num_collect::regularization::concepts
