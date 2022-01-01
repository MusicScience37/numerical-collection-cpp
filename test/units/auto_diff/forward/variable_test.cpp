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

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "eigen_approx.h"
#include "num_collect/auto_diff/forward/create_diff_variable.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::forward::variable<Scalar>", "", float, double) {
    using variable_type = num_collect::auto_diff::forward::variable<TestType>;

    SECTION("construct with all arguments") {
        constexpr auto value = static_cast<TestType>(1.234);
        constexpr auto diff = static_cast<TestType>(2.345);
        const auto var = variable_type(value, diff);

        REQUIRE_THAT(var.value(), Catch::Matchers::WithinRel(value));
        REQUIRE(var.has_diff());
        REQUIRE_THAT(var.diff(), Catch::Matchers::WithinRel(diff));
    }

    SECTION("construct with one argument") {
        constexpr auto value = static_cast<TestType>(1.234);
        const auto var = variable_type(value);

        REQUIRE_THAT(var.value(), Catch::Matchers::WithinRel(value));
        REQUIRE_FALSE(var.has_diff());
        REQUIRE_THROWS(var.diff());
    }

    SECTION("construct without arguments") {
        constexpr auto value = static_cast<TestType>(0.0);
        const auto var = variable_type();

        REQUIRE_THAT(var.value(), Catch::Matchers::WithinRel(value));
        REQUIRE_FALSE(var.has_diff());
        REQUIRE_THROWS(var.diff());
    }

    SECTION("add a variable") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = variable_type(3.456, -4.567);
        variable_type var = var1;
        var += var2;
        REQUIRE_THAT(var.value(),
            Catch::Matchers::WithinRel(var1.value() + var2.value()));
        REQUIRE_THAT(
            var.diff(), Catch::Matchers::WithinRel(var1.diff() + var2.diff()));
    }

    SECTION("add a value") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = static_cast<TestType>(3.456);
        variable_type var = var1;
        var += var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1.value() + var2));
        REQUIRE_THAT(var.diff(), Catch::Matchers::WithinRel(var1.diff()));
    }

    SECTION("subtract a variable") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = variable_type(3.456, -4.567);
        variable_type var = var1;
        var -= var2;
        REQUIRE_THAT(var.value(),
            Catch::Matchers::WithinRel(var1.value() - var2.value()));
        REQUIRE_THAT(
            var.diff(), Catch::Matchers::WithinRel(var1.diff() - var2.diff()));
    }

    SECTION("subtract a value") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = static_cast<TestType>(3.456);
        variable_type var = var1;
        var -= var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1.value() - var2));
        REQUIRE_THAT(var.diff(), Catch::Matchers::WithinRel(var1.diff()));
    }

    SECTION("multiply a variable") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = variable_type(3.456, -4.567);
        variable_type var = var1;
        var *= var2;
        REQUIRE_THAT(var.value(),
            Catch::Matchers::WithinRel(var1.value() * var2.value()));
        REQUIRE_THAT(var.diff(),
            Catch::Matchers::WithinRel(
                var2.value() * var1.diff() + var1.value() * var2.diff()));
    }

    SECTION("multiply a value") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = static_cast<TestType>(3.456);
        variable_type var = var1;
        var *= var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1.value() * var2));
        REQUIRE_THAT(
            var.diff(), Catch::Matchers::WithinRel(var2 * var1.diff()));
    }

    SECTION("divide by a variable") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = variable_type(3.456, -4.567);
        variable_type var = var1;
        var /= var2;
        REQUIRE_THAT(var.value(),
            Catch::Matchers::WithinRel(var1.value() / var2.value()));
        REQUIRE_THAT(var.diff(),
            Catch::Matchers::WithinRel(
                (var2.value() * var1.diff() - var1.value() * var2.diff()) /
                (var2.value() * var2.value())));
    }

    SECTION("divide by a value") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = static_cast<TestType>(3.456);
        variable_type var = var1;
        var /= var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1.value() / var2));
        REQUIRE_THAT(
            var.diff(), Catch::Matchers::WithinRel(var1.diff() / var2));
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::forward::variable<Scalar> operators", "", float,
    double) {
    using variable_type = num_collect::auto_diff::forward::variable<TestType>;

    SECTION("variable + variable") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = variable_type(3.456, -4.567);
        const auto var = var1 + var2;
        REQUIRE_THAT(var.value(),
            Catch::Matchers::WithinRel(var1.value() + var2.value()));
        REQUIRE_THAT(
            var.diff(), Catch::Matchers::WithinRel(var1.diff() + var2.diff()));
    }

    SECTION("value + variable") {
        const auto var1 = static_cast<TestType>(1.234);
        const auto var2 = variable_type(3.456, -4.567);
        const auto var = var1 + var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1 + var2.value()));
        REQUIRE_THAT(var.diff(), Catch::Matchers::WithinRel(var2.diff()));
    }

    SECTION("variable + value") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = static_cast<TestType>(3.456);
        const auto var = var1 + var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1.value() + var2));
        REQUIRE_THAT(var.diff(), Catch::Matchers::WithinRel(var1.diff()));
    }

    SECTION("variable - variable") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = variable_type(3.456, -4.567);
        const auto var = var1 - var2;
        REQUIRE_THAT(var.value(),
            Catch::Matchers::WithinRel(var1.value() - var2.value()));
        REQUIRE_THAT(
            var.diff(), Catch::Matchers::WithinRel(var1.diff() - var2.diff()));
    }

    SECTION("value - variable") {
        const auto var1 = static_cast<TestType>(1.234);
        const auto var2 = variable_type(3.456, -4.567);
        const auto var = var1 - var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1 - var2.value()));
        REQUIRE_THAT(var.diff(), Catch::Matchers::WithinRel(-var2.diff()));
    }

    SECTION("variable - value") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = static_cast<TestType>(3.456);
        const auto var = var1 - var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1.value() - var2));
        REQUIRE_THAT(var.diff(), Catch::Matchers::WithinRel(var1.diff()));
    }

    SECTION("variable * variable") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = variable_type(3.456, -4.567);
        const auto var = var1 * var2;
        REQUIRE_THAT(var.value(),
            Catch::Matchers::WithinRel(var1.value() * var2.value()));
        REQUIRE_THAT(var.diff(),
            Catch::Matchers::WithinRel(
                var2.value() * var1.diff() + var1.value() * var2.diff()));
    }

    SECTION("value * variable") {
        const auto var1 = static_cast<TestType>(1.234);
        const auto var2 = variable_type(3.456, -4.567);
        const auto var = var1 * var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1 * var2.value()));
        REQUIRE_THAT(
            var.diff(), Catch::Matchers::WithinRel(var1 * var2.diff()));
    }

    SECTION("variable * value") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = static_cast<TestType>(3.456);
        const auto var = var1 * var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1.value() * var2));
        REQUIRE_THAT(
            var.diff(), Catch::Matchers::WithinRel(var2 * var1.diff()));
    }

    SECTION("variable / variable") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = variable_type(3.456, -4.567);
        const auto var = var1 / var2;
        REQUIRE_THAT(var.value(),
            Catch::Matchers::WithinRel(var1.value() / var2.value()));
        REQUIRE_THAT(var.diff(),
            Catch::Matchers::WithinRel(
                (var2.value() * var1.diff() - var1.value() * var2.diff()) /
                (var2.value() * var2.value())));
    }

    SECTION("value / variable") {
        const auto var1 = static_cast<TestType>(1.234);
        const auto var2 = variable_type(3.456, -4.567);
        const auto var = var1 / var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1 / var2.value()));
        REQUIRE_THAT(var.diff(),
            Catch::Matchers::WithinRel(
                (-var1 * var2.diff()) / (var2.value() * var2.value())));
    }

    SECTION("variable / variable") {
        const auto var1 = variable_type(1.234, 2.345);
        const auto var2 = static_cast<TestType>(3.456);
        const auto var = var1 / var2;
        REQUIRE_THAT(
            var.value(), Catch::Matchers::WithinRel(var1.value() / var2));
        REQUIRE_THAT(
            var.diff(), Catch::Matchers::WithinRel(var1.diff() / var2));
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::forward::variable<Scalar, Vector> operations", "",
    float, double) {
    using diff_type = Eigen::Matrix<TestType, 2, 1>;
    using variable_type =
        num_collect::auto_diff::forward::variable<TestType, diff_type>;

    const variable_type left =
        num_collect::auto_diff::forward::create_diff_variable<TestType,
            diff_type>(1.234, 2, 0);
    const variable_type right =
        num_collect::auto_diff::forward::create_diff_variable<TestType,
            diff_type>(2.345, 2, 1);

    SECTION("addition") {
        const variable_type res = left + right;
        REQUIRE_THAT(res.value(),
            Catch::Matchers::WithinRel(left.value() + right.value()));
        REQUIRE_THAT(res.diff(), eigen_approx(left.diff() + right.diff()));
    }

    SECTION("subtraction") {
        const variable_type res = left - right;
        REQUIRE_THAT(res.value(),
            Catch::Matchers::WithinRel(left.value() - right.value()));
        REQUIRE_THAT(res.diff(), eigen_approx(left.diff() - right.diff()));
    }

    SECTION("multiplication") {
        const variable_type res = left * right;
        REQUIRE_THAT(res.value(),
            Catch::Matchers::WithinRel(left.value() * right.value()));
        REQUIRE_THAT(res.diff(),
            eigen_approx(
                right.value() * left.diff() + left.value() * right.diff()));
    }

    SECTION("division") {
        const variable_type res = left / right;
        REQUIRE_THAT(res.value(),
            Catch::Matchers::WithinRel(left.value() / right.value()));
        REQUIRE_THAT(res.diff(),
            eigen_approx(
                (right.value() * left.diff() - left.value() * right.diff()) /
                (right.value() * right.value())));
    }
}

TEST_CASE("Eigen::Matrix<num_collect::auto_diff::forward::variable>") {
    using diff_type = Eigen::Vector2d;
    using variable_type =
        num_collect::auto_diff::forward::variable<double, diff_type>;
    using vector_type = Eigen::Matrix<variable_type, 2, 1>;
    using num_collect::auto_diff::forward::create_diff_variable;

    const auto vec =
        vector_type(create_diff_variable<double, diff_type>(1.234, 2, 0),
            create_diff_variable<double, diff_type>(2.345, 2, 1));

    SECTION("prod") {
        const variable_type res = vec.prod();
        REQUIRE_THAT(res.value(),
            Catch::Matchers::WithinRel(vec(0).value() * vec(1).value()));
        REQUIRE_THAT(res.diff(),
            eigen_approx(diff_type(vec(1).value(), vec(0).value())));
    }
}
