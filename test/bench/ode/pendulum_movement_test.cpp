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
 * \brief Test of solving ODE of pendulum movement.
 */
#include <Eigen/Core>
#include <fmt/format.h>

#include "diagram_common.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/avf/avf2_formula.h"
#include "num_collect/ode/avf/avf3_formula.h"
#include "num_collect/ode/avf/avf4_formula.h"
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
#include "num_prob_collect/ode/pendulum_movement_problem.h"

using problem_type = num_prob_collect::ode::pendulum_movement_problem;

static constexpr std::string_view problem_name = "pendulum_movement_problem";
static constexpr std::string_view problem_description = "Pendulum Movement";

static auto calculate_reference(double init_time, double end_time,
    const Eigen::Vector2d& init_var) -> Eigen::Vector2d {
    num_collect::ode::runge_kutta::rk4_solver<problem_type> solver{
        problem_type{}};
#ifndef NDEBUG
    constexpr double step_size = 1e-4;
#else
    constexpr double step_size = 1e-6;
#endif
    solver.step_size(step_size);
    solver.init(init_time, init_var);
    solver.solve_till(end_time);
    return solver.variable();
}

static constexpr double init_time = 0.0;
#ifndef NDEBUG
static constexpr double end_time = 1.0;
#else
static constexpr double end_time = 10.0;
#endif
static const Eigen::Vector2d init_var = Eigen::Vector2d(0.0, 1.0);

static const Eigen::Vector2d reference =
    calculate_reference(init_time, end_time, init_var);

template <typename Solver>
inline void bench_one(
    const std::string& solver_name, bench_executor& executor) {
#ifndef NDEBUG
    constexpr num_collect::index_type repetitions = 10;
#else
    constexpr num_collect::index_type repetitions = 1000;
#endif

    constexpr std::array<double, 5> tolerance_list{
        1e-2, 1e-3, 1e-4, 1e-5, 1e-6};

    for (const double tol : tolerance_list) {
        const problem_type problem;
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
    bench_one<num_collect::ode::runge_kutta::tanaka1_solver<problem_type>>(
        "Tanaka1", executor);
    bench_one<num_collect::ode::runge_kutta::tanaka2_solver<problem_type>>(
        "Tanaka2", executor);
    bench_one<num_collect::ode::runge_kutta::sdirk4_solver<problem_type>>(
        "SDIRK4", executor);
    bench_one<num_collect::ode::runge_kutta::ark43_esdirk_solver<problem_type>>(
        "ARK4(3)-ESDIRK", executor);
    bench_one<num_collect::ode::runge_kutta::ark54_esdirk_solver<problem_type>>(
        "ARK5(4)-ESDIRK", executor);
    bench_one<num_collect::ode::runge_kutta::esdirk45_solver<problem_type>>(
        "ESDIRK45c", executor);
    bench_one<num_collect::ode::rosenbrock::ros3w_solver<problem_type>>(
        "ROS3w", executor);
    bench_one<num_collect::ode::rosenbrock::ros34pw3_solver<problem_type>>(
        "ROS34PW3", executor);
    bench_one<num_collect::ode::rosenbrock::rodasp_solver<problem_type>>(
        "RODASP", executor);
    bench_one<num_collect::ode::rosenbrock::rodaspr_solver<problem_type>>(
        "RODASPR", executor);
    bench_one<num_collect::ode::avf::avf2_auto_solver<problem_type>>(
        "AVF2", executor);
    bench_one<num_collect::ode::avf::avf3_auto_solver<problem_type>>(
        "AVF3", executor);
    bench_one<num_collect::ode::avf::avf4_auto_solver<problem_type>>(
        "AVF4", executor);

    executor.write_result(problem_name, problem_description, output_directory);

    return 0;
}
