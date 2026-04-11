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
 * \brief Benchmark of solvers of updates of full implicit Runge-Kutta methods.
 */
#include <stat_bench/benchmark_macros.h>

#include "num_collect/ode/runge_kutta/inexact_newton_decomposed_full_update_equation_solver.h"
#include "num_collect/ode/runge_kutta/inexact_newton_full_update_equation_solver.h"
#include "num_collect/ode/runge_kutta/radau2a5_formula.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/implicit_kaps_problem.h"

STAT_BENCH_CASE("full_update_equation_solver_exponential_problem",
    "inexact_newton_full_update_equation_solver") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::runge_kutta::radau2a5_formula<problem_type>;
    using solver_type = num_collect::ode::runge_kutta::
        inexact_newton_full_update_equation_solver<problem_type,
            formula_type::stages>;

    solver_type solver(formula_type::slope_coeffs(),
        formula_type::time_coeffs(), formula_type::update_coeffs());

    problem_type problem;
    constexpr double time = 0.0;
    constexpr double step_size = 1e-2;
    constexpr double variable = 1.0;

    STAT_BENCH_MEASURE() {
        Eigen::Vector3d solution = Eigen::Vector3d::Zero();
        solver.init(problem, time, step_size, variable, solution);
        solver.solve();
    };
}

STAT_BENCH_CASE("full_update_equation_solver_exponential_problem",
    "inexact_newton_decomposed_full_update_equation_solver") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using formula_type =
        num_collect::ode::runge_kutta::radau2a5_formula<problem_type>;
    using solver_type = num_collect::ode::runge_kutta::
        inexact_newton_decomposed_full_update_equation_solver<problem_type,
            formula_type::stages>;
    using solver_data_type = num_collect::ode::runge_kutta::
        inexact_newton_decomposed_full_update_equation_solver_data<double,
            formula_type::stages>;

    const solver_data_type data(formula_type::slope_coeffs(),
        formula_type::time_coeffs(), formula_type::update_coeffs());
    solver_type solver(data);

    problem_type problem;
    constexpr double time = 0.0;
    constexpr double step_size = 1e-2;
    constexpr double variable = 1.0;

    STAT_BENCH_MEASURE() {
        Eigen::Vector3d solution = Eigen::Vector3d::Zero();
        solver.init(problem, time, step_size, variable, solution);
        solver.solve();
    };
}

STAT_BENCH_CASE("full_update_equation_solver_implicit_kaps_problem",
    "inexact_newton_full_update_equation_solver") {
    using problem_type = num_prob_collect::ode::implicit_kaps_problem;
    using formula_type =
        num_collect::ode::runge_kutta::radau2a5_formula<problem_type>;
    using solver_type = num_collect::ode::runge_kutta::
        inexact_newton_full_update_equation_solver<problem_type,
            formula_type::stages>;

    solver_type solver(formula_type::slope_coeffs(),
        formula_type::time_coeffs(), formula_type::update_coeffs());

    constexpr double epsilon = 0.1;
    problem_type problem{epsilon};
    constexpr double time = 0.0;
    constexpr double step_size = 1e-2;
    const Eigen::Vector2d variable{{1.0, 1.0}};

    STAT_BENCH_MEASURE() {
        using solution_type = solver_type::update_vector_type;
        solution_type solution = solution_type::Zero();
        solver.init(problem, time, step_size, variable, solution);
        solver.solve();
    };
}

STAT_BENCH_MAIN
