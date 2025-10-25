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
 * \brief Test of kahan_adder class.
 */
#include "num_collect/util/kahan_adder.h"

#include <cmath>
#include <cstddef>
#include <ostream>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/base/constants.h"

TEMPLATE_TEST_CASE("num_collect::util::kahan_adder (floating-point number)", "",
    float, double) {
    using adder_type = num_collect::util::kahan_adder<TestType>;

    SECTION("default constructor") {
        auto adder = adder_type();
        REQUIRE(adder.sum() == static_cast<TestType>(0));
    }

    SECTION("add some numbers") {
        auto adder = adder_type();
        adder += 1.0;
        adder += 2.0;
        REQUIRE(adder.sum() == static_cast<TestType>(3));
    }

    SECTION("subtract some numbers") {
        auto adder = adder_type();
        adder -= 1.0;
        adder -= 2.0;
        REQUIRE(adder.sum() == static_cast<TestType>(-3));
    }

    SECTION("calculate an infinite sum") {
        auto adder = adder_type();
        constexpr std::size_t terms = 10000;
        for (std::size_t i = 1; i <= terms; ++i) {
            const auto term =
                static_cast<TestType>(std::pow(static_cast<TestType>(i), -2));
            adder += term;
        }
        const auto reference =
            static_cast<TestType>(std::pow(num_collect::pi<TestType>, 2)) /
            static_cast<TestType>(6);
        const auto tol = static_cast<TestType>(1e-4);
        REQUIRE_THAT(adder.sum(), Catch::Matchers::WithinRel(reference, tol));
    }
}
