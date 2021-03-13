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
 * \brief test of basic operations in multi-double calculations
 */
#include "num_collect/multi_double/impl/basic_operations.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

TEST_CASE("num_collect::multi_double::impl::quick_two_sum") {
    using num_collect::multi_double::impl::quick_two_sum;

    SECTION("calculate sum without error") {
        double s = 0.0;
        double e = 0.0;

        constexpr double a = 0x1.0p+0;
        constexpr double b = 0x1.0p-52;
        constexpr double s_true = 0x1.0000000000001p+0;
        constexpr double e_true = 0.0;
        quick_two_sum(a, b, s, e);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum with error") {
        double s = 0.0;
        double e = 0.0;

        constexpr double a = 0x1.0p+0;
        constexpr double b = 0x1.8p-52;
        constexpr double s_true = 0x1.0000000000002p+0;
        constexpr double e_true = -0x1.0p-53;
        quick_two_sum(a, b, s, e);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }
}

TEST_CASE("num_collect::multi_double::impl::two_sum") {
    using num_collect::multi_double::impl::two_sum;

    SECTION("calculate sum without error when a > b") {
        double s = 0.0;
        double e = 0.0;

        constexpr double a = 0x1.0p+0;
        constexpr double b = 0x1.0p-52;
        constexpr double s_true = 0x1.0000000000001p+0;
        constexpr double e_true = 0.0;
        two_sum(a, b, s, e);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum with error when a > b") {
        double s = 0.0;
        double e = 0.0;

        constexpr double a = 0x1.0p+0;
        constexpr double b = 0x1.8p-52;
        constexpr double s_true = 0x1.0000000000002p+0;
        constexpr double e_true = -0x1.0p-53;
        two_sum(a, b, s, e);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum without error when a < b") {
        double s = 0.0;
        double e = 0.0;

        constexpr double a = 0x1.0p-52;
        constexpr double b = 0x1.0p+0;
        constexpr double s_true = 0x1.0000000000001p+0;
        constexpr double e_true = 0.0;
        two_sum(a, b, s, e);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum with error when a < b") {
        double s = 0.0;
        double e = 0.0;

        constexpr double a = 0x1.8p-52;
        constexpr double b = 0x1.0p+0;
        constexpr double s_true = 0x1.0000000000002p+0;
        constexpr double e_true = -0x1.0p-53;
        two_sum(a, b, s, e);
        REQUIRE_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }
}
