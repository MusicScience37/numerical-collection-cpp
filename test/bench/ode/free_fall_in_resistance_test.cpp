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
#include <celero/Celero.h>

#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_collect/ode/runge_kutta/ros34pw3_formula.h"
#include "num_collect/ode/runge_kutta/ros3w_formula.h"
#include "num_collect/ode/runge_kutta/tanaka1_formula.h"
#include "num_collect/ode/runge_kutta/tanaka2_formula.h"
#include "num_prob_collect/ode/free_fall_in_resistance_problem.h"
#include "steps_udm.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

using problem_type = num_prob_collect::ode::free_fall_in_resistance_problem;

class free_fall_in_resistance_fixture : public celero::TestFixture {
public:
    free_fall_in_resistance_fixture() = default;

    [[nodiscard]] auto getExperimentValues() const
        -> std::vector<celero::TestFixture::ExperimentValue> override {
        std::vector<celero::TestFixture::ExperimentValue> problem_space;
        problem_space.emplace_back(1);     // NOLINT
        problem_space.emplace_back(1000);  // NOLINT
        return problem_space;
    }

    void setUp(
        const celero::TestFixture::ExperimentValue& experiment_value) override {
        k_ = static_cast<int>(experiment_value.Value);
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
        steps_->addValue(solver.steps());
    }

    [[nodiscard]] auto getUserDefinedMeasurements() const -> std::vector<
        std::shared_ptr<celero::UserDefinedMeasurement>> override {
        return {steps_};
    }

private:
    double k_{1.0};
    double g_{1.0};
    std::shared_ptr<steps_udm> steps_{std::make_shared<steps_udm>()};
};

// NOLINTNEXTLINE: external library
BASELINE_F(ode_rk_free_fall_in_resistance, rkf45,
    free_fall_in_resistance_fixture, 30, 100) {
    using solver_type =
        num_collect::ode::runge_kutta::rkf45_solver<problem_type>;
    auto solver = solver_type(problem());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(ode_rk_free_fall_in_resistance, tanaka1,
    free_fall_in_resistance_fixture, 30, 10) {
    using solver_type =
        num_collect::ode::runge_kutta::tanaka1_solver<problem_type>;
    auto solver = solver_type(problem());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(ode_rk_free_fall_in_resistance, tanaka2,
    free_fall_in_resistance_fixture, 30, 100) {
    using solver_type =
        num_collect::ode::runge_kutta::tanaka2_solver<problem_type>;
    auto solver = solver_type(problem());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(ode_rk_free_fall_in_resistance, ros3w,
    free_fall_in_resistance_fixture, 30, 100) {
    using solver_type =
        num_collect::ode::runge_kutta::ros3w_solver<problem_type>;
    auto solver = solver_type(problem());
    perform(solver);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(ode_rk_free_fall_in_resistance, ros34pw3,
    free_fall_in_resistance_fixture, 30, 100) {
    using solver_type =
        num_collect::ode::runge_kutta::ros34pw3_solver<problem_type>;
    auto solver = solver_type(problem());
    perform(solver);
}
