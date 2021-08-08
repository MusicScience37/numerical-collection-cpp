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
 * \brief Test of variable class.
 */
#include "num_collect/auto_diff/forward/variable.h"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::forward::variable<Scalar>", "", float, double) {
    using variable_type = num_collect::auto_diff::forward::variable<TestType>;

    SECTION("construct with all arguments") {
        constexpr auto value = static_cast<TestType>(1.234);
        constexpr auto diff = static_cast<TestType>(2.345);
        const auto var = variable_type(value, diff);

        REQUIRE_THAT(var.value(), Catch::Matchers::WithinRel(value));
        REQUIRE_THAT(var.diff(), Catch::Matchers::WithinRel(diff));
    }

    SECTION("construct with one argument") {
        constexpr auto value = static_cast<TestType>(1.234);
        constexpr auto diff = static_cast<TestType>(0.0);
        const auto var = variable_type(value);

        REQUIRE_THAT(var.value(), Catch::Matchers::WithinRel(value));
        REQUIRE_THAT(var.diff(), Catch::Matchers::WithinRel(diff));
    }

    SECTION("construct without arguments") {
        constexpr auto value = static_cast<TestType>(0.0);
        constexpr auto diff = static_cast<TestType>(0.0);
        const auto var = variable_type();

        REQUIRE_THAT(var.value(), Catch::Matchers::WithinRel(value));
        REQUIRE_THAT(var.diff(), Catch::Matchers::WithinRel(diff));
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::forward::create_diff_variable<Scalar>", "", float,
    double) {
    using variable_type = num_collect::auto_diff::forward::variable<TestType>;

    SECTION("create a variable") {
        constexpr auto value = static_cast<TestType>(1.234);
        const auto var =
            num_collect::auto_diff::forward::create_diff_variable<TestType>(
                value);
        REQUIRE_THAT(var.value(), Catch::Matchers::WithinRel(value));
        REQUIRE_THAT(
            var.diff(), Catch::Matchers::WithinRel(static_cast<TestType>(1)));
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::forward::create_diff_variable<Scalar, Vector>", "",
    float, double) {
    SECTION("create a variable with Eigen::VectorXx type") {
        using diff_type = Eigen::Matrix<TestType, Eigen::Dynamic, 1>;
        using variable_type =
            num_collect::auto_diff::forward::variable<TestType, diff_type>;

        constexpr auto value = static_cast<TestType>(1.234);
        constexpr num_collect::index_type size = 3;
        constexpr num_collect::index_type index = 1;
        const auto var =
            num_collect::auto_diff::forward::create_diff_variable<TestType,
                diff_type>(value, size, index);

        REQUIRE_THAT(var.value(), Catch::Matchers::WithinRel(value));
        REQUIRE_THAT(var.diff()(0),
            Catch::Matchers::WithinRel(static_cast<TestType>(0)));
        REQUIRE_THAT(var.diff()(1),
            Catch::Matchers::WithinRel(static_cast<TestType>(1)));
        REQUIRE_THAT(var.diff()(2),
            Catch::Matchers::WithinRel(static_cast<TestType>(0)));
    }
}
