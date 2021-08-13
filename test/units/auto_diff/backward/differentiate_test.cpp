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
 * \brief Test of differentiate function.
 */
#include "num_collect/auto_diff/backward/differentiate.h"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::differentiate(variable, variable)", "",
    float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("same variable") {
        const auto var =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());

        const scalar_type coeff = differentiate(var, var);

        REQUIRE_THAT(
            coeff, Catch::Matchers::WithinRel(static_cast<scalar_type>(1)));
    }

    SECTION("quadratic function") {
        const auto var =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const variable_type val =
            static_cast<scalar_type>(0.5) * var * var - var;

        const scalar_type coeff = differentiate(val, var);

        const scalar_type true_coeff =
            var.value() - static_cast<scalar_type>(1);
        REQUIRE_THAT(coeff, Catch::Matchers::WithinRel(true_coeff));
    }

    SECTION("unrelated variable") {
        const auto var1 =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const auto var2 =
            variable_type(static_cast<scalar_type>(2.345), variable_tag());

        const scalar_type coeff = differentiate(var1, var2);

        REQUIRE(coeff == static_cast<scalar_type>(0));
    }
}
