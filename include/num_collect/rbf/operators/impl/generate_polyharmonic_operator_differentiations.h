/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of generate_polyharmonic_operator_differentiations
 * function.
 */
#pragma once

#include <array>

#include <Eigen/Core>

#include "num_collect/functions/pow.h"

namespace num_collect::rbf::operators::impl {

/*!
 * \brief Generate orders of differentiations for a polyharmonic operator.
 *
 * \tparam PolyharmonicOrder Order of the polyharmonic operator.
 * \tparam VariableDimensions Dimensions of variables.
 * \return Order of differentiations.
 */
template <int PolyharmonicOrder, int VariableDimensions>
    requires(PolyharmonicOrder > 0) && (VariableDimensions > 0)
[[nodiscard]] auto generate_polyharmonic_operator_differentiations()
    -> std::array<Eigen::Vector<int, VariableDimensions>,
        functions::pow(VariableDimensions, PolyharmonicOrder)> {
    constexpr auto num_operators =
        functions::pow(VariableDimensions, PolyharmonicOrder);
    std::array<Eigen::Vector<int, VariableDimensions>,
        static_cast<std::size_t>(num_operators)>
        orders_list;
    for (int i = 0; i < num_operators; ++i) {
        Eigen::Vector<int, VariableDimensions> orders =
            Eigen::Vector<int, VariableDimensions>::Zero();
        int index = i;
        for (int j = 0; j < PolyharmonicOrder; ++j) {
            int dimension_index = index % VariableDimensions;
            orders(dimension_index) += 2;
            index /= VariableDimensions;
        }
        orders_list[static_cast<std::size_t>(i)] = orders;
    }
    return orders_list;
}

}  // namespace num_collect::rbf::operators::impl
