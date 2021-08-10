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
#include "num_collect/auto_diff/backward/variable.h"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::variable", "", float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::constant_tag;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("construct with a node") {
        constexpr auto value = static_cast<scalar_type>(1.234);
        const auto node =
            num_collect::auto_diff::backward::graph::create_node<scalar_type>();

        const auto var = variable_type(value, node);

        REQUIRE(var.value() == value);
        REQUIRE(var.node() == node);
    }

    SECTION("construct a constant") {
        constexpr auto value = static_cast<scalar_type>(1.234);

        const auto var = variable_type(value, constant_tag());

        REQUIRE(var.value() == value);
        REQUIRE(var.node() == nullptr);
    }

    SECTION("construct a variable") {
        constexpr auto value = static_cast<scalar_type>(1.234);

        const auto var = variable_type(value, variable_tag());

        REQUIRE(var.value() == value);
        REQUIRE(var.node() != nullptr);
    }

    SECTION("construct a constant without tags") {
        constexpr auto value = static_cast<scalar_type>(1.234);

        const auto var = variable_type(value);

        REQUIRE(var.value() == value);
        REQUIRE(var.node() == nullptr);
    }

    SECTION("construct without arguments") {
        const auto var = variable_type();

        REQUIRE(var.value() == static_cast<scalar_type>(0));
        REQUIRE(var.node() == nullptr);
    }
}
