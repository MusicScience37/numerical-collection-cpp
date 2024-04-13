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
 * \brief Definition of kernel_matrix_solver concept.
 */
#pragma once

#include <type_traits>

#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/same_as.h"      // IWYU pragma: keep

namespace num_collect::rbf::concepts {

/*!
 * \brief Concept of solvers of linear equations of kernel matrices.
 *
 * \tparam T Type.
 */
template <typename T>
concept kernel_matrix_solver = requires() {
    typename T::kernel_matrix_type;
    typename T::vector_type;
    typename T::scalar_type;

    requires std::is_same_v<typename T::scalar_type,
        typename T::kernel_matrix_type::Scalar>;
    requires base::concepts::real_scalar<typename T::scalar_type>;

    T();

    requires requires(T& obj,
        const typename T::kernel_matrix_type& kernel_matrix,
        const typename T::vector_type& data) {
        obj.compute(kernel_matrix, data);
    };

    requires requires(const T& obj, typename T::vector_type& coefficients,
        const typename T::scalar_type& reg_param) {
        obj.solve(coefficients, reg_param);
    };

    requires requires(const T& obj, const typename T::scalar_type& reg_param) {
        {
            obj.calc_mle_objective(reg_param)
        } -> base::concepts::same_as<typename T::scalar_type>;
    };

    requires requires(const T& obj, const typename T::scalar_type& reg_param) {
        {
            obj.calc_common_coeff(reg_param)
        } -> base::concepts::same_as<typename T::scalar_type>;
    };

    requires requires(const T& obj, const typename T::vector_type& data,
        const typename T::scalar_type& reg_param) {
        {
            obj.calc_reg_term(data, reg_param)
        } -> base::concepts::same_as<typename T::scalar_type>;
    };

    requires requires(const T& obj) { obj.eigenvalues(); };
};

}  // namespace num_collect::rbf::concepts
