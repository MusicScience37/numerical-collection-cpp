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
 * \brief Test of solving ODE of spring movement with fixed step sizes.
 */
#include <array>
#include <cmath>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <fmt/base.h>

#include "diagram_common.h"
#include "fixed_step_bench_executor.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/avf/avf2_formula.h"
#include "num_collect/ode/avf/avf3_formula.h"
#include "num_collect/ode/avf/avf4_formula.h"
#include "num_collect/ode/runge_kutta/rk4_formula.h"
#include "num_collect/ode/symplectic/leap_frog_formula.h"
#include "num_collect/ode/symplectic/symplectic_forest4_formula.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

using problem_type = num_prob_collect::ode::spring_movement_problem;

static constexpr std::string_view problem_name =
    "spring_movement_problem_fixed_step";
static constexpr std::string_view problem_description = "Spring Movement";

template <typename Solver>
inline void bench_one(
    const std::string& solver_name, fixed_step_bench_executor& executor) {
    constexpr double init_time = 0.0;
#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
    constexpr double end_time = 10.0;
#else
    constexpr double end_time = 100.0;
#endif
    const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);
    const Eigen::Vector2d reference =
        Eigen::Vector2d(std::cos(end_time), std::sin(end_time));
    const auto energy_function = [](const Eigen::Vector2d& var) -> double {
        return var.squaredNorm();
    };

#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
    constexpr std::array<double, 3> step_size_list{1e-1, 1e-2, 1e-3};
#else
    constexpr std::array<double, 4> step_size_list{1e-1, 1e-2, 1e-3, 1e-4};
#endif

    for (const double step_size : step_size_list) {
#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
        constexpr num_collect::index_type repetitions = 10;
#else
        const auto repetitions =
            static_cast<num_collect::index_type>(10000.0 * step_size);
#endif
        const problem_type problem;
        executor.perform<problem_type, Solver>(solver_name, problem, init_time,
            end_time, init_var, reference, repetitions, step_size,
            energy_function);
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

    fixed_step_bench_executor executor{};

    bench_one<num_collect::ode::runge_kutta::rk4_solver<problem_type>>(
        "RK4", executor);
    bench_one<num_collect::ode::symplectic::leap_frog_solver<problem_type>>(
        "LeapFrog", executor);
    bench_one<
        num_collect::ode::symplectic::symplectic_forest4_solver<problem_type>>(
        "Forest4", executor);
    bench_one<num_collect::ode::avf::avf2_solver<problem_type>>(
        "AVF2", executor);
    bench_one<num_collect::ode::avf::avf3_solver<problem_type>>(
        "AVF3", executor);
    bench_one<num_collect::ode::avf::avf4_solver<problem_type>>(
        "AVF4", executor);

    executor.write_result(problem_name, problem_description, output_directory);

    return 0;
}
