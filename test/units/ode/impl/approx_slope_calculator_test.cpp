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
 * \brief Test of approx_slope_calculator class.
 */
#include "num_collect/ode/impl/approx_slope_calculator.h"

#include <limits>

#include <Eigen/Core>
#include <Eigen/LU>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_prob_collect/ode/changing_mass_exponential_problem.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/implicit_kaps_problem.h"
#include "num_prob_collect/ode/kaps_problem.h"

TEST_CASE("num_collect::ode::impl::approx_slope_calculator") {
    using num_collect::ode::impl::approx_slope_calculator;

    SECTION("handle single-variate problem") {
        SECTION("without mass") {
            using problem_type = num_prob_collect::ode::exponential_problem;
            using calculator_type = approx_slope_calculator<problem_type>;

            calculator_type calculator;
            problem_type problem;
            constexpr double time = 0.123;
            constexpr double variable = 0.456;
            double slope{};
            calculator(problem, time, variable, slope);

            CHECK_THAT(slope, Catch::Matchers::WithinRel(variable));
        }

        SECTION("with non-singular mass") {
            using problem_type =
                num_prob_collect::ode::changing_mass_exponential_problem;
            using calculator_type = approx_slope_calculator<problem_type>;

            calculator_type calculator;
            problem_type problem;
            constexpr double time = 0.123;
            constexpr double variable = 0.456;
            double slope{};
            calculator(problem, time, variable, slope);

            CHECK_THAT(slope, Catch::Matchers::WithinRel(variable));
        }

        SECTION("with singular mass") {
            using problem_type =
                num_prob_collect::ode::changing_mass_exponential_problem;
            using calculator_type = approx_slope_calculator<problem_type>;

            calculator_type calculator;
            problem_type problem;
            constexpr double time = 0.123;
            constexpr double variable =
                -1000.0;  // Mass is zero with this variable.
            double slope{};
            calculator(problem, time, variable, slope);

            CHECK(slope == 0.0);
        }
    }

    SECTION("handle multi-variate problem") {
        SECTION("without mass matrix") {
            using problem_type = num_prob_collect::ode::kaps_problem;
            using calculator_type = approx_slope_calculator<problem_type>;

            calculator_type calculator;
            constexpr double epsilon = 1e-3;
            problem_type problem{epsilon};
            constexpr double time = 0.123;
            const Eigen::Vector2d variable{0.456, 0.789};
            Eigen::Vector2d slope;
            calculator(problem, time, variable, slope);

            CHECK_THAT(slope, eigen_approx(problem.diff_coeff()));
        }

        SECTION("with non-singular mass matrix") {
            using problem_type = num_prob_collect::ode::implicit_kaps_problem;
            using calculator_type = approx_slope_calculator<problem_type>;

            calculator_type calculator;
            constexpr double epsilon = 1e-3;
            problem_type problem{epsilon};
            constexpr double time = 0.123;
            const Eigen::Vector2d variable{0.456, 0.789};
            Eigen::Vector2d slope;
            calculator(problem, time, variable, slope);

            const Eigen::Vector2d expected_slope =
                problem.mass().partialPivLu().solve(problem.diff_coeff());
            constexpr double tol = 1e-4;  // GCC can't solve correctly.
            CHECK_THAT(slope, eigen_approx(expected_slope, tol));
        }

        SECTION("with singular mass matrix") {
            using problem_type = num_prob_collect::ode::implicit_kaps_problem;
            using calculator_type = approx_slope_calculator<problem_type>;

            calculator_type calculator;
            constexpr double epsilon = 0.0;
            problem_type problem{epsilon};
            constexpr double time = 0.123;
            const Eigen::Vector2d variable{0.456, 0.789};
            Eigen::Vector2d slope;
            calculator(problem, time, variable, slope);

            // Mass matrix is singular, so the slope is approximated by least
            // squares solution.
            Eigen::Vector2d residual =
                problem.mass() * slope - problem.diff_coeff();
            CHECK(residual.norm() < problem.diff_coeff().norm());
        }

        SECTION("with a mass matrix and resulted in NaN") {
            using problem_type = num_prob_collect::ode::implicit_kaps_problem;
            using calculator_type = approx_slope_calculator<problem_type>;

            calculator_type calculator;
            constexpr double epsilon = 0.0;
            problem_type problem{epsilon};
            constexpr double time = 0.123;
            const Eigen::Vector2d variable{
                0.456, std::numeric_limits<double>::quiet_NaN()};
            Eigen::Vector2d slope;
            calculator(problem, time, variable, slope);

            CHECK_THAT(slope, eigen_approx(Eigen::Vector2d{{0.0, 0.0}}));
        }
    }
}
