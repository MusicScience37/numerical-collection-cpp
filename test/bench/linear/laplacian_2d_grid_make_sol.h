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
 * \brief Definition of laplacian_2d_grid_make_sol function.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"

template <typename Matrix>
[[nodiscard]] inline auto laplacian_2d_grid_make_sol(
    const num_prob_collect::finite_element::laplacian_2d_grid<Matrix>& grid)
    -> Eigen::VectorXd {
    const num_collect::index_type grid_size = grid.grid_rows() + 1;
    Eigen::VectorXd expected_sol(grid.mat_size());
    for (num_collect::index_type xi = 0; xi < grid_size - 1; ++xi) {
        const double x =
            static_cast<double>(xi + 1) / static_cast<double>(grid_size);
        for (num_collect::index_type yi = 0; yi < grid_size - 1; ++yi) {
            const double y =
                static_cast<double>(yi + 1) / static_cast<double>(grid_size);
            const num_collect::index_type i = grid.index(xi, yi);
            expected_sol(i) = x * x + y * y;
        }
    }
    return expected_sol;
}
