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
 * \brief Test of newton_raphson class.
 */
#include "num_collect/roots/newton_raphson.h"

#include <cmath>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_prob_collect/roots/cubic_root_test_function.h"
#include "num_prob_collect/roots/double_cubic_test_function.h"

TEST_CASE("num_collect::roots::newton_raphson<cubic_root_test_function>") {
    using function_type = num_prob_collect::roots::cubic_root_test_function;
    using finder_type = num_collect::roots::newton_raphson<function_type>;

    SECTION("initialize") {
        constexpr double target = 3.0;
        auto finder = finder_type(function_type(target));
        constexpr double init_var = 2.0;
        finder.init(init_var);
        REQUIRE_THAT(finder.variable(), Catch::Matchers::WithinRel(init_var));
        constexpr double value = 5.0;
        REQUIRE_THAT(finder.value(), Catch::Matchers::WithinRel(value));
        constexpr double jacobian = 12.0;
        REQUIRE_THAT(finder.jacobian(), Catch::Matchers::WithinRel(jacobian));
        REQUIRE(finder.iterations() == 0);
        REQUIRE(finder.evaluations() == 1);
        REQUIRE(std::isinf(finder.last_change()));
        REQUIRE(finder.last_change() > 0.0);
        REQUIRE_THAT(
            finder.value_norm(), Catch::Matchers::WithinRel(std::abs(value)));
    }

    SECTION("iterate once") {
        constexpr double target = 3.0;
        auto finder = finder_type(function_type(target));
        constexpr double init_var = 2.0;
        finder.init(init_var);
        REQUIRE_NOTHROW(finder.iterate());
        REQUIRE(finder.variable() != init_var);  // NOLINT
        REQUIRE(finder.iterations() == 1);
        REQUIRE(finder.evaluations() > 1);
    }

    SECTION("solve") {
        constexpr double target = 3.0;
        auto finder = finder_type(function_type(target));
        constexpr double init_var = 2.0;
        finder.init(init_var);
        REQUIRE_NOTHROW(finder.solve());
        const double solution = std::cbrt(target);
        constexpr double tol = 1e-4;
        REQUIRE_THAT(
            finder.variable(), Catch::Matchers::WithinRel(solution, tol));
        REQUIRE(finder.iterations() > 1);
    }
}

TEST_CASE("num_collect::roots::newton_raphson<double_cubic_test_function>") {
    using function_type = num_prob_collect::roots::double_cubic_test_function;
    using finder_type = num_collect::roots::newton_raphson<function_type>;

    SECTION("initialize") {
        auto finder = finder_type(function_type());
        const auto init_var = Eigen::Vector2d(1.0, 1.0);
        finder.init(init_var);
        REQUIRE_THAT(finder.variable(), eigen_approx(init_var));
        const auto value = Eigen::Vector2d(-45.0, -7.0);
        REQUIRE_THAT(finder.value(), eigen_approx(value));
        const auto jacobian =
            (Eigen::Matrix2d() << 6.0, -3.0, 3.0, 0.0).finished();
        REQUIRE_THAT(finder.jacobian(), eigen_approx(jacobian));
        REQUIRE(finder.iterations() == 0);
        REQUIRE(finder.evaluations() == 1);
        REQUIRE(std::isinf(finder.last_change()));
        REQUIRE(finder.last_change() > 0.0);
        REQUIRE_THAT(
            finder.value_norm(), Catch::Matchers::WithinRel(value.norm()));
    }

    SECTION("iterate once") {
        auto finder = finder_type(function_type());
        const auto init_var = Eigen::Vector2d(1.0, 1.0);
        finder.init(init_var);
        REQUIRE_NOTHROW(finder.iterate());
        REQUIRE(finder.variable()(0) != init_var(0));  // NOLINT
        REQUIRE(finder.variable()(1) != init_var(1));  // NOLINT
        REQUIRE(finder.iterations() == 1);
        REQUIRE(finder.evaluations() > 1);
    }

    SECTION("solve") {
        auto finder = finder_type(function_type());
        const auto init_var = Eigen::Vector2d(1.0, 1.0);
        finder.init(init_var);
        REQUIRE_NOTHROW(finder.solve());
        const auto solution = Eigen::Vector2d(3.0, 2.0);
        constexpr double tol = 1e-4;
        REQUIRE_THAT(finder.variable(), eigen_approx(solution, tol));
        REQUIRE(finder.iterations() > 1);
    }
}
