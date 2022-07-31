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
#include <memory>
#include <type_traits>

#include <Eigen/Core>
#include <stat_bench/bench/invocation_context.h>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/param/parameter_value_vector.h>

#include "num_collect/base/index_type.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/non_embedded_formula_wrapper.h"
#include "num_collect/ode/runge_kutta/rk4_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_collect/ode/runge_kutta/ros34pw3_formula.h"
#include "num_collect/ode/runge_kutta/ros3w_formula.h"
#include "num_collect/ode/runge_kutta/tanaka1_formula.h"
#include "num_collect/ode/runge_kutta/tanaka2_formula.h"
#include "num_prob_collect/ode/free_fall_in_resistance_problem.h"

STAT_BENCH_MAIN

using problem_type = num_prob_collect::ode::free_fall_in_resistance_problem;

class free_fall_in_resistance_fixture : public stat_bench::FixtureBase {
public:
    free_fall_in_resistance_fixture() {
        add_param<double>("k")->add(1.0)->add(1e+3);  // NOLINT
    }

    void setup(stat_bench::bench::InvocationContext& context) override {
        k_ = context.get_param<double>("k");
    }

    [[nodiscard]] auto problem() const -> problem_type {
        return problem_type(k_, g_);
    }

    template <typename Solver>
    void perform(Solver& solver) {
        constexpr double init_time = 0.0;
        const Eigen::Vector2d init_var = Eigen::Vector2d(0.0, 0.0);
        solver.init(init_time, init_var);
#ifndef NDEBUG
        constexpr double end_time = 0.1;
#else
        constexpr double end_time = 5.0;
#endif
        solver.solve_till(end_time);
        steps_ = solver.steps();

        const Eigen::Vector2d reference =
            Eigen::Vector2d((g_ / k_) * std::expm1(-k_ * end_time),
                -(g_ / (k_ * k_)) * std::expm1(-k_ * end_time) -
                    g_ / k_ * end_time);
        error_ = (solver.variable() - reference).norm();
    }

    void tear_down(stat_bench::bench::InvocationContext& context) override {
        context.add_custom_output("steps", static_cast<double>(steps_));
        context.add_custom_output("error", error_);
    }

private:
    double k_{1.0};
    double g_{1.0};
    num_collect::index_type steps_{};
    double error_{};
};

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "rkf45") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::rkf45_solver<problem_type>;
        auto solver = solver_type(problem());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "tanaka1") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::tanaka1_solver<problem_type>;
        auto solver = solver_type(problem());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "tanaka2") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::tanaka2_solver<problem_type>;
        auto solver = solver_type(problem());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "ros3w") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::ros3w_solver<problem_type>;
        auto solver = solver_type(problem());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "ros34pw3") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::ode::runge_kutta::ros34pw3_solver<problem_type>;
        auto solver = solver_type(problem());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "rk4_auto") {
    STAT_BENCH_MEASURE() {
        using solver_type = num_collect::ode::non_embedded_auto_solver<
            num_collect::ode::runge_kutta::rk4_formula<problem_type>>;
        auto solver = solver_type(problem());
        perform(solver);
    };
}
