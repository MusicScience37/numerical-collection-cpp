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
#include <celero/Celero.h>

#include "log_energy_change.h"
#include "num_collect/ode/avf/avf2_formula.h"
#include "num_collect/ode/avf/avf3_formula.h"
#include "num_collect/ode/non_embedded_formula_wrapper.h"
#include "num_collect/ode/runge_kutta/rk4_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_collect/ode/runge_kutta/ros34pw3_formula.h"
#include "num_collect/ode/runge_kutta/ros3w_formula.h"
#include "num_collect/ode/runge_kutta/tanaka1_formula.h"
#include "num_collect/ode/runge_kutta/tanaka2_formula.h"
#include "num_prob_collect/ode/pendulum_movement_problem.h"
#include "steps_udm.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

class pendulum_movement_fixture : public celero::TestFixture {
public:
    pendulum_movement_fixture() = default;

    [[nodiscard]] static auto calc_energy(const Eigen::Vector2d& var)
        -> double {
        return 0.5 * var[0] * var[0] - std::cos(var[1]);  // NOLINT
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
        steps_->addValue(solver.steps());

        const double init_energy = calc_energy(init_var);
        const double energy = calc_energy(solver.variable());
        log_energy_change_->addValue(
            std::log10(std::abs(energy - init_energy)));
    }

    [[nodiscard]] auto getUserDefinedMeasurements() const -> std::vector<
        std::shared_ptr<celero::UserDefinedMeasurement>> override {
        return {steps_, log_energy_change_};
    }

private:
    std::shared_ptr<steps_udm> steps_{std::make_shared<steps_udm>()};
    std::shared_ptr<log_energy_change> log_energy_change_{
        std::make_shared<log_energy_change>()};
};

using problem_type = num_prob_collect::ode::pendulum_movement_problem;

// NOLINTNEXTLINE: external library
BASELINE_F(
    ode_rk_pendulum_movement, rkf45, pendulum_movement_fixture, 30, 100) {
    using solver_type =
        num_collect::ode::runge_kutta::rkf45_solver<problem_type>;
    auto solver = solver_type(problem_type());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    ode_rk_pendulum_movement, tanaka1, pendulum_movement_fixture, 30, 10) {
    using solver_type =
        num_collect::ode::runge_kutta::tanaka1_solver<problem_type>;
    auto solver = solver_type(problem_type());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    ode_rk_pendulum_movement, tanaka2, pendulum_movement_fixture, 30, 100) {
    using solver_type =
        num_collect::ode::runge_kutta::tanaka2_solver<problem_type>;
    auto solver = solver_type(problem_type());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    ode_rk_pendulum_movement, ros3w, pendulum_movement_fixture, 30, 100) {
    using solver_type =
        num_collect::ode::runge_kutta::ros3w_solver<problem_type>;
    auto solver = solver_type(problem_type());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    ode_rk_pendulum_movement, ros34pw3, pendulum_movement_fixture, 30, 100) {
    using solver_type =
        num_collect::ode::runge_kutta::ros34pw3_solver<problem_type>;
    auto solver = solver_type(problem_type());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    ode_rk_pendulum_movement, rk4_auto, pendulum_movement_fixture, 30, 100) {
    using solver_type = num_collect::ode::non_embedded_auto_solver<
        num_collect::ode::runge_kutta::rk4_formula<problem_type>>;
    auto solver = solver_type(problem_type());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    ode_rk_pendulum_movement, avf2_auto, pendulum_movement_fixture, 30, 100) {
    using solver_type = num_collect::ode::avf::avf2_auto_solver<problem_type>;
    auto solver = solver_type(problem_type());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    ode_rk_pendulum_movement, avf3_auto, pendulum_movement_fixture, 30, 100) {
    using solver_type = num_collect::ode::avf::avf3_auto_solver<problem_type>;
    auto solver = solver_type(problem_type());
    perform(solver);
}
