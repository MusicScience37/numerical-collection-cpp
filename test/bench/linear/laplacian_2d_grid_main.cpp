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
 * \brief Benchmark of solving equations of Laplacian matrices.
 */
#include <algorithm>

#include <stat_bench/benchmark_macros.h>

#ifdef _OPENMP
#include <omp.h>
#endif

STAT_BENCH_GROUP("laplacian_2d_grid")
    .add_parameter_to_time_line_plot_log("size");

auto main(int argc, const char** argv) -> int {
#ifdef _OPENMP
    const int num_procs = omp_get_num_procs();
    constexpr double threads_rate = 0.5;
    const auto num_threads =
        std::max(static_cast<int>(num_procs * threads_rate), 2);
    omp_set_num_threads(num_threads);
#endif
    return stat_bench::impl::default_main(argc, argv);
}
