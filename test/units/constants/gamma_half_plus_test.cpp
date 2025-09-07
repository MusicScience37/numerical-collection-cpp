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
 * \brief Test of gamma_half_plus function.
 */
#include "num_collect/constants/gamma_half_plus.h"

#include <concepts>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/functions/gamma.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE_SIG("num_collect::constants::gamma_half_plus", "",
    ((typename Result, typename Integer), Result, Integer), (double, int),
    (double, unsigned int)) {
    if constexpr (std::signed_integral<Integer>) {
        SECTION("n = -3") {
            constexpr auto n = static_cast<Integer>(-3);
            constexpr auto val =
                num_collect::constants::gamma_half_plus<Result>(n);
            const auto reference = num_collect::functions::gamma(
                static_cast<Result>(n) + static_cast<Result>(0.5));
            CHECK_THAT(val, Catch::Matchers::WithinRel(reference));
        }

        SECTION("n = -2") {
            constexpr auto n = static_cast<Integer>(-2);
            constexpr auto val =
                num_collect::constants::gamma_half_plus<Result>(n);
            const auto reference = num_collect::functions::gamma(
                static_cast<Result>(n) + static_cast<Result>(0.5));
            CHECK_THAT(val, Catch::Matchers::WithinRel(reference));
        }

        SECTION("n = -1") {
            constexpr auto n = static_cast<Integer>(-1);
            constexpr auto val =
                num_collect::constants::gamma_half_plus<Result>(n);
            const auto reference = num_collect::functions::gamma(
                static_cast<Result>(n) + static_cast<Result>(0.5));
            CHECK_THAT(val, Catch::Matchers::WithinRel(reference));
        }
    }

    SECTION("n = 0") {
        constexpr auto n = static_cast<Integer>(0);
        constexpr auto val = num_collect::constants::gamma_half_plus<Result>(n);
        const auto reference = num_collect::functions::gamma(
            static_cast<Result>(n) + static_cast<Result>(0.5));
        CHECK_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("n = 1") {
        constexpr auto n = static_cast<Integer>(1);
        constexpr auto val = num_collect::constants::gamma_half_plus<Result>(n);
        const auto reference = num_collect::functions::gamma(
            static_cast<Result>(n) + static_cast<Result>(0.5));
        CHECK_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("n = 2") {
        constexpr auto n = static_cast<Integer>(2);
        constexpr auto val = num_collect::constants::gamma_half_plus<Result>(n);
        const auto reference = num_collect::functions::gamma(
            static_cast<Result>(n) + static_cast<Result>(0.5));
        CHECK_THAT(val, Catch::Matchers::WithinRel(reference));
    }

    SECTION("n = 3") {
        constexpr auto n = static_cast<Integer>(3);
        constexpr auto val = num_collect::constants::gamma_half_plus<Result>(n);
        const auto reference = num_collect::functions::gamma(
            static_cast<Result>(n) + static_cast<Result>(0.5));
        CHECK_THAT(val, Catch::Matchers::WithinRel(reference));
    }
}
