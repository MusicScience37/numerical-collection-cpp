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
 * \brief Test of solvers of equations in Rosenbrock method.
 */
#include <array>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <fmt/format.h>

#include "diagram_common.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/rosenbrock/lu_rosenbrock_equation_solver.h"
#include "num_collect/ode/rosenbrock/mixed_broyden_rosenbrock_equation_solver.h"
#include "num_collect/ode/rosenbrock/ros34pw3_formula.h"
#include "num_collect/ode/rosenbrock/ros3w_formula.h"
#include "num_prob_collect/ode/kaps_problem.h"

using problem_type = num_prob_collect::ode::kaps_problem;

static constexpr std::string_view problem_name = "kaps_problem3_rosenbrock";
static constexpr std::string_view problem_description =
    "Kaps' problem (epsilon=1e-3)";

template <typename Solver>
inline void bench_one(
    const std::string& solver_name, bench_executor& executor) {
    constexpr double epsilon = 1e-3;
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

    bench_executor executor{};

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::rosenbrock::ros3w_formula<problem_type,
            num_collect::ode::rosenbrock::lu_rosenbrock_equation_solver<
                problem_type>>>>("ROS3w_lu", executor);

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::rosenbrock::ros3w_formula<problem_type,
            num_collect::ode::rosenbrock::
                mixed_broyden_rosenbrock_equation_solver<problem_type>>>>(
        "ROS3w_broyden", executor);

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::rosenbrock::ros34pw3_formula<problem_type,
            num_collect::ode::rosenbrock::lu_rosenbrock_equation_solver<
                problem_type>>>>("ROS34PW3_lu", executor);

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::rosenbrock::ros34pw3_formula<problem_type,
            num_collect::ode::rosenbrock::
                mixed_broyden_rosenbrock_equation_solver<problem_type>>>>(
        "ROS34PW3_broyden", executor);

    executor.write_result(problem_name, problem_description, output_directory);

    return 0;
}
