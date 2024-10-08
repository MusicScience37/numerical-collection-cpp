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
 * \brief Test of reverse_cuthill_mckee_ordering class.
 */
#include "num_collect/linear/reverse_cuthill_mckee_ordering.h"

#include <algorithm>
#include <cstddef>
#include <vector>

#include <Eigen/SparseCore>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "calculate_sparse_matrix_profile.h"
#include "fmt_approval_tests.h"
#include "format_non_zeros.h"
#include "num_collect/base/index_type.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"

TEMPLATE_TEST_CASE("num_collect::linear::reverse_cuthill_mckee_ordering", "",
    float, double, long double) {
    using num_collect::linear::reverse_cuthill_mckee_ordering;
    using num_prob_collect::linear::laplacian_2d_grid;

    using scalar_type = TestType;
    using matrix_type = Eigen::SparseMatrix<scalar_type>;
    using storage_index_type = typename matrix_type::StorageIndex;
    using ordering_type = reverse_cuthill_mckee_ordering<storage_index_type>;
    using permutation_type = typename ordering_type::permutation_type;

    SECTION("perform ordering") {
        constexpr num_collect::index_type grid_size = 10;
        constexpr auto grid_width = static_cast<scalar_type>(0.1);
        laplacian_2d_grid<matrix_type> grid{grid_size, grid_size, grid_width};
        const matrix_type& matrix = grid.mat();

        permutation_type permutation;
        ordering_type()(matrix, permutation);

        matrix_type twisted;
        twisted = matrix.twistedBy(permutation);

        ApprovalTests::Approvals::verify(
            fmt::format("Original:\n"
                        "{0}"
                        "Profile: {2}\n\n"
                        "Twisted:\n"
                        "{1}"
                        "Profile: {3}",
                format_non_zeros(matrix), format_non_zeros(twisted),
                calculate_sparse_matrix_profile(matrix),
                calculate_sparse_matrix_profile(twisted)));
    }

    SECTION("validate ordering") {
        constexpr num_collect::index_type grid_size = 10;
        constexpr auto grid_width = static_cast<scalar_type>(0.1);
        laplacian_2d_grid<matrix_type> grid{grid_size, grid_size, grid_width};
        const matrix_type& matrix = grid.mat();

        permutation_type permutation;
        ordering_type()(matrix, permutation);

        auto indices = std::vector<int>(
            permutation.indices().begin(), permutation.indices().end());
        std::sort(indices.begin(), indices.end());

        std::vector<int> expected;
        expected.reserve(static_cast<std::size_t>(matrix.rows()));
        for (int i = 0, size = static_cast<int>(matrix.rows()); i < size; ++i) {
            expected.push_back(i);
        }

        CHECK(indices == expected);
    }
}
