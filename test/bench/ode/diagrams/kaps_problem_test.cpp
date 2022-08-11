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
#include "num_prob_collect/ode/kaps_problem.h"

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
#include "num_collect/ode/runge_kutta/dopri5_formula.h"
#include "num_collect/ode/runge_kutta/rk4_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_collect/ode/runge_kutta/sdirk4_formula.h"
#include "num_collect/ode/runge_kutta/tanaka1_formula.h"
#include "num_collect/ode/runge_kutta/tanaka2_formula.h"

using problem_type = num_prob_collect::ode::kaps_problem;

static constexpr std::string_view problem_name_base = "kaps_problem";

template <typename Solver>
inline void bench_one(
    const std::string& solver_name, double epsilon, bench_result& result) {
    constexpr double init_time = 0.0;
    constexpr double end_time = 1.0;
    const Eigen::Vector2d init_var{{1.0, 1.0}};
    const Eigen::Vector2d reference{
        {std::exp(-2.0 * end_time), std::exp(-end_time)}};

#ifndef NDEBUG
    constexpr num_collect::index_type repetitions = 10;
#else
    num_collect::index_type repetitions = 1000;  // NOLINT
    if (solver_name == "RKF45" || solver_name == "DOPRI5") {
        if (epsilon < 1e-4) {
            repetitions = 10;  // NOLINT
        } else {
            repetitions = 100;  // NOLINT
        }
    }
#endif

    constexpr std::array<double, 5> tolerance_list{
        1e-2, 1e-3, 1e-4, 1e-5, 1e-6};

    for (const double tol : tolerance_list) {
        const problem_type problem{epsilon};
        perform<problem_type, Solver>(solver_name, problem, init_time, end_time,
            init_var, reference, repetitions, tol, result);
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

#ifndef NDEBUG
    constexpr std::array<double, 2> epsilon_list{1.0, 1e-3};
#else
    constexpr std::array<double, 3> epsilon_list{1.0, 1e-3, 1e-6};
#endif
    for (const double epsilon : epsilon_list) {
        const std::string problem_name = fmt::format(
            "{}{:.0f}", problem_name_base, std::abs(std::log10(epsilon)));

        bench_result result{};

        bench_one<num_collect::ode::runge_kutta::rkf45_solver<problem_type>>(
            "RKF45", epsilon, result);
        bench_one<num_collect::ode::runge_kutta::dopri5_solver<problem_type>>(
            "DOPRI5", epsilon, result);
        bench_one<num_collect::ode::runge_kutta::tanaka1_solver<problem_type>>(
            "Tanaka1", epsilon, result);
        bench_one<num_collect::ode::runge_kutta::tanaka2_solver<problem_type>>(
            "Tanaka2", epsilon, result);
        bench_one<num_collect::ode::runge_kutta::sdirk4_solver<problem_type>>(
            "SDIRK4", epsilon, result);
        bench_one<num_collect::ode::rosenbrock::ros3w_solver<problem_type>>(
            "ROS3w", epsilon, result);
        bench_one<num_collect::ode::rosenbrock::ros34pw3_solver<problem_type>>(
            "ROS34PW3", epsilon, result);
        bench_one<num_collect::ode::rosenbrock::rodasp_solver<problem_type>>(
            "RODASP", epsilon, result);
        bench_one<num_collect::ode::rosenbrock::rodaspr_solver<problem_type>>(
            "RODASPR", epsilon, result);

        write_result(problem_name, result, output_directory);
    }

    return 0;
}
