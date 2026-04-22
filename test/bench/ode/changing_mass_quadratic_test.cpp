/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Benchmark of ODE solvers.
 */

#include <array>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <fmt/format.h>

#include "diagram_common.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/runge_kutta/ark43_esdirk_formula.h"
#include "num_collect/ode/runge_kutta/ark54_esdirk_formula.h"
#include "num_collect/ode/runge_kutta/esdirk45_formula.h"
#include "num_collect/ode/runge_kutta/radau2a5_formula.h"
#include "num_collect/ode/runge_kutta/sdirk4_formula.h"
#include "num_collect/ode/step_size_limits.h"
#include "num_prob_collect/ode/changing_mass_quadratic_problem.h"

using problem_type = num_prob_collect::ode::changing_mass_quadratic_problem;

static constexpr std::string_view problem_name =
    "changing_mass_quadratic_problem";
static constexpr std::string_view problem_description = "Changing Mass in 2D";

template <typename Solver>
inline void bench_one(
    const std::string& solver_name, bench_executor& executor) {
    constexpr double init_time = 0.0;
    constexpr double end_time = 3.0;
    const Eigen::Vector2d init_var{{0.0, 0.0}};
    const Eigen::Vector2d reference{{end_time, 0.5 * end_time * end_time}};

#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
    constexpr num_collect::index_type repetitions = 10;
#else
    constexpr num_collect::index_type repetitions = 1000;
#endif

#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
    constexpr std::array<double, 2> tolerance_list{1e-2, 1e-3};
#else
    constexpr std::array<double, 5> tolerance_list{
        1e-2, 1e-3, 1e-4, 1e-5, 1e-6};
#endif

    for (const double tol : tolerance_list) {
        const problem_type problem;
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

    // This upper limit is required to avoid failures of some solvers.
    executor.step_size_limits(
        num_collect::ode::step_size_limits<double>().upper_limit(0.1));

    bench_one<num_collect::ode::runge_kutta::sdirk4_solver<problem_type>>(
        "SDIRK4", executor);
    bench_one<num_collect::ode::runge_kutta::ark43_esdirk_solver<problem_type>>(
        "ARK4(3)-ESDIRK", executor);
    bench_one<num_collect::ode::runge_kutta::ark54_esdirk_solver<problem_type>>(
        "ARK5(4)-ESDIRK", executor);
    bench_one<num_collect::ode::runge_kutta::esdirk45_solver<problem_type>>(
        "ESDIRK45c", executor);
    bench_one<num_collect::ode::runge_kutta::radau2a5_changing_mass_auto_solver<
        problem_type>>("RadauIIA5", executor);

    executor.write_result(problem_name, problem_description, output_directory);

    return 0;
}
