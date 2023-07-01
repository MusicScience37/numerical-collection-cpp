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
 * \brief Test of variable_iteration_parameter_value class.
 */
#include "num_collect/logging/iterations/variable_iteration_parameter_value.h"

#include <catch2/catch_test_macros.hpp>

#include "num_collect/logging/concepts/iteration_parameter_value.h"  // IWYU pragma: keep

TEST_CASE(
    "num_collect::logging::iterations::variable_iteration_parameter_value") {
    using num_collect::logging::concepts::iteration_parameter_value;
    using num_collect::logging::iterations::variable_iteration_parameter_value;

    SECTION("check of concept") {
        using algorithm_type = void;
        using value_type = int;
        STATIC_REQUIRE(iteration_parameter_value<
            variable_iteration_parameter_value<algorithm_type, value_type>,
            algorithm_type, value_type>);
    }

    SECTION("get values") {
        using algorithm_type = void;
        using value_type = int;
        using parameter_value_type =
            variable_iteration_parameter_value<algorithm_type, value_type>;

        value_type value = 123;  // NOLINT
        const parameter_value_type param_value{value};
        CHECK(param_value.get() == 123);  // NOLINT

        value = 2345;  // NOLINT
        void* algorithm = nullptr;
        CHECK(param_value.get(algorithm) == 2345);  // NOLINT
    }
}
