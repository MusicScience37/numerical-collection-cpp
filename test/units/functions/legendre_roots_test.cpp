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
 * \brief Test of legendre_roots class.
 */
#include "num_collect/functions/legendre_roots.h"

#include <ostream>
#include <string>

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/functions/legendre.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::functions::legendre_roots", "", float, double) {
    const num_collect::index_type degree =
        // NOLINTNEXTLINE
        GENERATE(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50);
    SECTION("construct with degree") {
        const auto roots =
            num_collect::functions::legendre_roots<TestType>(degree);

        REQUIRE(roots.degree() == degree);
        REQUIRE(roots.size() == degree);

        REQUIRE(roots[0] < static_cast<TestType>(1));
        for (num_collect::index_type i = 1; i < roots.size(); ++i) {
            INFO("i = " << i);
            REQUIRE(roots[i] < roots[i - 1]);

            constexpr auto tol = std::numeric_limits<TestType>::epsilon() *
                static_cast<TestType>(1e+4);
            REQUIRE_THAT(num_collect::functions::legendre(roots[i], degree),
                Catch::Matchers::WithinAbs(static_cast<TestType>(0), tol));
        }
        REQUIRE(roots[roots.size() - 1] > static_cast<TestType>(-1));
    }

    SECTION("compute") {
        auto roots = num_collect::functions::legendre_roots<TestType>();
        REQUIRE(roots.degree() == 0);
        REQUIRE(roots.size() == 0);

        roots.compute(degree);

        REQUIRE(roots.degree() == degree);
        REQUIRE(roots.size() == degree);

        REQUIRE(roots[0] < static_cast<TestType>(1));
        for (num_collect::index_type i = 1; i < roots.size(); ++i) {
            INFO("i = " << i);
            REQUIRE(roots[i] < roots[i - 1]);

            constexpr auto tol = std::numeric_limits<TestType>::epsilon() *
                static_cast<TestType>(1e+4);
            REQUIRE_THAT(num_collect::functions::legendre(roots[i], degree),
                Catch::Matchers::WithinAbs(static_cast<TestType>(0), tol));
        }
        REQUIRE(roots[roots.size() - 1] > static_cast<TestType>(-1));
    }
}
