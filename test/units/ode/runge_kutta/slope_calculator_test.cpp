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
 * \brief Test of slope_calculator class.
 */
#include "num_collect/ode/runge_kutta/slope_calculator.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/ode/problems/linear_first_order_dae_problem.h"
#include "num_collect/util/vector.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/implicit_exponential_problem.h"
#include "num_prob_collect/ode/implicit_kaps_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::runge_kutta::slope_calculator") {
    SECTION("calculate for a single variate problem without mass") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using calculator_type =
            num_collect::ode::runge_kutta::slope_calculator<problem_type>;

        problem_type problem;
        constexpr double time = 0.123;
        constexpr double variable = 2.345;
        double slope{0.0};
        calculator_type calculator;
        REQUIRE_NOTHROW(calculator(problem, time, variable, slope));

        constexpr double expected_slope = variable;
        CHECK_THAT(slope, Catch::Matchers::WithinRel(expected_slope));
    }

    SECTION("calculate for a single variate problem with mass") {
        using problem_type =
            num_prob_collect::ode::implicit_exponential_problem;
        using calculator_type =
            num_collect::ode::runge_kutta::slope_calculator<problem_type>;

        problem_type problem;
        constexpr double time = 0.123;
        constexpr double variable = 2.345;
        double slope{0.0};
        calculator_type calculator;
        REQUIRE_NOTHROW(calculator(problem, time, variable, slope));

        constexpr double expected_slope = variable;
        CHECK_THAT(slope, Catch::Matchers::WithinRel(expected_slope));
    }

    SECTION("calculate for a multi variate problem without mass") {
        using problem_type = num_prob_collect::ode::spring_movement_problem;
        using calculator_type =
            num_collect::ode::runge_kutta::slope_calculator<problem_type>;

        problem_type problem;
        constexpr double time = 0.123;
        const Eigen::Vector2d variable(2.345, 3.456);
        Eigen::Vector2d slope;
        calculator_type calculator;
        REQUIRE_NOTHROW(calculator(problem, time, variable, slope));

        const Eigen::Vector2d expected_slope(-3.456, 2.345);
        CHECK_THAT(slope, eigen_approx(expected_slope));
    }

    SECTION("calculate for a multi variate problem with a dense mass matrix") {
        using problem_type = num_prob_collect::ode::implicit_kaps_problem;
        using calculator_type =
            num_collect::ode::runge_kutta::slope_calculator<problem_type>;

        constexpr double epsilon = 0.1;
        problem_type problem{epsilon};
        constexpr double time = 0.123;
        const Eigen::Vector2d variable(2.345, 3.456);
        Eigen::Vector2d slope;
        calculator_type calculator;
        REQUIRE_NOTHROW(calculator(problem, time, variable, slope));

        const Eigen::MatrixXd mass = problem.mass();
        const Eigen::Vector2d expected_slope =
            mass.partialPivLu().solve(problem.diff_coeff());
        CHECK_THAT(slope, eigen_approx(expected_slope));
    }

    SECTION("calculate for a multi variate problem with a sparse mass matrix") {
        using vector_type = Eigen::VectorXd;
        using matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
        using problem_type =
            num_collect::ode::problems::linear_first_order_dae_problem<
                vector_type, matrix_type>;
        using calculator_type =
            num_collect::ode::runge_kutta::slope_calculator<problem_type>;

        // Create diagonal matrices for ease of testing.
        num_collect::util::vector<Eigen::Triplet<double>> mass_triplets;
        num_collect::util::vector<Eigen::Triplet<double>> stiffness_triplets;
        mass_triplets.emplace_back(0, 0, 2.0);
        mass_triplets.emplace_back(1, 1, 3.0);
        stiffness_triplets.emplace_back(0, 0, -1.0);
        stiffness_triplets.emplace_back(1, 1, -1.0);
        matrix_type mass(2, 2);
        mass.setFromTriplets(mass_triplets.begin(), mass_triplets.end());
        matrix_type stiffness(2, 2);
        stiffness.setFromTriplets(
            stiffness_triplets.begin(), stiffness_triplets.end());
        const vector_type load = vector_type::Zero(2);
        problem_type problem{mass, stiffness, load};

        constexpr double time = 0.123;
        const vector_type variable{{2.345, 3.456}};
        vector_type slope(2);
        calculator_type calculator;
        REQUIRE_NOTHROW(calculator(problem, time, variable, slope));

        const vector_type expected_slope{{-0.5 * 2.345, -1.0 / 3.0 * 3.456}};
        CHECK_THAT(slope, eigen_approx(expected_slope));
    }
}
