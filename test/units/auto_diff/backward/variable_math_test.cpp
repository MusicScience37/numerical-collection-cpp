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
 * \brief Test of mathemttical functions for variable class.
 */
#include "num_collect/auto_diff/backward/variable_math.h"

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/auto_diff/backward/differentiate.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::auto_diff::backward::exp", "", float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::constant_tag;
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("process an argument with node") {
        const auto var = variable_type(1.234, variable_tag());

        const variable_type res = exp(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::exp(var.value())));
        REQUIRE(res.node());

        const scalar_type coeff = differentiate(res, var);
        REQUIRE_THAT(coeff, Catch::Matchers::WithinRel(std::exp(var.value())));
    }

    SECTION("process an argument without node") {
        const auto var = variable_type(1.234, constant_tag());

        const variable_type res = exp(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::exp(var.value())));
        REQUIRE_FALSE(res.node());
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::auto_diff::backward::log", "", float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::constant_tag;
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("process an argument with node") {
        const auto var = variable_type(1.234, variable_tag());

        const variable_type res = log(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::log(var.value())));
        REQUIRE(res.node());

        const scalar_type coeff = differentiate(res, var);
        REQUIRE_THAT(coeff, Catch::Matchers::WithinRel(1 / var.value()));
    }

    SECTION("process an argument without node") {
        const auto var = variable_type(1.234, constant_tag());

        const variable_type res = log(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::log(var.value())));
        REQUIRE_FALSE(res.node());
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::sqrt", "", float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::constant_tag;
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("process an argument with node") {
        const auto var = variable_type(1.234, variable_tag());

        const variable_type res = sqrt(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::sqrt(var.value())));
        REQUIRE(res.node());

        const scalar_type coeff = differentiate(res, var);
        REQUIRE_THAT(coeff,
            Catch::Matchers::WithinRel(
                1 / static_cast<scalar_type>(2) / std::sqrt(var.value())));
    }

    SECTION("process an argument without node") {
        const auto var = variable_type(1.234, constant_tag());

        const variable_type res = log(var);
        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(std::log(var.value())));
        REQUIRE_FALSE(res.node());
    }
}
