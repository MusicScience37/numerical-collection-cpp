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
 * \brief Test of pi_step_size_controller class.
 */
#include "num_collect/ode/pi_step_size_controller.h"

#include <cmath>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/ode/concepts/step_size_controller.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_collect/ode/step_size_limits.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::pi_step_size_controller") {
    using num_collect::ode::error_tolerances;
    using num_collect::ode::pi_step_size_controller;
    using num_collect::ode::step_size_limits;
    using num_collect::ode::runge_kutta::rkf45_formula;
    using num_prob_collect::ode::exponential_problem;
    using num_prob_collect::ode::spring_movement_problem;

    SECTION("check assumption") { CHECK(std::pow(0.0, 0.0) == 1.0); }

    SECTION("check concept") {
        using problem_type = spring_movement_problem;
        using formula_type = rkf45_formula<problem_type>;
        using controller_type = pi_step_size_controller<formula_type>;
        STATIC_CHECK(
            num_collect::ode::concepts::step_size_controller<controller_type>);
    }

    SECTION("check when error is large") {
        using problem_type = spring_movement_problem;
        using formula_type = rkf45_formula<problem_type>;
        using controller_type = pi_step_size_controller<formula_type>;

        controller_type controller;

        const auto variable = Eigen::Vector2d{{0.0, 1.0}};
        controller.init();

        const auto limits =
            step_size_limits<double>().upper_limit(1.0).lower_limit(0.2);
        controller.limits(limits);

        const auto tolerances =
            error_tolerances<Eigen::Vector2d>()
                .tol_rel_error(Eigen::Vector2d{{1e-2, 1e-2}})
                .tol_abs_error(Eigen::Vector2d{{1e-2, 1e-2}});
        controller.tolerances(tolerances);

        SECTION("step size in limit") {
            constexpr double reduction_rate = 0.5;
            controller.reduction_rate(reduction_rate);

            double step_size = 0.5;
            const auto error = Eigen::Vector2d{{2e-2, 2e-2}};
            CHECK_FALSE(
                controller.check_and_calc_next(step_size, variable, error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.25));
        }

        SECTION("step size under limit") {
            constexpr double reduction_rate = 0.1;
            controller.reduction_rate(reduction_rate);

            double step_size = 0.5;
            const auto error = Eigen::Vector2d{{2e-2, 2e-2}};
            CHECK_FALSE(
                controller.check_and_calc_next(step_size, variable, error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.2));
        }

        SECTION("already small step size") {
            double step_size = 0.1;
            const auto error = Eigen::Vector2d{{2e-2, 2e-2}};
            CHECK(controller.check_and_calc_next(step_size, variable, error));
        }
    }

    SECTION("check when error satisfies tolerances") {
        using problem_type = exponential_problem;
        using formula_type = rkf45_formula<problem_type>;
        using controller_type = pi_step_size_controller<formula_type>;

        controller_type controller;

        constexpr double variable = 0.1;
        controller.init();

        const auto limits =
            step_size_limits<double>().upper_limit(1.0).lower_limit(0.2);
        controller.limits(limits);

        const auto tolerances =
            error_tolerances<double>().tol_rel_error(1e-2).tol_abs_error(1e-3);
        controller.tolerances(tolerances);

        constexpr double reduction_rate = 0.5;
        controller.reduction_rate(reduction_rate);

        constexpr double current_step_error_exponent = 0.5;
        controller.current_step_error_exponent(current_step_error_exponent);

        constexpr double previous_step_error_exponent = 0.25;
        controller.previous_step_error_exponent(previous_step_error_exponent);

        constexpr double step_size_factor_safety_coeff = 0.8;
        controller.step_size_factor_safety_coeff(step_size_factor_safety_coeff);

        constexpr double max_step_size_factor = 5.0;
        controller.max_step_size_factor(max_step_size_factor);

        constexpr double min_step_size_factor = 0.1;
        controller.min_step_size_factor(min_step_size_factor);

        SECTION("step size in limit") {
            double step_size = 0.4;
            const double current_error = 2e-3 / std::pow(2.0, 2);
            CHECK(controller.check_and_calc_next(
                step_size, variable, current_error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.64));
        }

        SECTION("use error in the previous step") {
            double step_size = 0.4;
            const double previous_error = 2e-3 / std::pow(4.0, 4);
            CHECK(controller.check_and_calc_next(
                step_size, variable, previous_error));

            step_size = 0.6;
            const double current_error = 2e-3 / std::pow(2.0, 2);
            CHECK(controller.check_and_calc_next(
                step_size, variable, current_error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.24));
        }

        SECTION("factor too large") {
            double step_size = 0.1;
            const double current_error = 2e-3 / std::pow(10.0, 2);
            CHECK(controller.check_and_calc_next(
                step_size, variable, current_error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.5));
        }

        SECTION("no error resulting in invalid factor") {
            double step_size = 0.5;
            const double current_error = 0.0;
            CHECK(controller.check_and_calc_next(
                step_size, variable, current_error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.5));
        }

        SECTION("factor too large") {
            double step_size = 0.5;
            const double current_error = 2e-3 / std::pow(10.0, 2);
            CHECK(controller.check_and_calc_next(
                step_size, variable, current_error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(1.0));
        }

        SECTION("factor too small") {
            double step_size = 0.4;
            const double previous_error = 2e-3 / std::pow(20.0, 4);
            CHECK(controller.check_and_calc_next(
                step_size, variable, previous_error));

            step_size = 3.0;
            const double current_error = 2e-3 / std::pow(2.0, 2);
            CHECK(controller.check_and_calc_next(
                step_size, variable, current_error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.3));
        }
    }
}
