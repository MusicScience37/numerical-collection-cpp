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
 * \brief Test of error_tolerances class.
 */
#include "num_collect/ode/error_tolerances.h"

#include <string>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <fmt/format.h>

TEST_CASE("num_collect::ode::error_tolerances<vector>") {
    using num_collect::ode::error_tolerances;

    SECTION("default constructor") {
        using variable_type = Eigen::Vector2d;

        const variable_type variable{{0.0, -3.0}};
        const auto tolerances = error_tolerances<variable_type>();

        const auto error1 = variable_type{{1e-6, 0.0}};
        const double expected_norm1 = std::sqrt(0.5);
        CHECK_THAT(tolerances.calc_norm(variable, error1),
            Catch::Matchers::WithinRel(expected_norm1));

        const auto error2 = variable_type{{0.0, 1e-6}};
        const double expected_norm2 = std::sqrt(1.0 / 32.0);
        CHECK_THAT(tolerances.calc_norm(variable, error2),
            Catch::Matchers::WithinRel(expected_norm2));
    }

    SECTION("set tolerances") {
        using variable_type = Eigen::VectorXd;

        const variable_type variable{{0.0, -2.0, 1.0}};
        const variable_type tol_rel_error{{1e-2, 1e-3, 1e-4}};
        const variable_type tol_abs_error{{2e-2, 2e-3, 1e-4}};
        const auto tolerances = error_tolerances<variable_type>()
                                    .tol_rel_error(tol_rel_error)
                                    .tol_abs_error(tol_abs_error);

        const auto error1 = variable_type{{1e-2, 4e-3, 4e-4}};
        const double expected_norm1 = std::sqrt((0.25 + 1.0 + 4.0) / 3.0);
        CHECK_THAT(tolerances.calc_norm(variable, error1),
            Catch::Matchers::WithinRel(expected_norm1));
    }

    SECTION("check tolerances") {
        using variable_type = Eigen::VectorXd;

        const variable_type variable{{0.0, -2.0, 1.0}};
        const variable_type tol_rel_error{{1e-2, 1e-3, 1e-4}};
        const variable_type tol_abs_error{{2e-2, 2e-3, 1e-4}};
        const auto tolerances = error_tolerances<variable_type>()
                                    .tol_rel_error(tol_rel_error)
                                    .tol_abs_error(tol_abs_error);

        const auto error1 = variable_type{{2e-2, 4e-3, 2e-4}};
        CHECK(tolerances.check(variable, error1));

        const auto error2 = variable_type{{2.1e-2, 4e-3, 2e-4}};
        CHECK_FALSE(tolerances.check(variable, error2));

        const auto error3 = variable_type{{2e-2, 4.1e-3, 2e-4}};
        CHECK_FALSE(tolerances.check(variable, error3));

        const auto error4 = variable_type{{2e-2, 4e-3, 2.1e-4}};
        CHECK_FALSE(tolerances.check(variable, error4));
    }

    SECTION("set relative error to a scalar and absolute error to a vector") {
        using variable_type = Eigen::VectorXd;
        using scalar_type = double;

        const variable_type variable{{0.0, -2.0, 1.0}};
        const scalar_type tol_rel_error{1e-2};
        const variable_type tol_abs_error{{2e-2, 2e-2, 1e-2}};
        const auto tolerances = error_tolerances<variable_type>()
                                    .tol_rel_error(tol_rel_error)
                                    .tol_abs_error(tol_abs_error);

        const auto error1 = variable_type{{2e-2, 4e-2, 2e-2}};
        CHECK(tolerances.check(variable, error1));
        CHECK_THAT(tolerances.calc_norm(variable, error1),
            Catch::Matchers::WithinRel(1.0));

        const auto error2 = variable_type{{2e-2, 4.1e-2, 2e-2}};
        CHECK_FALSE(tolerances.check(variable, error2));
    }

    SECTION("set relative error to a vector and absolute error to a scalar") {
        using variable_type = Eigen::VectorXd;
        using scalar_type = double;

        const variable_type variable{{0.0, -0.5, 1.0}};
        const variable_type tol_rel_error{{1e-2, 2e-2, 3e-2}};
        const scalar_type tol_abs_error{1e-2};
        const auto tolerances = error_tolerances<variable_type>()
                                    .tol_rel_error(tol_rel_error)
                                    .tol_abs_error(tol_abs_error);

        const auto error1 = variable_type{{1e-2, 2e-2, 4e-2}};
        CHECK(tolerances.check(variable, error1));
        CHECK_THAT(tolerances.calc_norm(variable, error1),
            Catch::Matchers::WithinRel(1.0));

        const auto error2 = variable_type{{1e-2, 2e-2, 4.1e-2}};
        CHECK_FALSE(tolerances.check(variable, error2));
    }
}

TEST_CASE("num_collect::ode::error_tolerances<scalar>") {
    using num_collect::ode::error_tolerances;

    SECTION("default constructor") {
        const auto tolerances = error_tolerances<double>();

        CHECK_THAT(tolerances.calc_norm(0.0, 1e-6),  // NOLINT
            Catch::Matchers::WithinRel(1.0));        // NOLINT

        CHECK_THAT(tolerances.calc_norm(1.5, 1e-6),  // NOLINT
            Catch::Matchers::WithinRel(0.4));        // NOLINT
    }

    SECTION("set tolerances") {
        const auto tolerances = error_tolerances<double>()
                                    .tol_rel_error(1e-2)   // NOLINT
                                    .tol_abs_error(1e-3);  // NOLINT

        CHECK_THAT(tolerances.calc_norm(0.0, 1e-4),  // NOLINT
            Catch::Matchers::WithinRel(0.1));        // NOLINT

        CHECK_THAT(tolerances.calc_norm(0.3, 2e-3),  // NOLINT
            Catch::Matchers::WithinRel(0.5));        // NOLINT
    }

    SECTION("check tolerances") {
        const auto tolerances = error_tolerances<double>()
                                    .tol_rel_error(1e-2)   // NOLINT
                                    .tol_abs_error(1e-3);  // NOLINT

        CHECK(tolerances.check(0.0, 0.9e-3));        // NOLINT
        CHECK(tolerances.check(0.0, 1.0e-3));        // NOLINT
        CHECK_FALSE(tolerances.check(0.0, 1.1e-3));  // NOLINT

        CHECK(tolerances.check(0.1, 1.9e-3));        // NOLINT
        CHECK(tolerances.check(0.1, 2.0e-3));        // NOLINT
        CHECK_FALSE(tolerances.check(0.1, 2.1e-3));  // NOLINT
    }
}
