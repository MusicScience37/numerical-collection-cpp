/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of polynomial_term class.
 */
#include "num_collect/rbf/polynomial_term.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("num_collect::rbf::polynomial_term") {
    using num_collect::rbf::polynomial_term;

    SECTION("create a polynomial term") {
        const Eigen::Vector3i degrees(2, 1, 0);

        const polynomial_term term(degrees);

        CHECK(term.degrees() == degrees);
    }

    SECTION("evaluate a polynomial for a variable") {
        const Eigen::Vector2i degrees(1, 2);
        const polynomial_term term(degrees);

        const Eigen::Vector2d variable(1.2, 3.4);
        const double value = term(variable);

        const double expected_value = 13.872;
        CHECK_THAT(value, Catch::Matchers::WithinRel(expected_value));
    }
}
