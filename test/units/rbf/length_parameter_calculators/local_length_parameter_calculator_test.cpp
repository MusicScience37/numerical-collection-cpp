/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of local_length_parameter_calculator class.
 */
#include "num_collect/rbf/length_parameter_calculators/local_length_parameter_calculator.h"

#include <vector>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/rbf/concepts/length_parameter_calculator.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"

TEST_CASE("num_collect::rbf::rbfs::local_length_parameter_calculator") {
    using num_collect::rbf::concepts::length_parameter_calculator;
    using num_collect::rbf::distance_functions::euclidean_distance_function;
    using num_collect::rbf::length_parameter_calculators::
        local_length_parameter_calculator;

    SECTION("check of concept") {
        STATIC_REQUIRE(
            length_parameter_calculator<local_length_parameter_calculator<
                euclidean_distance_function<double>>>);
        STATIC_REQUIRE(
            length_parameter_calculator<local_length_parameter_calculator<
                euclidean_distance_function<Eigen::Vector3d>>>);
    }

    SECTION("set scale") {
        using variable_type = double;
        using distance_function_type =
            euclidean_distance_function<variable_type>;
        local_length_parameter_calculator<distance_function_type> calculator;

        constexpr double scale = 1.25;
        calculator.scale(scale);

        CHECK(calculator.scale() == scale);
    }

    SECTION("check whether scale is valid") {
        using variable_type = double;
        using distance_function_type =
            euclidean_distance_function<variable_type>;
        local_length_parameter_calculator<distance_function_type> calculator;

        CHECK_NOTHROW(calculator.scale(0.0001));
        CHECK_THROWS(calculator.scale(0.0));
        CHECK_THROWS(calculator.scale(-0.0001));
    }

    SECTION("compute the length parameter") {
        using variable_type = double;
        using distance_function_type =
            euclidean_distance_function<variable_type>;
        const distance_function_type distance_function;
        local_length_parameter_calculator<distance_function_type> calculator;
        constexpr double scale = 3.0;
        calculator.scale(scale);

        const auto variables = std::vector{0.1, 0.2, 0.4};
        calculator.compute(variables, distance_function);

        CHECK_THAT(
            calculator.length_parameter_at(0), Catch::Matchers::WithinRel(0.3));
        CHECK_THAT(
            calculator.length_parameter_at(1), Catch::Matchers::WithinRel(0.3));
        CHECK_THAT(
            calculator.length_parameter_at(2), Catch::Matchers::WithinRel(0.6));
    }

    SECTION("check number of points for computation") {
        using variable_type = double;
        using distance_function_type =
            euclidean_distance_function<variable_type>;
        const distance_function_type distance_function;
        local_length_parameter_calculator<distance_function_type> calculator;

        CHECK_NOTHROW(calculator.compute(std::vector{1.0}, distance_function));
        CHECK_THROWS(
            calculator.compute(std::vector<double>{}, distance_function));
    }
}
