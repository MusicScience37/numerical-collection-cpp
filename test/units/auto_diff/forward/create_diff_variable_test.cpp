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
 * \brief Test of create_diff_variable function.
 */
#include "num_collect/auto_diff/forward/create_diff_variable.h"

#include <Eigen/Core>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::forward::create_diff_variable<Scalar>", "", float,
    double) {
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

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::forward::create_diff_variable_vector", "", float,
    double) {
    SECTION("create a variable") {
        using value_type = TestType;
        using value_vector_type = Eigen::Matrix<TestType, Eigen::Dynamic, 1>;
        using variable_vector_type =
            num_collect::auto_diff::forward::variable_vector_type<
                value_vector_type>;

        const value_vector_type value_vec =
            (value_vector_type(3) << 1.234, 2.345, 3.456).finished();
        const variable_vector_type var =
            num_collect::auto_diff::forward::create_diff_variable_vector(
                value_vec);

        REQUIRE(var.rows() == 3);
        REQUIRE(var.cols() == 1);
        for (num_collect::index_type i = 0; i < 3; ++i) {
            INFO("i = " << i);
            REQUIRE_THAT(
                var(i).value(), Catch::Matchers::WithinRel(value_vec(i)));
            REQUIRE(var(i).diff().rows() == 3);
            REQUIRE(var(i).diff().cols() == 1);
            for (num_collect::index_type j = 0; j < 3; ++j) {
                REQUIRE_THAT(var(i).diff()(j),
                    Catch::Matchers::WithinRel(
                        static_cast<value_type>((i == j) ? 1 : 0)));
            }
        }
    }
}
