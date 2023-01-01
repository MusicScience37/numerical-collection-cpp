/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of multiply_safely function.
 */
#include "num_collect/util/multiply_safely.h"

#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

TEST_CASE("num_collect::util::multiply_safely") {
    using num_collect::util::multiply_safely;

    SECTION("calculate at runtime") {
        const int a = 5;
        const int b = 7;
        const int expected = 35;
        const int result = multiply_safely(a, b);
        CHECK(result == expected);
    }

    SECTION("calculate at compile-time") {
        constexpr int a = 5;
        constexpr int b = 7;
        constexpr int expected = 35;
        constexpr int result = multiply_safely(a, b);
        CHECK(result == expected);
    }

    SECTION("calculate random problems") {
        static constexpr std::int32_t min_input = -0x7FFF;
        static constexpr std::int32_t max_input = 0x7FFF;
        static constexpr std::int32_t num_inputs = 10;

        const std::int32_t a = GENERATE(Catch::Generators::take(
            num_inputs, Catch::Generators::random(min_input, max_input)));
        const std::int32_t b = GENERATE(Catch::Generators::take(
            num_inputs, Catch::Generators::random(min_input, max_input)));

        const std::int32_t expected = a * b;
        const std::int32_t result = multiply_safely(a, b);
        CHECK(result == expected);
    }

    SECTION("check of overflows") {
        CHECK(multiply_safely(0, 0) == 0);
        CHECK(multiply_safely(0, 1) == 0);

        CHECK(multiply_safely(static_cast<std::uint32_t>(0x0F0F0F0F),
                  static_cast<std::uint32_t>(0x11)) ==
            static_cast<std::uint32_t>(0xFFFFFFFF));
        CHECK_THROWS(multiply_safely(static_cast<std::uint32_t>(0x10000),
            static_cast<std::uint32_t>(0x10000)));
    }
}
