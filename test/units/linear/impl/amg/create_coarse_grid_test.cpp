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
 * \brief Test of create_coarse_grid function.
 */
#include "num_collect/linear/impl/amg/create_coarse_grid.h"

#include <ApprovalTests.hpp>
#include <catch2/catch_test_macros.hpp>

#include "num_prob_collect/linear/laplacian_2d_grid.h"

TEST_CASE("num_collect::linear::impl::amg::create_coarse_grid") {
    using num_collect::linear::impl::amg::create_coarse_grid;
    using num_collect::linear::impl::amg::node_layer;

    SECTION("apply to laplacian_2d_grid") {
        using num_prob_collect::linear::laplacian_2d_grid;
        using scalar_type = double;
        using matrix_type = Eigen::SparseMatrix<scalar_type>;
        using storage_index_type = typename matrix_type::StorageIndex;

        constexpr num_collect::index_type grid_size = 10;
        constexpr auto grid_width = static_cast<scalar_type>(0.1);
        constexpr auto strong_coeff_rate_threshold =
            static_cast<scalar_type>(0.5);
        laplacian_2d_grid<matrix_type> grid{grid_size, grid_size, grid_width};
        const matrix_type& matrix = grid.mat();

        const auto node_classification =
            create_coarse_grid(matrix, strong_coeff_rate_threshold);

        fmt::memory_buffer buffer;
        buffer.append(std::string_view("Classification:"));
        for (num_collect::index_type i = 0; i < node_classification.size();
             ++i) {
            if (i % grid_size == 0U) {
                buffer.push_back('\n');
            }
            if (node_classification[i] == node_layer::coarse) {
                buffer.push_back('x');
            } else {
                buffer.push_back('o');
            }
            buffer.push_back(' ');
        }
        ApprovalTests::Approvals::verify(
            std::string(buffer.data(), buffer.size()));
    }
}
