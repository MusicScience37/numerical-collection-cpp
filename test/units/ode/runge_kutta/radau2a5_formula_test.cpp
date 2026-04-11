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
 * \brief Test of radau2a5_formula class.
 */
#include "num_collect/ode/runge_kutta/radau2a5_formula.h"

#include <cmath>
#include <type_traits>
#include <variant>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "comparison_approvals.h"
#include "num_collect/logging/logger.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::runge_kutta::radau2a5_formula") {
    using num_collect::ode::runge_kutta::radau2a5_formula;

    SECTION("static definition") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using formula_type = radau2a5_formula<problem_type>;

        STATIC_REQUIRE(formula_type::stages == 3);
        STATIC_REQUIRE(formula_type::order == 5);
    }

    SECTION("check coefficients") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using formula_type = radau2a5_formula<problem_type>;

        const auto slope_coeffs = formula_type::slope_coeffs();
        const auto time_coeffs = formula_type::time_coeffs();
        const auto update_coeffs = formula_type::update_coeffs();

        CHECK_THAT(slope_coeffs.row(0).sum(),
            Catch::Matchers::WithinRel(time_coeffs(0)));
        CHECK_THAT(slope_coeffs.row(1).sum(),
            Catch::Matchers::WithinRel(time_coeffs(1)));
        CHECK_THAT(slope_coeffs.row(2).sum(),
            Catch::Matchers::WithinRel(time_coeffs(2)));
        CHECK_THAT(update_coeffs.sum(), Catch::Matchers::WithinRel(1.0));
    }

    SECTION("check eigenvalues") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using formula_type = radau2a5_formula<problem_type>;
        auto formula = formula_type(problem_type());

        const auto& solvers = formula.formula_solver().decomposed_solvers();

        num_collect::logging::logger logger;
        int num_real_eigenvalues = 0;
        int num_complex_eigenvalues = 0;
        for (const auto& solver : solvers) {
            std::visit(
                [&logger, &num_real_eigenvalues, &num_complex_eigenvalues](
                    const auto& concrete_solver) {
                    const auto eigenvalue = concrete_solver.eigenvalue();
                    using eigenvalue_type = std::decay_t<decltype(eigenvalue)>;
                    if constexpr (std::is_same_v<eigenvalue_type, double>) {
                        NUM_COLLECT_LOG_DEBUG(
                            logger, "Real eigenvalue: {}", eigenvalue);
                        ++num_real_eigenvalues;
                    } else if constexpr (std::is_same_v<eigenvalue_type,
                                             std::complex<double>>) {
                        NUM_COLLECT_LOG_DEBUG(logger,
                            "Complex eigenvalue: {} + {} i", eigenvalue.real(),
                            eigenvalue.imag());
                        ++num_complex_eigenvalues;
                    } else {
                        FAIL();
                    }
                },
                solver);
        }
        CHECK(num_real_eigenvalues == 1);
        CHECK(num_complex_eigenvalues == 1);
    }

    SECTION("step in one-dimensional problem") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using formula_type = radau2a5_formula<problem_type>;
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        constexpr double prev_var = 1.0;
        double next_var = 0.0;
        double error = 0.0;
        formula.step_with_error_estimate(
            time, step_size, prev_var, next_var, error);

        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference_and_error(
            next_var, error, reference);
    }

    SECTION("step in multi-dimensional problem") {
        using problem_type = num_prob_collect::ode::spring_movement_problem;
        using formula_type = radau2a5_formula<problem_type>;
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-2;
        const Eigen::Vector2d prev_var(1.0, 0.0);
        Eigen::Vector2d next_var;
        Eigen::Vector2d error;
        formula.step_with_error_estimate(
            time, step_size, prev_var, next_var, error);

        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(step_size), std::sin(step_size));
        comparison_approvals::verify_with_reference_and_error(
            next_var, error, reference);
    }
}
