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
 * \brief Test of smoothed_digital_filter_step_size_controller class.
 */
#include "num_collect/ode/smoothed_digital_filter_step_size_controller.h"

#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/functions/pow.h"
#include "num_collect/ode/concepts/step_size_strategy.h"

TEST_CASE("num_collect::ode::smoothed_digital_filter_step_size_strategy") {
    using num_collect::functions::pow;
    using num_collect::ode::smoothed_digital_filter_step_size_strategy;

    SECTION("check concept") {
        using strategy_type =
            smoothed_digital_filter_step_size_strategy<double>;
        STATIC_CHECK(
            num_collect::ode::concepts::step_size_strategy<strategy_type>);
    }

    constexpr num_collect::index_type method_order = 1;
    smoothed_digital_filter_step_size_strategy<double> strategy(method_order);

    constexpr double error_norm_safety_coeff = 0.8;
    strategy.error_norm_safety_coeff(error_norm_safety_coeff);

    const auto filter = [](double x) { return 1.0 + std::atan(x - 1.0); };

    SECTION("calculate an ordinary step size without memory") {
        double step_size = 0.4;
        constexpr double error_norm = 0.1;

        strategy.calc_next(step_size, error_norm);

        constexpr double expected_factor =
            pow(error_norm_safety_coeff / error_norm, 0.5);
        const double expected_step_size = 0.4 * filter(expected_factor);
        constexpr double rel_tol = 1e-5;
        CHECK_THAT(
            step_size, Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        CHECK(strategy.is_previous_step_size_accepted());

        SECTION("calculate the next step size with memory") {
            step_size = 0.4;  // Change the step size for ease of testing.
            constexpr double error_norm2 = 0.2;
            strategy.calc_next(step_size, error_norm2);

            constexpr double expected_factor2 =
                pow(error_norm_safety_coeff / error_norm2, 0.125) *
                pow(error_norm_safety_coeff / error_norm, 0.125) *
                pow(expected_factor, -0.25);
            const double expected_step_size = 0.4 * filter(expected_factor2);
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        }

        SECTION("notify that the previous step size is rejected") {
            strategy.notify_previous_step_size_rejected();

            CHECK_FALSE(strategy.is_previous_step_size_accepted());

            SECTION("calculate the next step size without memory too") {
                double step_size = 0.4;
                constexpr double error_norm = 0.1;

                strategy.calc_next(step_size, error_norm);

                constexpr double expected_factor =
                    pow(error_norm_safety_coeff / error_norm, 0.5);
                const double expected_step_size = 0.4 * filter(expected_factor);
                constexpr double rel_tol = 1e-5;
                CHECK_THAT(step_size,
                    Catch::Matchers::WithinRel(expected_step_size, rel_tol));
                CHECK(strategy.is_previous_step_size_accepted());
            }
        }
    }

    SECTION("calculate a small step size without memory") {
        double step_size = 0.4;
        constexpr double error_norm = 1.0;

        strategy.calc_next(step_size, error_norm);

        constexpr double expected_factor =
            pow(error_norm_safety_coeff / error_norm, 0.5);
        const double expected_step_size = 0.4 * filter(expected_factor);
        constexpr double rel_tol = 1e-5;
        CHECK_THAT(
            step_size, Catch::Matchers::WithinRel(expected_step_size, rel_tol));
        CHECK_FALSE(strategy.is_previous_step_size_accepted());

        SECTION("calculate the next step size without memory too") {
            double step_size = 0.4;
            constexpr double error_norm = 0.1;

            strategy.calc_next(step_size, error_norm);

            constexpr double expected_factor =
                pow(error_norm_safety_coeff / error_norm, 0.5);
            const double expected_step_size = 0.4 * filter(expected_factor);
            constexpr double rel_tol = 1e-5;
            CHECK_THAT(step_size,
                Catch::Matchers::WithinRel(expected_step_size, rel_tol));
            CHECK(strategy.is_previous_step_size_accepted());
        }
    }

    SECTION("handle too small error norms") {
        double step_size = 0.4;
        constexpr double error_norm = 1e-20;

        strategy.calc_next(step_size, error_norm);

        // Too small error norms result in a large factor,
        // and the limited factor is approximately 1 + pi/2.
        constexpr double expected_filtered_factor =
            1.0 + 0.5 * num_collect::pi<double>;
        const double expected_step_size = 0.4 * expected_filtered_factor;
        constexpr double rel_tol = 1e-2;
        CHECK_THAT(
            step_size, Catch::Matchers::WithinRel(expected_step_size, rel_tol));
    }

    SECTION("handle zero error norms") {
        double step_size = 0.4;
        constexpr double error_norm = 0.0;

        strategy.calc_next(step_size, error_norm);

        // Zero error norms result in a large factor,
        // and the limited factor is approximately 1 + pi/2.
        constexpr double expected_filtered_factor =
            1.0 + 0.5 * num_collect::pi<double>;
        const double expected_step_size = 0.4 * expected_filtered_factor;
        constexpr double rel_tol = 1e-2;
        CHECK_THAT(
            step_size, Catch::Matchers::WithinRel(expected_step_size, rel_tol));
    }

    SECTION("handle negative error norms") {
        double step_size = 0.4;
        constexpr double error_norm = -0.1;

        strategy.calc_next(step_size, error_norm);

        // Negative error norms result in no change in step size.
        constexpr double expected_step_size = 0.4;
        constexpr double rel_tol = 1e-5;
        CHECK_THAT(
            step_size, Catch::Matchers::WithinRel(expected_step_size, rel_tol));
    }

    SECTION("handle NaN") {
        double step_size = 0.4;
        constexpr double error_norm = std::numeric_limits<double>::quiet_NaN();

        strategy.calc_next(step_size, error_norm);

        // NaN error norms result in no change in step size.
        constexpr double expected_step_size = 0.4;
        constexpr double rel_tol = 1e-5;
        CHECK_THAT(
            step_size, Catch::Matchers::WithinRel(expected_step_size, rel_tol));
    }
}
