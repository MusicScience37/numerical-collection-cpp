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
 * \brief Test of any_objective_function class.
 */
#include "num_collect/opt/any_objective_function.h"

#include <type_traits>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

#include "num_collect/opt/concepts/multi_variate_objective_function.h"
#include "num_collect/opt/concepts/single_variate_objective_function.h"

TEST_CASE("num_collect::opt::any_objective_function") {
    using num_collect::opt::any_objective_function;

    SECTION("copy and move") {
        using test_type = any_objective_function<double(Eigen::Vector3d)>;
        STATIC_REQUIRE(std::is_copy_constructible_v<test_type>);
        STATIC_REQUIRE(std::is_move_constructible_v<test_type>);
        STATIC_REQUIRE(std::is_copy_assignable_v<test_type>);
        STATIC_REQUIRE(std::is_move_assignable_v<test_type>);
    }

    SECTION("satisfy concepts") {
        STATIC_REQUIRE(
            num_collect::opt::concepts::single_variate_objective_function<
                any_objective_function<double(double)>>);
        STATIC_REQUIRE(
            num_collect::opt::concepts::multi_variate_objective_function<
                any_objective_function<double(Eigen::Vector3d)>>);
    }

    SECTION("create an object") {
        const auto function_object = [](const Eigen::Vector3d& x) {
            return x.squaredNorm();
        };
        any_objective_function<double(Eigen::Vector3d)> function{
            function_object};

        const Eigen::Vector3d variable{{1.0, 2.0, 3.0}};
        REQUIRE_NOTHROW(function.evaluate_on(variable));

        CHECK(function.value() == 14.0);
    }

    SECTION("set a function object after construction") {
        any_objective_function<double(Eigen::Vector3d)> function;

        const auto function_object = [](const Eigen::Vector3d& x) {
            return x.squaredNorm();
        };
        function = function_object;

        const Eigen::Vector3d variable{{1.0, 2.0, 3.0}};
        REQUIRE_NOTHROW(function.evaluate_on(variable));

        CHECK(function.value() == 14.0);
    }
}
