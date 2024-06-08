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
 * \brief Test of basic operations in multi-double calculations
 */
#include "num_collect/multi_double/impl/basic_operations.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("num_collect::multi_double::impl::quick_two_sum") {
    using num_collect::multi_double::impl::quick_two_sum;

    SECTION("calculate sum without error") {
        constexpr double a = 0x1.0p+0;
        constexpr double b = 0x1.0p-52;
        constexpr double s_true = 0x1.0000000000001p+0;
        constexpr double e_true = 0.0;
        const auto [s, e] = quick_two_sum(a, b);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum with error") {
        constexpr double a = 0x1.0p+0;
        constexpr double b = 0x1.8p-52;
        constexpr double s_true = 0x1.0000000000002p+0;
        constexpr double e_true = -0x1.0p-53;
        const auto [s, e] = quick_two_sum(a, b);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }
}

TEST_CASE("num_collect::multi_double::impl::two_sum") {
    using num_collect::multi_double::impl::two_sum;

    SECTION("calculate sum without error when a > b") {
        constexpr double a = 0x1.0p+0;
        constexpr double b = 0x1.0p-52;
        constexpr double s_true = 0x1.0000000000001p+0;
        constexpr double e_true = 0.0;
        const auto [s, e] = two_sum(a, b);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum with error when a > b") {
        constexpr double a = 0x1.0p+0;
        constexpr double b = 0x1.8p-52;
        constexpr double s_true = 0x1.0000000000002p+0;
        constexpr double e_true = -0x1.0p-53;
        const auto [s, e] = two_sum(a, b);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum without error when a < b") {
        constexpr double a = 0x1.0p-52;
        constexpr double b = 0x1.0p+0;
        constexpr double s_true = 0x1.0000000000001p+0;
        constexpr double e_true = 0.0;
        const auto [s, e] = two_sum(a, b);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum with error when a < b") {
        constexpr double a = 0x1.8p-52;
        constexpr double b = 0x1.0p+0;
        constexpr double s_true = 0x1.0000000000002p+0;
        constexpr double e_true = -0x1.0p-53;
        const auto [s, e] = two_sum(a, b);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }
}

TEST_CASE("num_collect::multi_double::impl::split") {
    using num_collect::multi_double::impl::split;

    SECTION("split a number") {
        constexpr double a = 0x1.AAAAAAAAAAAAAp+0;
        constexpr double a_h_true = 0x1.AAAAAA8p+0;
        constexpr double a_l_true = 0x0.0000002AAAAAAp+0;
        const auto [a_h, a_l] = split(a);
        REQUIRE_THAT(a_h, Catch::Matchers::WithinULP(a_h_true, 0));
        REQUIRE_THAT(a_l, Catch::Matchers::WithinULP(a_l_true, 0));
    }

    SECTION("split another number") {
        constexpr double a = 0x1.5555555555555p+0;
        constexpr double a_h_true = 0x1.5555558p+0;
        constexpr double a_l_true = a - a_h_true;
        const auto [a_h, a_l] = split(a);
        REQUIRE_THAT(a_h, Catch::Matchers::WithinULP(a_h_true, 0));
        REQUIRE_THAT(a_l, Catch::Matchers::WithinULP(a_l_true, 0));
    }
}

TEST_CASE("num_collect::multi_double::impl::two_prod_no_fma") {
    using num_collect::multi_double::impl::two_prod_no_fma;

    SECTION("calculate product without error") {
        constexpr double a = 0x1.000001p+1;
        constexpr double b = 0x1.0000001p+2;
        constexpr double p_true = 0x1.0000011000001p+3;
        constexpr double e_true = 0.0;
        const auto [p, e] = two_prod_no_fma(a, b);
        REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with error") {
        constexpr double a = 0x1.0000001p+1;
        constexpr double b = 0x1.0000008p-2;
        constexpr double p_true = 0x1.0000009p-1;
        constexpr double e_true = 0x1.0p-54;
        const auto [p, e] = two_prod_no_fma(a, b);
        REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with small error") {
        constexpr double a = 0x1.0000000000001p+1;
        constexpr double b = 0x1.0000000000001p-2;
        constexpr double p_true = 0x1.0000000000002p-1;
        constexpr double e_true = 0x1.0p-105;
        const auto [p, e] = two_prod_no_fma(a, b);
        REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }
}

#ifdef __AVX2__

TEST_CASE("num_collect::multi_double::impl::two_prod_fma") {
    using num_collect::multi_double::impl::two_prod_fma;

    SECTION("calculate product without error") {
        constexpr double a = 0x1.000001p+1;
        constexpr double b = 0x1.0000001p+2;
        constexpr double p_true = 0x1.0000011000001p+3;
        constexpr double e_true = 0.0;
        const auto [p, e] = two_prod_fma(a, b);
        REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with error") {
        constexpr double a = 0x1.0000001p+1;
        constexpr double b = 0x1.0000008p-2;
        constexpr double p_true = 0x1.0000009p-1;
        constexpr double e_true = 0x1.0p-54;
        const auto [p, e] = two_prod_fma(a, b);
        REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with small error") {
        constexpr double a = 0x1.0000000000001p+1;
        constexpr double b = 0x1.0000000000001p-2;
        constexpr double p_true = 0x1.0000000000002p-1;
        constexpr double e_true = 0x1.0p-105;
        const auto [p, e] = two_prod_fma(a, b);
        REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }
}

#endif

TEST_CASE("num_collect::multi_double::impl::two_prod") {
    using num_collect::multi_double::impl::two_prod;

    SECTION("calculate product without error") {
        constexpr double a = 0x1.000001p+1;
        constexpr double b = 0x1.0000001p+2;
        constexpr double p_true = 0x1.0000011000001p+3;
        constexpr double e_true = 0.0;
        const auto [p, e] = two_prod(a, b);
        REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with error") {
        constexpr double a = 0x1.0000001p+1;
        constexpr double b = 0x1.0000008p-2;
        constexpr double p_true = 0x1.0000009p-1;
        constexpr double e_true = 0x1.0p-54;
        const auto [p, e] = two_prod(a, b);
        REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with small error") {
        constexpr double a = 0x1.0000000000001p+1;
        constexpr double b = 0x1.0000000000001p-2;
        constexpr double p_true = 0x1.0000000000002p-1;
        constexpr double e_true = 0x1.0p-105;
        const auto [p, e] = two_prod(a, b);
        REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }
}
