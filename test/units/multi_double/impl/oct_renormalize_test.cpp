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
 * \brief Test of oct_renormalize function.
 */
#include "num_collect/multi_double/impl/oct_renormalize.h"

#include <array>
#include <iomanip>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("num_collect::multi_double::impl::oct_renormalize") {
    using num_collect::multi_double::impl::oct_renormalize;

    SECTION("calculate random problems") {
        std::array<double, 5> inputs{};
        std::array<double, 4> results_true{};
        std::tie(inputs, results_true) =
            GENERATE(Catch::Generators::table<std::array<double, 5>,
                std::array<double, 4>>({
                // cspell: disable
                std::make_tuple(
                    std::to_array<double>({0x1.a45f4d3ecd99ap+28,
                        0x1.9a21dc10c2ea5p-22, 0x1.a1bf18db488afp-72,
                        0x1.c36b2a2df1c6fp-122, 0x1.329f3ad37fc34p-172}),
                    std::to_array<double>(
                        {0x1.a45f4d3ecd9a0p+28, 0x1.a21dc10c2eab8p-26,
                            0x1.bf18db488b60ep-80, -0x1.4d5d20e38c358p-134})),
                // cspell: enable
            }));
        INFO("inputs: " << std::hexfloat << std::setprecision(13) << inputs[0]
                        << ", " << inputs[1] << ", " << inputs[2] << ", "
                        << inputs[3] << ", " << inputs[4]);
        INFO("results_true: " << std::hexfloat << std::setprecision(13)
                              << results_true[0] << ", " << results_true[1]
                              << ", " << results_true[2] << ", "
                              << results_true[3]);

        const auto results = oct_renormalize(inputs);
        INFO("results: " << std::hexfloat << std::setprecision(13) << results[0]
                         << ", " << results[1] << ", " << results[2] << ", "
                         << results[3]);

        for (std::size_t i = 0; i < results.size(); ++i) {
            INFO("i = " << i);
            CHECK_THAT(
                results[i], Catch::Matchers::WithinULP(results_true[i], 0));
        }
    }
}
