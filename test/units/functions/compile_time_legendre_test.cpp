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
 * \brief Test of legendre function on compile time.
 */
#include <ostream>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/functions/legendre.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::functions::legendre (compile time)", "", float, double) {
    SECTION("n = 0") {
        constexpr int n = 0;
        SECTION("x = -1") {
            constexpr auto x = static_cast<TestType>(-1);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = 1.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = -0.5") {
            constexpr auto x = static_cast<TestType>(-0.5);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = 1.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 0") {
            constexpr auto x = static_cast<TestType>(0);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = 1.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 0.5") {
            constexpr auto x = static_cast<TestType>(0.5);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = 1.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 1") {
            constexpr auto x = static_cast<TestType>(1);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = 1.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
    }

    SECTION("n = 1") {
        constexpr int n = 1;
        SECTION("x = -1") {
            constexpr auto x = static_cast<TestType>(-1);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = -0.5") {
            constexpr auto x = static_cast<TestType>(-0.5);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 0") {
            constexpr auto x = static_cast<TestType>(0);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 0.5") {
            constexpr auto x = static_cast<TestType>(0.5);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 1") {
            constexpr auto x = static_cast<TestType>(1);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
    }

    SECTION("n = 2") {
        constexpr int n = 2;
        SECTION("x = -1") {
            constexpr auto x = static_cast<TestType>(-1);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = static_cast<TestType>(1.5) * x * x +
                static_cast<TestType>(-0.5);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = -0.5") {
            constexpr auto x = static_cast<TestType>(-0.5);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = static_cast<TestType>(1.5) * x * x +
                static_cast<TestType>(-0.5);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 0") {
            constexpr auto x = static_cast<TestType>(0);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = static_cast<TestType>(1.5) * x * x +
                static_cast<TestType>(-0.5);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 0.5") {
            constexpr auto x = static_cast<TestType>(0.5);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = static_cast<TestType>(1.5) * x * x +
                static_cast<TestType>(-0.5);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 1") {
            constexpr auto x = static_cast<TestType>(1);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = static_cast<TestType>(1.5) * x * x +
                static_cast<TestType>(-0.5);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
    }

    SECTION("n = 3") {
        constexpr int n = 3;
        SECTION("x = -1") {
            constexpr auto x = static_cast<TestType>(-1);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = static_cast<TestType>(2.5) * x * x * x +
                static_cast<TestType>(-1.5) * x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = -0.5") {
            constexpr auto x = static_cast<TestType>(-0.5);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = static_cast<TestType>(2.5) * x * x * x +
                static_cast<TestType>(-1.5) * x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 0") {
            constexpr auto x = static_cast<TestType>(0);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = static_cast<TestType>(2.5) * x * x * x +
                static_cast<TestType>(-1.5) * x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 0.5") {
            constexpr auto x = static_cast<TestType>(0.5);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = static_cast<TestType>(2.5) * x * x * x +
                static_cast<TestType>(-1.5) * x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
        SECTION("x = 1") {
            constexpr auto x = static_cast<TestType>(1);
            constexpr auto val = num_collect::functions::legendre(x, n);
            constexpr auto true_val = static_cast<TestType>(2.5) * x * x * x +
                static_cast<TestType>(-1.5) * x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
        }
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::functions::legendre_with_diff (compile time)",
    "", float, double) {
    SECTION("n = 0") {
        constexpr int n = 0;
        SECTION("x = -1") {
            constexpr auto x = static_cast<TestType>(-1);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = 1.0;
            constexpr auto true_diff = 0.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = -0.5") {
            constexpr auto x = static_cast<TestType>(-0.5);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = 1.0;
            constexpr auto true_diff = 0.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 0") {
            constexpr auto x = static_cast<TestType>(0);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = 1.0;
            constexpr auto true_diff = 0.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 0.5") {
            constexpr auto x = static_cast<TestType>(0.5);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = 1.0;
            constexpr auto true_diff = 0.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 1") {
            constexpr auto x = static_cast<TestType>(1);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = 1.0;
            constexpr auto true_diff = 0.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
    }

    SECTION("n = 1") {
        constexpr int n = 1;
        SECTION("x = -1") {
            constexpr auto x = static_cast<TestType>(-1);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = x;
            constexpr auto true_diff = 1.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = -0.5") {
            constexpr auto x = static_cast<TestType>(-0.5);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = x;
            constexpr auto true_diff = 1.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 0") {
            constexpr auto x = static_cast<TestType>(0);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = x;
            constexpr auto true_diff = 1.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 0.5") {
            constexpr auto x = static_cast<TestType>(0.5);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = x;
            constexpr auto true_diff = 1.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 1") {
            constexpr auto x = static_cast<TestType>(1);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = x;
            constexpr auto true_diff = 1.0;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
    }

    SECTION("n = 2") {
        constexpr int n = 2;
        SECTION("x = -1") {
            constexpr auto x = static_cast<TestType>(-1);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = static_cast<TestType>(1.5) * x * x +
                static_cast<TestType>(-0.5);
            constexpr auto true_diff = static_cast<TestType>(3) * x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = -0.5") {
            constexpr auto x = static_cast<TestType>(-0.5);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = static_cast<TestType>(1.5) * x * x +
                static_cast<TestType>(-0.5);
            constexpr auto true_diff = static_cast<TestType>(3) * x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 0") {
            constexpr auto x = static_cast<TestType>(0);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = static_cast<TestType>(1.5) * x * x +
                static_cast<TestType>(-0.5);
            constexpr auto true_diff = static_cast<TestType>(3) * x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 0.5") {
            constexpr auto x = static_cast<TestType>(0.5);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = static_cast<TestType>(1.5) * x * x +
                static_cast<TestType>(-0.5);
            constexpr auto true_diff = static_cast<TestType>(3) * x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 1") {
            constexpr auto x = static_cast<TestType>(1);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = static_cast<TestType>(1.5) * x * x +
                static_cast<TestType>(-0.5);
            constexpr auto true_diff = static_cast<TestType>(3) * x;
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
    }

    SECTION("n = 3") {
        constexpr int n = 3;
        SECTION("x = -1") {
            constexpr auto x = static_cast<TestType>(-1);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = static_cast<TestType>(2.5) * x * x * x +
                static_cast<TestType>(-1.5) * x;
            constexpr auto true_diff = static_cast<TestType>(7.5) * x * x +
                static_cast<TestType>(-1.5);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = -0.5") {
            constexpr auto x = static_cast<TestType>(-0.5);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = static_cast<TestType>(2.5) * x * x * x +
                static_cast<TestType>(-1.5) * x;
            constexpr auto true_diff = static_cast<TestType>(7.5) * x * x +
                static_cast<TestType>(-1.5);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 0") {
            constexpr auto x = static_cast<TestType>(0);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = static_cast<TestType>(2.5) * x * x * x +
                static_cast<TestType>(-1.5) * x;
            constexpr auto true_diff = static_cast<TestType>(7.5) * x * x +
                static_cast<TestType>(-1.5);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 0.5") {
            constexpr auto x = static_cast<TestType>(0.5);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = static_cast<TestType>(2.5) * x * x * x +
                static_cast<TestType>(-1.5) * x;
            constexpr auto true_diff = static_cast<TestType>(7.5) * x * x +
                static_cast<TestType>(-1.5);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
        SECTION("x = 1") {
            constexpr auto x = static_cast<TestType>(1);
            constexpr auto pair =
                num_collect::functions::legendre_with_diff(x, n);
            const auto [val, diff] = pair;
            constexpr auto true_val = static_cast<TestType>(2.5) * x * x * x +
                static_cast<TestType>(-1.5) * x;
            constexpr auto true_diff = static_cast<TestType>(7.5) * x * x +
                static_cast<TestType>(-1.5);
            REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val));
            REQUIRE_THAT(diff, Catch::Matchers::WithinRel(true_diff));
        }
    }
}
