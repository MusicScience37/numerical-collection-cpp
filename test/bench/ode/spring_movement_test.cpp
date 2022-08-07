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
 * \brief Test of solving ODE of spring movement.
 */
#include <cmath>

#include <Eigen/Core>
#include <stat_bench/bench/invocation_context.h>
#include <stat_bench/benchmark_macros.h>

#include "num_collect/base/index_type.h"
#include "num_collect/ode/avf/avf2_formula.h"
#include "num_collect/ode/avf/avf3_formula.h"
#include "num_collect/ode/avf/avf4_formula.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/non_embedded_formula_wrapper.h"
#include "num_collect/ode/rosenbrock/ros3w_formula.h"
#include "num_collect/ode/runge_kutta/rk4_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_collect/ode/runge_kutta/ros34pw3_formula.h"
#include "num_collect/ode/runge_kutta/tanaka1_formula.h"
#include "num_collect/ode/runge_kutta/tanaka2_formula.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

STAT_BENCH_MAIN

class spring_movement_fixture : public stat_bench::FixtureBase {
public:
    spring_movement_fixture() = default;

    [[nodiscard]] static auto calc_energy(const Eigen::Vector2d& var)
        -> double {
        return 0.5 * var.squaredNorm();  // NOLINT
    }

    template <typename Solver>
    void perform(Solver& solver) {
        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);
        solver.init(init_time, init_var);
#ifndef NDEBUG
        constexpr double end_time = 0.1;
#else
        constexpr double end_time = 10.0;
#endif
        solver.solve_till(end_time);
        steps_ = solver.steps();

        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(end_time), std::sin(end_time));
        error_ = (solver.variable() - reference).norm();

        const double init_energy = calc_energy(init_var);
        const double energy = calc_energy(solver.variable());
        energy_change_rate_ = std::abs(energy - init_energy) / init_energy;
    }

    void tear_down(stat_bench::bench::InvocationContext& context) override {
        context.add_custom_output("steps", static_cast<double>(steps_));
        context.add_custom_output("error", error_);
        context.add_custom_output("energy_change_rate", energy_change_rate_);
    }

private:
    num_collect::index_type steps_{};
    double error_{};
    double energy_change_rate_{};
};

using problem_type = num_prob_collect::ode::spring_movement_problem;

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(spring_movement_fixture, "ode_rk_spring_movement", "rkf45") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::rkf45_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    spring_movement_fixture, "ode_rk_spring_movement", "tanaka1") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::tanaka1_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    spring_movement_fixture, "ode_rk_spring_movement", "tanaka2") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::tanaka2_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(spring_movement_fixture, "ode_rk_spring_movement", "ros3w") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::rosenbrock::ros3w_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    spring_movement_fixture, "ode_rk_spring_movement", "ros34pw3") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::ros34pw3_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    spring_movement_fixture, "ode_rk_spring_movement", "rk4_auto") {
    STAT_BENCH_MEASURE() {
        using solver_type = num_collect::ode::non_embedded_auto_solver<
            num_collect::ode::runge_kutta::rk4_formula<problem_type>>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    spring_movement_fixture, "ode_rk_spring_movement", "avf2_auto") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::avf::avf2_auto_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    spring_movement_fixture, "ode_rk_spring_movement", "avf3_auto") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::avf::avf3_auto_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    spring_movement_fixture, "ode_rk_spring_movement", "avf4_auto") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::avf::avf4_auto_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}
