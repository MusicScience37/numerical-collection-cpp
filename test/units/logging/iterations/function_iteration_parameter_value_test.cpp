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
 * \brief Test of function_iteration_parameter_value class.
 */
#include "num_collect/logging/iterations/function_iteration_parameter_value.h"

#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/logging/concepts/iteration_parameter_value.h"  // IWYU pragma: keep

TEST_CASE(
    "num_collect::logging::iterations::function_iteration_parameter_value") {
    using num_collect::logging::concepts::iteration_parameter_value;
    using num_collect::logging::iterations::function_iteration_parameter_value;

    SECTION("get values") {
        using algorithm_type = void;
        using value_type = int;

        value_type value = 123;  // NOLINT
        auto function = [&value]() -> value_type { return value; };

        using parameter_value_type =
            function_iteration_parameter_value<algorithm_type, value_type,
                decltype(function)>;
        STATIC_REQUIRE(iteration_parameter_value<parameter_value_type,
            algorithm_type, value_type>);

        const parameter_value_type param_value{function};
        CHECK(param_value.get() == 123);  // NOLINT
        STATIC_REQUIRE(std::is_same_v<decltype(param_value.get()), int>);

        value = 2345;  // NOLINT
        void* algorithm = nullptr;
        CHECK(param_value.get(algorithm) == 2345);  // NOLINT
        STATIC_REQUIRE(
            std::is_same_v<decltype(param_value.get(algorithm)), int>);
    }

    SECTION("get references") {
        using algorithm_type = void;
        using value_type = int;

        value_type value = 123;  // NOLINT
        auto function = [&value]() -> const value_type& { return value; };

        using parameter_value_type =
            function_iteration_parameter_value<algorithm_type, value_type,
                decltype(function)>;
        STATIC_REQUIRE(iteration_parameter_value<parameter_value_type,
            algorithm_type, value_type>);

        const parameter_value_type param_value{function};

        CHECK(param_value.get() == 123);  // NOLINT
        STATIC_REQUIRE(std::is_same_v<decltype(param_value.get()), const int&>);

        value = 2345;  // NOLINT
        void* algorithm = nullptr;
        CHECK(param_value.get(algorithm) == 2345);  // NOLINT
        STATIC_REQUIRE(
            std::is_same_v<decltype(param_value.get(algorithm)), const int&>);
    }
}
