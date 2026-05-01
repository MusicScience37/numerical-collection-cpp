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
 * \brief Test of memory_step_size_controller class.
 */
#include "num_collect/ode/memory_step_size_controller.h"

#include <cmath>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/index_type.h"
#include "num_collect/ode/concepts/step_size_strategy.h"

TEST_CASE("num_collect::ode::memory_step_size_strategy") {
    using num_collect::ode::memory_step_size_strategy;

    SECTION("check concept") {
        using strategy_type = memory_step_size_strategy<double>;
        STATIC_CHECK(
            num_collect::ode::concepts::step_size_strategy<strategy_type>);
    }

    SECTION("calculate the next step size without memory") {
        constexpr num_collect::index_type method_order = 4;
        memory_step_size_strategy<double> strategy(method_order);

        constexpr double step_size_factor_safety_coeff = 0.8;
        constexpr double max_step_size_factor = 2.0;
        constexpr double min_step_size_factor = 0.1;
        strategy.step_size_factor_safety_coeff(step_size_factor_safety_coeff);
        strategy.max_step_size_factor(max_step_size_factor);
        strategy.min_step_size_factor(min_step_size_factor);

        SECTION("use a factor without change") {
            double step_size = 0.4;
            constexpr double error_norm = 0.1;

            strategy.calc_next(step_size, error_norm);

            constexpr double expected_step_size = 0.5071658216;
            constexpr double rel_tol = 1e-5;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        }

        SECTION("use a factor slightly lower than the maximum") {
            double step_size = 0.4;
            constexpr double error_norm = 0.011;
            // Factor is 1.9715.

            strategy.calc_next(step_size, error_norm);

            constexpr double expected_step_size = 0.7886266359;
            constexpr double rel_tol = 1e-5;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        }

        SECTION("use a factor limited by the maximum") {
            double step_size = 0.4;
            constexpr double error_norm = 0.01;
            // Factor is 2.0095.

            strategy.calc_next(step_size, error_norm);

            constexpr double expected_step_size = 0.8;
            constexpr double rel_tol = 1e-5;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        }

        SECTION(
            "use a factor without change when the previous step size is "
            "rejected") {
            double step_size = 0.4;
            constexpr double error_norm = 0.4;
            // Factor is 0.96.

            strategy.notify_previous_step_size_rejected();
            strategy.calc_next(step_size, error_norm);

            constexpr double expected_step_size = 0.3843598189;
            constexpr double rel_tol = 1e-5;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        }

        SECTION(
            "use a factor limited by the maximum when the previous step size "
            "is rejected") {
            double step_size = 0.4;
            constexpr double error_norm = 0.3;
            // Factor is 1.0178.

            strategy.notify_previous_step_size_rejected();
            strategy.calc_next(step_size, error_norm);

            constexpr double expected_step_size = 0.4;
            constexpr double rel_tol = 1e-5;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        }

        SECTION("handle too small error norms") {
            double step_size = 0.4;
            constexpr double error_norm = 1e-20;

            strategy.calc_next(step_size, error_norm);

            constexpr double expected_step_size = 0.8;
            constexpr double rel_tol = 1e-5;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        }

        SECTION("handle non-finite factors") {
            double step_size = 0.4;
            constexpr double error_norm =
                std::numeric_limits<double>::quiet_NaN();

            strategy.calc_next(step_size, error_norm);

            constexpr double expected_step_size = 0.4;
            constexpr double rel_tol = 1e-5;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        }
    }

    SECTION("calculate the next step size with memory") {
        constexpr num_collect::index_type method_order = 4;
        memory_step_size_strategy<double> strategy(method_order);

        constexpr double step_size_factor_safety_coeff = 0.8;
        constexpr double max_step_size_factor = 2.0;
        constexpr double min_step_size_factor = 0.1;
        strategy.step_size_factor_safety_coeff(step_size_factor_safety_coeff);
        strategy.max_step_size_factor(max_step_size_factor);
        strategy.min_step_size_factor(min_step_size_factor);

        SECTION("apply the factor using memory") {
            // First step.
            double step_size = 0.4;
            double error_norm = 0.9;
            strategy.calc_next(step_size, error_norm);
            double expected_step_size = 0.32681462;
            constexpr double rel_tol = 1e-5;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));

            // Second step.
            step_size = 0.3;  // Change the step size for ease of testing.
            error_norm = 0.3;
            strategy.calc_next(step_size, error_norm);
            expected_step_size = 0.2852807746;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        }

        SECTION(
            "calculate the factor using memory but limit it by the maximum") {
            // First step.
            double step_size = 0.4;
            double error_norm = 0.9;
            strategy.calc_next(step_size, error_norm);
            double expected_step_size = 0.32681462;
            constexpr double rel_tol = 1e-5;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));

            // Second step.
            step_size = 0.3;  // Change the step size for ease of testing.
            error_norm = 0.2;
            strategy.calc_next(step_size, error_norm);
            expected_step_size = 0.3311351188;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        }
    }
}
