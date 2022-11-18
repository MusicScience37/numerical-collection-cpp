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
 * \brief Test of mathematical functions for variable class.
 */
#include "num_collect/auto_diff/forward/variable_math.h"

#include <ostream>
#include <string>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/auto_diff/forward/create_diff_variable.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::auto_diff::forward::exp", "", float, double) {
    using value_type = TestType;
    using variable_type = num_collect::auto_diff::forward::variable<value_type>;
    using num_collect::auto_diff::forward::create_diff_variable;

    SECTION("process an argument with a differential coefficient") {
        const variable_type var =
            create_diff_variable(static_cast<value_type>(1.234));

        const variable_type res = exp(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::exp(var.value())));
        REQUIRE(res.has_diff());

        const value_type& coeff = res.diff();
        REQUIRE_THAT(coeff, Catch::Matchers::WithinRel(std::exp(var.value())));
    }

    SECTION("process an argument without a differential coefficient") {
        const auto var = variable_type(1.234);

        const variable_type res = exp(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::exp(var.value())));
        REQUIRE_FALSE(res.has_diff());
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::auto_diff::forward::log", "", float, double) {
    using value_type = TestType;
    using variable_type = num_collect::auto_diff::forward::variable<value_type>;
    using num_collect::auto_diff::forward::create_diff_variable;

    SECTION("process an argument with a differential coefficient") {
        const variable_type var =
            create_diff_variable(static_cast<value_type>(1.234));

        const variable_type res = log(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::log(var.value())));
        REQUIRE(res.has_diff());

        const value_type& coeff = res.diff();
        REQUIRE_THAT(coeff, Catch::Matchers::WithinRel(1 / var.value()));
    }

    SECTION("process an argument without a differential coefficient") {
        const auto var = variable_type(1.234);

        const variable_type res = log(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::log(var.value())));
        REQUIRE_FALSE(res.has_diff());
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::auto_diff::forward::sqrt", "", float, double) {
    using value_type = TestType;
    using variable_type = num_collect::auto_diff::forward::variable<value_type>;
    using num_collect::auto_diff::forward::create_diff_variable;

    SECTION("process an argument with a differential coefficient") {
        const variable_type var =
            create_diff_variable(static_cast<value_type>(1.234));

        const variable_type res = sqrt(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::sqrt(var.value())));
        REQUIRE(res.has_diff());

        const value_type& coeff = res.diff();
        REQUIRE_THAT(coeff,
            Catch::Matchers::WithinRel(
                1 / static_cast<value_type>(2) / std::sqrt(var.value())));
    }

    SECTION("process an argument without a differential coefficient") {
        const auto var = variable_type(1.234);

        const variable_type res = sqrt(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::sqrt(var.value())));
        REQUIRE_FALSE(res.has_diff());
    }
}
