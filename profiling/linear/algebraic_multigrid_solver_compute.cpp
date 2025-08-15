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
 * \brief Profiling of compute function in algebraic_multigrid_solver class.
 */
#include <gperftools/profiler.h>

#include "num_collect/linear/algebraic_multigrid_solver.h"
#include "num_prob_collect/linear/laplacian_2d_grid.h"

auto main() -> int {
#ifndef NDEBUG
    constexpr std::size_t grid_size = 32;  // For debugging.
#else
    constexpr std::size_t grid_size = 320;  // For profiling.
#endif
    constexpr double grid_width = 1.0 / static_cast<double>(grid_size + 1);

    using mat_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
    num_prob_collect::linear::laplacian_2d_grid<mat_type> grid{
        grid_size, grid_size, grid_width};

    ProfilerStart("profile_linear_algebraic_multigrid_solver_compute.prof");
#ifdef NDEBUG
    // NOLINTNEXTLINE
    for (std::size_t i = 0; i < 10; ++i) {
#endif
        num_collect::linear::algebraic_multigrid_solver<mat_type> solver;
        solver.compute(grid.mat());
#ifdef NDEBUG
    }
#endif
    ProfilerStop();

    return 0;
}
