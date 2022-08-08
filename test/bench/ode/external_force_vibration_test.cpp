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
 * \brief Test of solving ODE of vibration with external force.
 */
#include <cmath>

#include <Eigen/Core>
#include <stat_bench/bench/invocation_context.h>
#include <stat_bench/benchmark_macros.h>

#include "num_collect/base/index_type.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/non_embedded_formula_wrapper.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_collect/ode/rosenbrock/ros34pw3_formula.h"
#include "num_collect/ode/rosenbrock/ros3w_formula.h"
#include "num_collect/ode/runge_kutta/rk4_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_collect/ode/runge_kutta/tanaka1_formula.h"
#include "num_collect/ode/runge_kutta/tanaka2_formula.h"
#include "num_prob_collect/ode/autonomous_external_force_vibration_problem.h"
#include "num_prob_collect/ode/external_force_vibration_problem.h"

STAT_BENCH_MAIN

class external_force_vibration_fixture : public stat_bench::FixtureBase {
public:
    external_force_vibration_fixture() = default;

    template <typename Solver>
    void perform(Solver& solver) {
        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var = Eigen::Vector2d(-1.0, 0.0);
        solver.init(init_time, init_var);
#ifndef NDEBUG
        constexpr double end_time = 0.1;
#else
        constexpr double end_time = 10.0;
#endif
        solver.solve_till(end_time);
        steps_ = solver.steps();

        const Eigen::Vector2d reference =
            Eigen::Vector2d(-std::cos(end_time), -std::sin(end_time));
        error_ = (solver.variable() - reference).norm();
    }

    void tear_down(stat_bench::bench::InvocationContext& context) override {
        context.add_custom_output("steps", static_cast<double>(steps_));
        context.add_custom_output("error", error_);
    }

protected:
    num_collect::index_type steps_{};
    double error_{};
};

using problem_type = num_prob_collect::ode::external_force_vibration_problem;

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(external_force_vibration_fixture,
    "ode_rk_external_force_vibration", "rkf45") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::rkf45_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(external_force_vibration_fixture,
    "ode_rk_external_force_vibration", "tanaka1") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::tanaka1_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(external_force_vibration_fixture,
    "ode_rk_external_force_vibration", "tanaka2") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::tanaka2_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(external_force_vibration_fixture,
    "ode_rk_external_force_vibration", "ros3w") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::rosenbrock::ros3w_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(external_force_vibration_fixture,
    "ode_rk_external_force_vibration", "ros34pw3") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::rosenbrock::ros34pw3_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(external_force_vibration_fixture,
    "ode_rk_external_force_vibration", "rodasp") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::rosenbrock::rodasp_solver<problem_type>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(external_force_vibration_fixture,
    "ode_rk_external_force_vibration", "rodasp_autonomous") {
    STAT_BENCH_MEASURE() {
        using solver_type = num_collect::ode::rosenbrock::rodasp_solver<
            num_prob_collect::ode::autonomous_external_force_vibration_problem>;
        auto solver = solver_type(num_prob_collect::ode::
                autonomous_external_force_vibration_problem());

        constexpr double init_time = 0.0;
        const Eigen::Vector3d init_var = Eigen::Vector3d(-1.0, 0.0, init_time);
        solver.init(init_time, init_var);
#ifndef NDEBUG
        constexpr double end_time = 0.1;
#else
        constexpr double end_time = 10.0;
#endif
        solver.solve_till(end_time);
        steps_ = solver.steps();

        const Eigen::Vector3d reference =
            Eigen::Vector3d(-std::cos(end_time), -std::sin(end_time), end_time);
        error_ = (solver.variable() - reference).norm();
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(external_force_vibration_fixture,
    "ode_rk_external_force_vibration", "rk4_auto") {
    STAT_BENCH_MEASURE() {
        using solver_type = num_collect::ode::non_embedded_auto_solver<
            num_collect::ode::runge_kutta::rk4_formula<problem_type>>;
        auto solver = solver_type(problem_type());
        perform(solver);
    };
}
