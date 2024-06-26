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
 * \brief Test of basic_step_size_controller class.
 */
#include "num_collect/ode/basic_step_size_controller.h"

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

TEST_CASE("num_collect::ode::basic_step_size_controller") {
    using num_collect::ode::basic_step_size_controller;
    using num_collect::ode::error_tolerances;
    using num_collect::ode::step_size_limits;
    using num_collect::ode::runge_kutta::rkf45_formula;
    using num_prob_collect::ode::exponential_problem;
    using num_prob_collect::ode::spring_movement_problem;

    SECTION("check concept") {
        using problem_type = spring_movement_problem;
        using formula_type = rkf45_formula<problem_type>;
        using controller_type = basic_step_size_controller<formula_type>;
        STATIC_CHECK(
            num_collect::ode::concepts::step_size_controller<controller_type>);
    }

    SECTION("check when error is large") {
        using problem_type = spring_movement_problem;
        using formula_type = rkf45_formula<problem_type>;
        using controller_type = basic_step_size_controller<formula_type>;

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

        constexpr double step_size_factor_safety_coeff = 0.8;
        controller.step_size_factor_safety_coeff(step_size_factor_safety_coeff);

        constexpr double max_step_size_factor = 5.0;
        controller.max_step_size_factor(max_step_size_factor);

        SECTION("step size in limit") {
            constexpr double reduction_rate = 0.5;
            controller.reduction_rate(reduction_rate);

            double step_size = 0.5;  // NOLINT
            const auto error = Eigen::Vector2d{{2e-2, 2e-2}};
            CHECK_FALSE(
                controller.check_and_calc_next(step_size, variable, error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.25));  // NOLINT}
        }

        SECTION("step size under limit") {
            constexpr double reduction_rate = 0.1;
            controller.reduction_rate(reduction_rate);

            double step_size = 0.5;  // NOLINT
            const auto error = Eigen::Vector2d{{2e-2, 2e-2}};
            CHECK_FALSE(
                controller.check_and_calc_next(step_size, variable, error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.2));  // NOLINT}
        }

        SECTION("already small step size") {
            double step_size = 0.1;  // NOLINT
            const auto error = Eigen::Vector2d{{2e-2, 2e-2}};
            CHECK(controller.check_and_calc_next(step_size, variable, error));
        }
    }

    SECTION("check when error satisfies tolerances") {
        using problem_type = exponential_problem;
        using formula_type = rkf45_formula<problem_type>;
        using controller_type = basic_step_size_controller<formula_type>;

        controller_type controller;

        constexpr double variable = 0.1;
        controller.init();

        const auto limits =
            step_size_limits<double>().upper_limit(1.0).lower_limit(0.2);
        controller.limits(limits);

        const auto tolerances =
            error_tolerances<double>().tol_rel_error(1e-2).tol_abs_error(1e-3);
        controller.tolerances(tolerances);

        constexpr double step_size_factor_safety_coeff = 0.8;
        controller.step_size_factor_safety_coeff(step_size_factor_safety_coeff);

        constexpr double max_step_size_factor = 5.0;
        controller.max_step_size_factor(max_step_size_factor);

        constexpr double reduction_rate = 0.5;
        controller.reduction_rate(reduction_rate);

        SECTION("step size in limit") {
            double step_size = 0.4;  // NOLINT
            const double error = 2e-3 / std::pow(2.0, 5);
            CHECK(controller.check_and_calc_next(step_size, variable, error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.64));  // NOLINT}
        }

        SECTION("factor too large") {
            double step_size = 0.1;  // NOLINT
            const double error = 2e-3 / std::pow(10.0, 5);
            CHECK(controller.check_and_calc_next(step_size, variable, error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.5));  // NOLINT}
        }

        SECTION("no error resulting in invalid factor") {
            double step_size = 0.1;  // NOLINT
            const double error = 0.0;
            CHECK(controller.check_and_calc_next(step_size, variable, error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(0.5));  // NOLINT}
        }

        SECTION("step size too large") {
            double step_size = 0.5;  // NOLINT
            const double error = 2e-3 / std::pow(10.0, 5);
            CHECK(controller.check_and_calc_next(step_size, variable, error));
            CHECK_THAT(step_size, Catch::Matchers::WithinRel(1.0));  // NOLINT}
        }
    }
}
