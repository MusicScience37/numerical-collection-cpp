/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of solving Kaps' problem.
 */
#include "num_prob_collect/ode/implicit_kaps_problem.h"

#include <array>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <fmt/format.h>

#include "diagram_common.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/basic_step_size_controller.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/ode/rosenbrock/ros34pw3_formula.h"
#include "num_collect/ode/rosenbrock/ros3w_formula.h"

using problem_type = num_prob_collect::ode::implicit_kaps_problem;

static constexpr std::string_view problem_name_base = "implicit_kaps_problem";

template <typename Solver>
inline void bench_one(
    const std::string& solver_name, double epsilon, bench_executor& executor) {
    constexpr double init_time = 0.0;
    constexpr double end_time = 1.0;
    const Eigen::Vector2d init_var{{1.0, 1.0}};
    const Eigen::Vector2d reference{
        {std::exp(-2.0 * end_time), std::exp(-end_time)}};

#ifndef NDEBUG
    constexpr num_collect::index_type repetitions = 10;
#else
    constexpr num_collect::index_type repetitions = 1000;
#endif

    constexpr std::array<double, 5> tolerance_list{
        1e-2, 1e-3, 1e-4, 1e-5, 1e-6};

    for (const double tol : tolerance_list) {
        const problem_type problem{epsilon};
        executor.perform<problem_type, Solver>(solver_name, problem, init_time,
            end_time, init_var, reference, repetitions, tol);
    }
}

auto main(int argc, char** argv) -> int {
    if (argc != 2) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        fmt::print("Usage: {} <output_directory>", argv[0]);
        return 1;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    const std::string_view output_directory = argv[1];

    configure_logging();

    constexpr std::array<double, 4> epsilon_list{1.0, 1e-3, 1e-6, 0.0};
    for (const double epsilon : epsilon_list) {
        const std::string problem_name = fmt::format(
            "{}{:.0f}", problem_name_base, std::abs(std::log10(epsilon)));

        bench_executor executor{};

        bench_one<num_collect::ode::rosenbrock::ros3w_solver<problem_type>>(
            "ROS3w", epsilon, executor);
        bench_one<num_collect::ode::rosenbrock::ros34pw3_solver<problem_type>>(
            "ROS34PW3", epsilon, executor);
        bench_one<num_collect::ode::rosenbrock::rodasp_solver<problem_type>>(
            "RODASP", epsilon, executor);
        bench_one<num_collect::ode::rosenbrock::rodaspr_solver<problem_type>>(
            "RODASPR", epsilon, executor);

        executor.write_result(problem_name,
            fmt::format("Kaps' problem (epsilon={:.0e})", epsilon),
            output_directory);
    }

    return 0;
}
