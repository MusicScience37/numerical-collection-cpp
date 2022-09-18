/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of member_variable_iteration_parameter_value class.
 */
#include "num_collect/logging/iterations/member_variable_iteration_parameter_value.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "mock_algorithm.h"
#include "num_collect/logging/concepts/iteration_parameter_value.h"  // IWYU pragma: keep

TEST_CASE(
    "num_collect::logging::iterations::member_variable_iteration_parameter_"
    "value") {
    using num_collect::logging::concepts::iteration_parameter_value;
    using num_collect::logging::iterations::
        member_variable_iteration_parameter_value;
    using num_collect_test::logging::iterations::mock_algorithm;

    SECTION("check of concept") {
        using algorithm_type = mock_algorithm<int, int>;
        using value_type = int;
        STATIC_REQUIRE(
            iteration_parameter_value<member_variable_iteration_parameter_value<
                                          algorithm_type, value_type>,
                algorithm_type, value_type>);
    }

    SECTION("get values") {
        using algorithm_type = mock_algorithm<int, int>;
        using value_type = int;
        using parameter_value_type =
            member_variable_iteration_parameter_value<algorithm_type,
                value_type>;

        algorithm_type algorithm{123};  // NOLINT
        const parameter_value_type param_value{&algorithm_type::value1};
        CHECK(param_value.get(&algorithm) == 123);  // NOLINT

        CHECK_THROWS((void)param_value.get());
    }
}
