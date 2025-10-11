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
 * \brief Test of functions of internal implementations of exp function
 * for quad class.
 */
#include "num_collect/multi_double/impl/quad_exp_impl.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../format_quad_for_test.h"  // IWYU pragma: keep
#include "../quad_approx.h"

TEST_CASE("num_collect::multi_double::impl::expm1_maclaurin_series") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::expm1_maclaurin_series;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.64840e1719f80p-10, 0x1.cd5f99c38b04cp-64),
                quad(-0x1.64460558e8f48p-10, 0x1.b58b9e5cb561dp-64)),
            std::make_tuple(quad(-0x1.fd71823e9ed31p-28, 0x1.456b15bb2ec98p-83),
                quad(-0x1.fd71821ef06e8p-28, 0x1.a25022221087cp-83)),
            std::make_tuple(quad(-0x1.cdc648a9cfaeap-15, 0x1.8963ce8d06dd0p-69),
                quad(-0x1.cdc307b9fe0b1p-15, 0x1.ad7eb544f8e36p-70)),
            std::make_tuple(quad(0x1.3e3e59d300f44p-40, 0x1.c41897769fec4p-94),
                quad(0x1.3e3e59d301ba1p-40, 0x1.a2e3928e1558cp-94)),
            std::make_tuple(quad(0x1.8779b52a9a07bp-18, 0x1.c9fb9fa077a00p-76),
                quad(0x1.8779ffff43b98p-18, -0x1.60ba2240b82eep-76)),
            std::make_tuple(quad(0x1.64840e1719f80p-10, -0x1.cd5f99c38b04cp-64),
                quad(0x1.64c2253cfda0ep-10, -0x1.eb059ebed320cp-64)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = expm1_maclaurin_series(input);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(
            actual, make_quad_within_rel_matcher(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(0.0);

        const quad actual = expm1_maclaurin_series(input);

        CHECK(actual == expected);
    }
}

TEST_CASE("num_collect::multi_double::impl::exp_maclaurin_series") {
    using num_collect::multi_double::quad;
    using num_collect::multi_double::impl::exp_maclaurin_series;

    SECTION("calculate for non-zero values") {
        quad input;
        quad expected;
        std::tie(
            input, expected) = GENERATE(Catch::Generators::table<quad, quad>({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(quad(-0x1.64840e1719f80p-10, 0x1.cd5f99c38b04cp-64),
                quad(0x1.ff4ddcfd538b8p-1, 0x1.70dac5cf2e5abp-55)),
            std::make_tuple(quad(-0x1.fd71823e9ed31p-28, 0x1.456b15bb2ec98p-83),
                quad(0x1.ffffffc051cfcp-1, -0x1.ef06e7cbb5fbcp-56)),
            std::make_tuple(quad(-0x1.cdc648a9cfaeap-15, 0x1.8963ce8d06dd0p-69),
                quad(0x1.fff8c8f3e1180p-1, 0x1.f4f35afd6a89fp-55)),
            std::make_tuple(quad(0x1.3e3e59d300f44p-40, 0x1.c41897769fec4p-94),
                quad(0x1.00000000013e4p+0, -0x1.a62cfe45e9747p-56)),
            std::make_tuple(quad(0x1.8779b52a9a07bp-18, 0x1.c9fb9fa077a00p-76),
                quad(0x1.000061de7fffdp+0, 0x1.dcbfd3e8bbb7fp-57)),
            std::make_tuple(quad(0x1.64840e1719f80p-10, -0x1.cd5f99c38b04cp-64),
                quad(0x1.005930894f3f7p+0, -0x1.f27ac167afb4dp-54)),
            // cspell: enable
            // NOLINTEND
        }));
        INFO("input: " << format_quad_for_test(input));

        const quad actual = exp_maclaurin_series(input);
        constexpr quad relative_tolerance(0x1.0p-102);
        CHECK_THAT(
            actual, make_quad_within_rel_matcher(expected, relative_tolerance));
    }

    SECTION("calculate for zero") {
        const quad input(0.0);
        const quad expected(1.0);

        const quad actual = exp_maclaurin_series(input);

        CHECK(actual == expected);
    }
}
