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
 * \brief Test of root function.
 */
#include "num_collect/constants/root.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::constants::root", "", float, double) {
    SECTION("x < 0, n is even") {
        constexpr auto x = static_cast<TestType>(-1.0);
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr TestType val = num_collect::constants::root(x, n);
            REQUIRE(std::isnan(val));
        }
        SECTION("n = 4") {
            constexpr auto n = 4;
            constexpr TestType val = num_collect::constants::root(x, n);
            REQUIRE(std::isnan(val));
        }
    }

    SECTION("x < 0, n is odd") {
        constexpr auto x = static_cast<TestType>(-1.234);
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = -std::pow(
                -static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 5") {
            constexpr auto n = 5;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = -std::pow(
                -static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 11") {
            constexpr auto n = 11;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = -std::pow(
                -static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
    }

    SECTION("x = 0") {
        constexpr auto x = static_cast<TestType>(0);
        constexpr auto true_val = x;
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr TestType val = num_collect::constants::root(x, n);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr TestType val = num_collect::constants::root(x, n);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("n = 4") {
            constexpr auto n = 4;
            constexpr TestType val = num_collect::constants::root(x, n);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
    }

    SECTION("x > 0") {
        constexpr auto x = static_cast<TestType>(1.234);
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = std::pow(
                static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = std::pow(
                static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 4") {
            constexpr auto n = 4;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = std::pow(
                static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 5") {
            constexpr auto n = 5;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = std::pow(
                static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 10") {
            constexpr auto n = 10;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = std::pow(
                static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 11") {
            constexpr auto n = 11;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = std::pow(
                static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
    }

    SECTION("x >> 0") {
        constexpr auto x = static_cast<TestType>(1.234e+10);
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = std::pow(
                static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr TestType val = num_collect::constants::root(x, n);
            const TestType reference = std::pow(
                static_cast<TestType>(x), 1.0 / static_cast<TestType>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
    }

    SECTION("x == infinity") {
        constexpr auto x = std::numeric_limits<TestType>::infinity();
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr TestType val = num_collect::constants::root(x, n);
            REQUIRE(std::isinf(val));
            REQUIRE(val > num_collect::constants::zero<TestType>);
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr TestType val = num_collect::constants::root(x, n);
            REQUIRE(std::isinf(val));
            REQUIRE(val > num_collect::constants::zero<TestType>);
        }
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::root (integers)", "", int, long long) {
    SECTION("x < 0, n is even") {
        constexpr auto x = static_cast<TestType>(-2);
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr double val = num_collect::constants::root(x, n);
            REQUIRE(std::isnan(val));
        }
        SECTION("n = 4") {
            constexpr auto n = 4;
            constexpr double val = num_collect::constants::root(x, n);
            REQUIRE(std::isnan(val));
        }
    }

    SECTION("x < 0, n is odd") {
        constexpr auto x = static_cast<TestType>(-5);
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference = -std::pow(
                -static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 5") {
            constexpr auto n = 5;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference = -std::pow(
                -static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 11") {
            constexpr auto n = 11;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference = -std::pow(
                -static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
    }

    SECTION("x = 0") {
        constexpr auto x = static_cast<TestType>(0);
        constexpr double true_val = 0.0;
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr TestType val = num_collect::constants::root(x, n);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr TestType val = num_collect::constants::root(x, n);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("n = 4") {
            constexpr auto n = 4;
            constexpr TestType val = num_collect::constants::root(x, n);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
    }

    SECTION("x > 0") {
        constexpr auto x = static_cast<TestType>(123);
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 4") {
            constexpr auto n = 4;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 5") {
            constexpr auto n = 5;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 10") {
            constexpr auto n = 10;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 11") {
            constexpr auto n = 11;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
    }

    SECTION("x >> 0") {
        constexpr auto x = static_cast<TestType>(1234566789);
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 4") {
            constexpr auto n = 4;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 5") {
            constexpr auto n = 5;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 10") {
            constexpr auto n = 10;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
        SECTION("n = 11") {
            constexpr auto n = 11;
            constexpr double val = num_collect::constants::root(x, n);
            const double reference =
                std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(reference));
        }
    }
}
