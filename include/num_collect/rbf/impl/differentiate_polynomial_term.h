/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of differentiate_polynomial_term function.
 */
#pragma once

#include <optional>
#include <utility>

#include <Eigen/Core>

#include "num_collect/rbf/polynomial_term.h"
#include "num_collect/util/assert.h"

namespace num_collect::rbf::impl {

/*!
 * \brief Differentiate a polynomial term.
 *
 * \tparam Scalar Type of scalars used for coefficients.
 * \tparam NumDimensions Number of dimensions of variables.
 * \param[in] term Polynomial term.
 * \param[in] orders Number of partial differentiations for each dimension of
 * the variable.
 * \return Pair of the differentiated term and its coefficient. Null if the
 * resulting term is zero.
 */
template <typename Scalar, int NumDimensions>
[[nodiscard]] auto differentiate_polynomial_term(
    const polynomial_term<NumDimensions>& term,
    const Eigen::Vector<int, NumDimensions>& orders)
    -> std::optional<std::pair<polynomial_term<NumDimensions>, Scalar>> {
    NUM_COLLECT_DEBUG_ASSERT((orders.array() >= 0).all());

    const Eigen::Vector<int, NumDimensions> resulting_degrees =
        term.degrees() - orders;
    if ((resulting_degrees.array() < 0).any()) {
        return std::nullopt;
    }

    auto coefficient = static_cast<Scalar>(1);
    for (int d = 0; d < NumDimensions; ++d) {
        for (int i = 0; i < orders(d); ++i) {
            coefficient *= static_cast<Scalar>(term.degrees()(d) - i);
        }
    }
    return std::make_pair(
        polynomial_term<NumDimensions>(resulting_degrees), coefficient);
}

}  // namespace num_collect::rbf::impl
