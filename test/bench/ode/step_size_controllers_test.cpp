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
 * \brief Test of controllers of step sizes in ODE solvers.
 */
#include <array>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <fmt/format.h>

#include "diagram_common.h"
#include "num_collect/ode/classic_step_size_controller.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/non_embedded_formula_wrapper.h"
#include "num_collect/ode/pi_step_size_controller.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_collect/ode/rosenbrock/ros3w_formula.h"
#include "num_collect/ode/runge_kutta/radau2a5_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_prob_collect/ode/kaps_problem.h"

using problem_type = num_prob_collect::ode::kaps_problem;
constexpr double epsilon = 1e-3;

static constexpr std::string_view problem_name =
    "kaps_problem_step_size_controllers";
static constexpr std::string_view problem_description =
    "Kaps' Problem (epsilon=1e-3)";

template <typename Solver>
inline void bench_one(
    const std::string& solver_name, bench_executor& executor) {
    constexpr double init_time = 0.0;
    constexpr double end_time = 1.0;
    const Eigen::Vector2d init_var{{1.0, 1.0}};
    const Eigen::Vector2d reference{
        {std::exp(-2.0 * end_time), std::exp(-end_time)}};

#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
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
        fmt::print("Usage: {} <output_directory>", argv[0]);
        return 1;
    }

    const std::string_view output_directory = argv[1];

    configure_logging();

    bench_executor executor{};

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::runge_kutta::rkf45_formula<problem_type>,
        num_collect::ode::classic_step_size_controller<problem_type>>>(
        "RKF45_classic", executor);

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::runge_kutta::rkf45_formula<problem_type>,
        num_collect::ode::pi_step_size_controller<problem_type>>>(
        "RKF45_pi", executor);

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::non_embedded_formula_wrapper<
            num_collect::ode::runge_kutta::radau2a5_formula<problem_type>>,
        num_collect::ode::classic_step_size_controller<problem_type>>>(
        "Radau2A5_classic", executor);

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::non_embedded_formula_wrapper<
            num_collect::ode::runge_kutta::radau2a5_formula<problem_type>>,
        num_collect::ode::pi_step_size_controller<problem_type>>>(
        "Radau2A5_pi", executor);

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::rosenbrock::ros3w_formula<problem_type>,
        num_collect::ode::classic_step_size_controller<problem_type>>>(
        "ROS3w_classic", executor);

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::rosenbrock::ros3w_formula<problem_type>,
        num_collect::ode::pi_step_size_controller<problem_type>>>(
        "ROS3w_pi", executor);

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::rosenbrock::rodasp_formula<problem_type>,
        num_collect::ode::classic_step_size_controller<problem_type>>>(
        "RODASP_classic", executor);

    bench_one<num_collect::ode::embedded_solver<
        num_collect::ode::rosenbrock::rodasp_formula<problem_type>,
        num_collect::ode::pi_step_size_controller<problem_type>>>(
        "RODASP_pi", executor);

    executor.write_result(problem_name, problem_description, output_directory);

    return 0;
}
