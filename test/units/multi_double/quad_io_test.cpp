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
 * \brief Test of IO functions of quad class.
 */
#include "num_collect/multi_double/quad_io.h"  // IWYU pragma: keep

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <fmt/format.h>

#include "num_collect/multi_double/quad.h"

TEST_CASE("formatter<num_collect::multi_double::quad>") {
    using num_collect::multi_double::quad;

    SECTION("format with no specification") {
        constexpr auto value =  // cspell: disable-next-line
            quad(0x1.13ef763c41377p-32, -0x1.f5dcebaf97254p-86);

        const std::string str = fmt::format("{}", value);

        CHECK_THAT(str,
            Catch::Matchers::Matches(
                R"(2\.50961782402166541591647970941\de\-10)"));
    }

    SECTION("format with a precision") {
        constexpr auto value =  // cspell: disable-next-line
            quad(0x1.13ef763c41377p-32, -0x1.f5dcebaf97254p-86);

        const std::string str = fmt::format("{:.10}", value);

        CHECK_THAT(str, Catch::Matchers::Matches(R"(2\.509617824\de\-10)"));
    }

    SECTION("format with a precision in scientific notation") {
        constexpr auto value =  // cspell: disable-next-line
            quad(0x1.13ef763c41377p-32, -0x1.f5dcebaf97254p-86);

        const std::string str = fmt::format("{:.10e}", value);

        CHECK_THAT(str, Catch::Matchers::Matches(R"(2\.509617824\de\-10)"));
    }

    SECTION("format with a width") {
        constexpr auto value =  // cspell: disable-next-line
            quad(0x1.13ef763c41377p-32, -0x1.f5dcebaf97254p-86);

        const std::string str = fmt::format("{:15.5e}", value);

        CHECK_THAT(str, Catch::Matchers::Matches(R"(    2\.5096\de\-10)"));
    }

    SECTION("format with another argument") {
        constexpr auto value =  // cspell: disable-next-line
            quad(0x1.13ef763c41377p-32, -0x1.f5dcebaf97254p-86);

        const std::string str = fmt::format(" {:15.5e} {}", value, 123);

        CHECK_THAT(str, Catch::Matchers::Matches(R"(     2\.5096\de\-10 123)"));
    }
}
