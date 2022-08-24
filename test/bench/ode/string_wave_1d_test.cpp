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
 * \brief Test of solving ODE of 1D wave equation of strings.
 */
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
#include "num_collect/ode/runge_kutta/ark43_erk_formula.h"
#include "num_collect/ode/runge_kutta/ark43_esdirk_formula.h"
#include "num_collect/ode/runge_kutta/ark54_esdirk_formula.h"
#include "num_collect/ode/runge_kutta/dopri5_formula.h"
#include "num_collect/ode/runge_kutta/esdirk45_formula.h"
#include "num_collect/ode/runge_kutta/rk4_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_collect/ode/runge_kutta/sdirk4_formula.h"
#include "num_collect/ode/runge_kutta/tanaka1_formula.h"
#include "num_collect/ode/runge_kutta/tanaka2_formula.h"
#include "num_collect/ode/step_size_limits.h"
#include "num_prob_collect/ode/string_wave_1d_problem.h"

using problem_type = num_prob_collect::ode::string_wave_1d_problem;

static constexpr std::string_view problem_name = "string_wave_1d_problem";
static constexpr std::string_view problem_description = "String Wave";

template <typename Solver>
inline void bench_one(
    const std::string& solver_name, bench_executor& executor) {
    constexpr double init_time = 0.0;

#ifndef NDEBUG
    constexpr double end_time = 0.1;
    constexpr num_collect::index_type num_points = 11;
    constexpr num_collect::index_type repetitions = 10;
#else
    constexpr double end_time = 1.0;
    constexpr num_collect::index_type num_points = 101;
    constexpr num_collect::index_type repetitions = 10;
#endif

    num_prob_collect::ode::string_wave_1d_parameters parameters{
        .num_points = num_points};
    num_prob_collect::ode::string_wave_1d_solution solution{parameters};
    solution.evaluate_on(init_time);
    const Eigen::VectorXd init_var = solution.solution();
    solution.evaluate_on(end_time);
    const Eigen::VectorXd reference = solution.solution();

    constexpr std::array<double, 5> tolerance_list{
        1e-2, 1e-3, 1e-4, 1e-5, 1e-6};

    for (const double tol : tolerance_list) {
        const problem_type problem{parameters};
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

    bench_one<num_collect::ode::runge_kutta::rkf45_solver<problem_type>>(
        "RKF45", executor);
    bench_one<num_collect::ode::runge_kutta::dopri5_solver<problem_type>>(
        "DOPRI5", executor);
    bench_one<num_collect::ode::runge_kutta::ark43_erk_solver<problem_type>>(
        "ARK4(3)-ERK", executor);

    // Implicit formulas are too slow with large step sizes.
    executor.step_size_limits(
        num_collect::ode::step_size_limits<double>().upper_limit(
            1e-2));  // NOLINT
    bench_one<num_collect::ode::rosenbrock::ros3w_solver<problem_type>>(
        "ROS3w", executor);
    bench_one<num_collect::ode::rosenbrock::ros34pw3_solver<problem_type>>(
        "ROS34PW3", executor);
    bench_one<num_collect::ode::rosenbrock::rodasp_solver<problem_type>>(
        "RODASP", executor);
    bench_one<num_collect::ode::rosenbrock::rodaspr_solver<problem_type>>(
        "RODASPR", executor);

    executor.write_result(problem_name, problem_description, output_directory);

    return 0;
}
