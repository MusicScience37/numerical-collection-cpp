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
 * \brief Test of member_function_iteration_parameter_value class.
 */
#include "num_collect/logging/iterations/member_function_iteration_parameter_value.h"

#include <catch2/catch_test_macros.hpp>

#include "mock_algorithm.h"
#include "num_collect/logging/concepts/iteration_parameter_value.h"
#include "trompeloeil_catch2.h"

TEST_CASE(
    "num_collect::logging::iterations::member_function_iteration_parameter_"
    "value") {
    using num_collect::logging::concepts::iteration_parameter_value;
    using num_collect::logging::iterations::
        member_function_iteration_parameter_value;
    using num_collect_test::logging::iterations::mock_algorithm;

    SECTION("get values") {
        using value_type = int;
        using return_type = value_type;
        using algorithm_type = mock_algorithm<value_type, return_type>;

        value_type value = 123;
        algorithm_type algorithm{0};
        ALLOW_CALL(algorithm, get_impl()).RETURN(value);

        using parameter_value_type =
            member_function_iteration_parameter_value<algorithm_type,
                value_type, decltype(&algorithm_type::get)>;
        STATIC_REQUIRE(iteration_parameter_value<parameter_value_type,
            algorithm_type, value_type>);

        const parameter_value_type param_value{&algorithm_type::get};
        CHECK(param_value.get(&algorithm) == 123);

        CHECK_THROWS((void)param_value.get());
    }

    SECTION("get the reference of a value") {
        using value_type = int;
        using return_type = const value_type&;
        using algorithm_type = mock_algorithm<value_type, return_type>;

        value_type value = 123;
        algorithm_type algorithm{0};
        ALLOW_CALL(algorithm, get_impl()).RETURN(value);

        using parameter_value_type =
            member_function_iteration_parameter_value<algorithm_type,
                value_type, decltype(&algorithm_type::get)>;
        STATIC_REQUIRE(iteration_parameter_value<parameter_value_type,
            algorithm_type, value_type>);

        const parameter_value_type param_value{&algorithm_type::get};
        CHECK(param_value.get(&algorithm) == 123);

        CHECK_THROWS((void)param_value.get());
    }
}
