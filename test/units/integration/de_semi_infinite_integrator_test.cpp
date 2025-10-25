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
 * \brief Test of de_semi_infinite_integrator class.
 */
#include "num_collect/integration/de_semi_infinite_integrator.h"

#include <cmath>
#include <ostream>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/constants.h"

TEMPLATE_TEST_CASE("num_collect::integration::de_semi_infinite_integrator", "",
    float, double) {
    SECTION("integrate exp(-x)") {
        const auto integrator =
            num_collect::integration::de_semi_infinite_integrator<TestType(
                TestType)>();

        const auto val = integrator([](TestType x) { return std::exp(-x); });

        const auto true_val = static_cast<TestType>(1);
        constexpr auto tol = static_cast<TestType>(1e-4);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val, tol));
    }

    SECTION("compute gamma(1/2)") {
        const auto integrator =
            num_collect::integration::de_semi_infinite_integrator<TestType(
                TestType)>();

        const auto val =
            integrator([](TestType x) { return std::exp(-x) / std::sqrt(x); });

        const auto true_val = std::sqrt(num_collect::pi<TestType>);
        constexpr auto tol = static_cast<TestType>(1e-4);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val, tol));
    }
}
