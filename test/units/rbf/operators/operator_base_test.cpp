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
 * \brief Test of operator_base class.
 */
#include "num_collect/rbf/operators/operator_base.h"

#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/operators/function_value_operator.h"
#include "num_collect/rbf/rbf_polynomial_interpolator.h"

TEST_CASE("num_collect::rbf::operators::operator_base") {
    using num_collect::rbf::global_rbf_polynomial_interpolator;
    using num_collect::rbf::operators::function_value_operator;
    using num_collect::rbf::operators::operator_multiple;
    using num_collect::rbf::operators::operator_negation;
    using num_collect::rbf::operators::operator_sum;

    const Eigen::VectorXd variables = Eigen::VectorXd::LinSpaced(20, 0.0, 1.0);
    const Eigen::VectorXd function_values = variables.array().sin();
    global_rbf_polynomial_interpolator<double(double)> interpolator;
    interpolator.compute(variables, function_values);

    SECTION("multiply by scalar from right") {
        const auto target_operator = function_value_operator{0.456};
        constexpr double scalar = 2.34;

        const auto multiplied = target_operator * scalar;

        STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(multiplied)>,
            operator_multiple<double, function_value_operator<double>>>);
        CHECK_THAT(multiplied.scalar(), Catch::Matchers::WithinRel(scalar));
        CHECK_THAT(multiplied.target_operator().variable(),
            Catch::Matchers::WithinRel(target_operator.variable()));

        const double evaluated = interpolator.evaluate(multiplied);
        const double expected = scalar * interpolator.evaluate(target_operator);
        CHECK_THAT(evaluated, Catch::Matchers::WithinRel(expected));
    }

    SECTION("multiply by scalar from left") {
        const auto target_operator = function_value_operator{0.456};
        constexpr double scalar = 2.34;

        const auto multiplied = scalar * target_operator;

        STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(multiplied)>,
            operator_multiple<double, function_value_operator<double>>>);
        CHECK_THAT(multiplied.scalar(), Catch::Matchers::WithinRel(scalar));
        CHECK_THAT(multiplied.target_operator().variable(),
            Catch::Matchers::WithinRel(target_operator.variable()));

        const double evaluated = interpolator.evaluate(multiplied);
        const double expected = scalar * interpolator.evaluate(target_operator);
        CHECK_THAT(evaluated, Catch::Matchers::WithinRel(expected));
    }

    SECTION("divide by scalar") {
        const auto target_operator = function_value_operator{0.456};
        constexpr double scalar = 2.34;

        const auto divided = target_operator / scalar;

        STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(divided)>,
            operator_multiple<double, function_value_operator<double>>>);
        CHECK_THAT(divided.scalar(), Catch::Matchers::WithinRel(1.0 / scalar));
        CHECK_THAT(divided.target_operator().variable(),
            Catch::Matchers::WithinRel(target_operator.variable()));

        const double evaluated = interpolator.evaluate(divided);
        const double expected =
            (1.0 / scalar) * interpolator.evaluate(target_operator);
        CHECK_THAT(evaluated, Catch::Matchers::WithinRel(expected));
    }

    SECTION("negate") {
        const auto target_operator = function_value_operator{0.456};

        const auto negated = -target_operator;

        STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(negated)>,
            operator_negation<function_value_operator<double>>>);
        CHECK_THAT(negated.target_operator().variable(),
            Catch::Matchers::WithinRel(target_operator.variable()));

        const double evaluated = interpolator.evaluate(negated);
        const double expected = -interpolator.evaluate(target_operator);
        CHECK_THAT(evaluated, Catch::Matchers::WithinRel(expected));
    }

    SECTION("add") {
        const auto left_operator = function_value_operator{0.234};
        const auto right_operator = function_value_operator{0.456};

        const auto sum = left_operator + right_operator;

        STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(sum)>,
            operator_sum<function_value_operator<double>,
                function_value_operator<double>>>);
        CHECK_THAT(sum.left_operator().variable(),
            Catch::Matchers::WithinRel(left_operator.variable()));
        CHECK_THAT(sum.right_operator().variable(),
            Catch::Matchers::WithinRel(right_operator.variable()));

        const double evaluated = interpolator.evaluate(sum);
        const double expected = interpolator.evaluate(left_operator) +
            interpolator.evaluate(right_operator);
        CHECK_THAT(evaluated, Catch::Matchers::WithinRel(expected));
    }

    SECTION("subtract") {
        const auto left_operator = function_value_operator{0.234};
        const auto right_operator = function_value_operator{0.456};

        const auto difference = left_operator - right_operator;

        STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(difference)>,
            operator_sum<function_value_operator<double>,
                operator_negation<function_value_operator<double>>>>);
        CHECK_THAT(difference.left_operator().variable(),
            Catch::Matchers::WithinRel(left_operator.variable()));
        CHECK_THAT(difference.right_operator().target_operator().variable(),
            Catch::Matchers::WithinRel(right_operator.variable()));

        const double evaluated = interpolator.evaluate(difference);
        const double expected = interpolator.evaluate(left_operator) -
            interpolator.evaluate(right_operator);
        CHECK_THAT(evaluated, Catch::Matchers::WithinRel(expected));
    }
}
