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
 * \brief Test of solving ODE of free false in resistance.
 */
#include <cmath>
#include <memory>
#include <type_traits>

#include <Eigen/Core>
#include <stat_bench/bench/invocation_context.h>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/param/parameter_value_vector.h>

#include "num_collect/base/index_type.h"
#include "num_collect/ode/basic_step_size_controller.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/non_embedded_formula_wrapper.h"
#include "num_collect/ode/pi_step_size_controller.h"
#include "num_collect/ode/rosenbrock/ros3w_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_prob_collect/ode/free_fall_in_resistance_problem.h"

STAT_BENCH_MAIN

using problem_type = num_prob_collect::ode::free_fall_in_resistance_problem;

class free_fall_in_resistance_fixture : public stat_bench::FixtureBase {
public:
    free_fall_in_resistance_fixture() {
        add_param<double>("k")->add(1.0)->add(1e+2)->add(1e+4);  // NOLINT
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
    "ode_rk_free_fall_in_resistance", "rkf45_basic") {
    STAT_BENCH_MEASURE() {
        using formula_type =
            num_collect::ode::runge_kutta::rkf45_formula<problem_type>;
        using solver_type = num_collect::ode::embedded_solver<formula_type,
            num_collect::ode::basic_step_size_controller<formula_type>>;
        auto solver = solver_type(problem());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "rkf45_pi_gustafsson") {
    STAT_BENCH_MEASURE() {
        using formula_type =
            num_collect::ode::runge_kutta::rkf45_formula<problem_type>;
        using solver_type = num_collect::ode::embedded_solver<formula_type,
            num_collect::ode::pi_step_size_controller<formula_type>>;
        auto solver = solver_type(problem());
        solver.step_size_controller().current_step_error_exponent(
            0.7 / (formula_type::lesser_order + 1));  // NOLINT
        solver.step_size_controller().previous_step_error_exponent(
            0.4 / (formula_type::lesser_order + 1));  // NOLINT
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "rkf45_pi_hairer") {
    STAT_BENCH_MEASURE() {
        using formula_type =
            num_collect::ode::runge_kutta::rkf45_formula<problem_type>;
        using solver_type = num_collect::ode::embedded_solver<formula_type,
            num_collect::ode::pi_step_size_controller<formula_type>>;
        auto solver = solver_type(problem());
        solver.step_size_controller().current_step_error_exponent(
            1.0 / (formula_type::lesser_order + 1));  // NOLINT
        solver.step_size_controller().previous_step_error_exponent(
            0.08);  // NOLINT
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "ros3w_basic") {
    STAT_BENCH_MEASURE() {
        using formula_type =
            num_collect::ode::rosenbrock::ros3w_formula<problem_type>;
        using solver_type = num_collect::ode::embedded_solver<formula_type,
            num_collect::ode::basic_step_size_controller<formula_type>>;
        auto solver = solver_type(problem());
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "ros3w_pi_gustafsson") {
    STAT_BENCH_MEASURE() {
        using formula_type =
            num_collect::ode::rosenbrock::ros3w_formula<problem_type>;
        using solver_type = num_collect::ode::embedded_solver<formula_type,
            num_collect::ode::pi_step_size_controller<formula_type>>;
        auto solver = solver_type(problem());
        solver.step_size_controller().current_step_error_exponent(
            0.7 / (formula_type::lesser_order + 1));  // NOLINT
        solver.step_size_controller().previous_step_error_exponent(
            0.4 / (formula_type::lesser_order + 1));  // NOLINT
        perform(solver);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(free_fall_in_resistance_fixture,
    "ode_rk_free_fall_in_resistance", "ros3w_pi_hairer") {
    STAT_BENCH_MEASURE() {
        using formula_type =
            num_collect::ode::rosenbrock::ros3w_formula<problem_type>;
        using solver_type = num_collect::ode::embedded_solver<formula_type,
            num_collect::ode::pi_step_size_controller<formula_type>>;
        auto solver = solver_type(problem());
        solver.step_size_controller().current_step_error_exponent(
            1.0 / (formula_type::lesser_order + 1));  // NOLINT
        solver.step_size_controller().previous_step_error_exponent(
            0.08);  // NOLINT
        perform(solver);
    };
}
