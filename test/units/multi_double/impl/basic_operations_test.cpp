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
#include <catch2/generators/catch_generators.hpp>
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
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum with error") {
        constexpr double a = 0x1.0p+0;
        constexpr double b = 0x1.8p-52;
        constexpr double s_true = 0x1.0000000000002p+0;
        constexpr double e_true = -0x1.0p-53;
        const auto [s, e] = quick_two_sum(a, b);
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("random problems") {
        double a{};
        double b{};
        double s_true{};
        double e_true{};
        std::tie(a, b, s_true, e_true) =
            GENERATE(Catch::Generators::table<double, double, double, double>({
                // NOLINTBEGIN
                // cspell: disable
                std::make_tuple(0x1.4ec8826dadce9p-1, 0x1.1af9948136b00p-21,
                    0x1.4ec8941d4716ap-1, 0x1.b580000000000p-56),
                std::make_tuple(0x1.42dcb014b0680p+16, -0x1.087a25b23ec08p-25,
                    0x1.42dcb014afe3cp+16, 0x1.7693704fe0000p-39),
                std::make_tuple(-0x1.4b2089fd0eaf2p-13, 0x1.75ab32e4c3a6ep-54,
                    -0x1.4b2089fd0df45p-13, 0x1.665c9874dc000p-67)
                // cspell: enable
                // NOLINTEND
            }));
        const auto [s, e] = quick_two_sum(a, b);
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate at compile time") {
        static constexpr auto inputs = std::to_array({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(0x1.4ec8826dadce9p-1, 0x1.1af9948136b00p-21,
                0x1.4ec8941d4716ap-1, 0x1.b580000000000p-56),
            std::make_tuple(0x1.42dcb014b0680p+16, -0x1.087a25b23ec08p-25,
                0x1.42dcb014afe3cp+16, 0x1.7693704fe0000p-39),
            std::make_tuple(-0x1.4b2089fd0eaf2p-13, 0x1.75ab32e4c3a6ep-54,
                -0x1.4b2089fd0df45p-13, 0x1.665c9874dc000p-67),
            // cspell: enable
            // NOLINTEND
        });
        constexpr auto outputs = [] constexpr {
            std::array<std::tuple<double, double>, inputs.size()> results{};
            for (std::size_t i = 0; i < inputs.size(); ++i) {
                const auto& [a, b, s, e] = inputs[i];
                results[i] = quick_two_sum(a, b);
            }
            return results;
        }();

        for (std::size_t i = 0; i < inputs.size(); ++i) {
            INFO("i = " << i);
            const auto& [a, b, s_true, e_true] = inputs[i];
            const auto& [s, e] = outputs[i];
            CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
            CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
        }
    }

    SECTION("calculate with zeros") {
        constexpr double a = 0.0;
        constexpr double b = 0.0;
        constexpr double s_true = 0.0;
        constexpr double e_true = 0.0;
        const auto [s, e] = quick_two_sum(a, b);
        CHECK(s == s_true);  // NOLINT
        CHECK(e == e_true);  // NOLINT
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
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum with error when a > b") {
        constexpr double a = 0x1.0p+0;
        constexpr double b = 0x1.8p-52;
        constexpr double s_true = 0x1.0000000000002p+0;
        constexpr double e_true = -0x1.0p-53;
        const auto [s, e] = two_sum(a, b);
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum without error when a < b") {
        constexpr double a = 0x1.0p-52;
        constexpr double b = 0x1.0p+0;
        constexpr double s_true = 0x1.0000000000001p+0;
        constexpr double e_true = 0.0;
        const auto [s, e] = two_sum(a, b);
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate sum with error when a < b") {
        constexpr double a = 0x1.8p-52;
        constexpr double b = 0x1.0p+0;
        constexpr double s_true = 0x1.0000000000002p+0;
        constexpr double e_true = -0x1.0p-53;
        const auto [s, e] = two_sum(a, b);
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("random problems") {
        double a{};
        double b{};
        double s_true{};
        double e_true{};
        std::tie(a, b, s_true, e_true) =
            GENERATE(Catch::Generators::table<double, double, double, double>({
                // NOLINTBEGIN
                // cspell: disable
                std::make_tuple(0x1.752ce6b30fa8ap-2, 0x1.63ca1efb198f9p-37,
                    0x1.752ce6b33c21ep-2, 0x1.efb198f900000p-57),
                std::make_tuple(-0x1.361e3ab0234bap-26, 0x1.a65c4b218a9eep+11,
                    0x1.a65c4b2180edfp+11, -0x1.d5811a5d00000p-45),
                std::make_tuple(0x1.fbfe071bdbba7p-8, -0x1.109e2f692e0f6p+9,
                    -0x1.109d316a2a817p+9, -0x1.1164000000000p-46),
                std::make_tuple(-0x1.f20991f2ea560p-17, 0x1.84cadb1bf3b14p-34,
                    -0x1.f208cf8d7cc80p-17, -0x1.89d8000000000p-71),
                // cspell: enable
                // NOLINTEND
            }));
        const auto [s, e] = two_sum(a, b);
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate at compile time") {
        static constexpr auto inputs = std::to_array({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(0x1.752ce6b30fa8ap-2, 0x1.63ca1efb198f9p-37,
                0x1.752ce6b33c21ep-2, 0x1.efb198f900000p-57),
            std::make_tuple(-0x1.361e3ab0234bap-26, 0x1.a65c4b218a9eep+11,
                0x1.a65c4b2180edfp+11, -0x1.d5811a5d00000p-45),
            std::make_tuple(0x1.fbfe071bdbba7p-8, -0x1.109e2f692e0f6p+9,
                -0x1.109d316a2a817p+9, -0x1.1164000000000p-46),
            std::make_tuple(-0x1.f20991f2ea560p-17, 0x1.84cadb1bf3b14p-34,
                -0x1.f208cf8d7cc80p-17, -0x1.89d8000000000p-71),
            // cspell: enable
            // NOLINTEND
        });
        constexpr auto outputs = [] constexpr {
            std::array<std::tuple<double, double>, inputs.size()> results{};
            for (std::size_t i = 0; i < inputs.size(); ++i) {
                const auto& [a, b, s, e] = inputs[i];
                results[i] = two_sum(a, b);
            }
            return results;
        }();

        for (std::size_t i = 0; i < inputs.size(); ++i) {
            INFO("i = " << i);
            const auto& [a, b, s_true, e_true] = inputs[i];
            const auto& [s, e] = outputs[i];
            CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
            CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
        }
    }

    SECTION("calculate with zeros") {
        constexpr double a = 0.0;
        constexpr double b = 0.0;
        constexpr double s_true = 0.0;
        constexpr double e_true = 0.0;
        const auto [s, e] = two_sum(a, b);
        CHECK(s == s_true);  // NOLINT
        CHECK(e == e_true);  // NOLINT
    }
}

TEST_CASE("num_collect::multi_double::impl::split") {
    using num_collect::multi_double::impl::split;

    SECTION("split a number") {
        constexpr double a = 0x1.AAAAAAAAAAAAAp+0;
        constexpr double a_h_true = 0x1.AAAAAA8p+0;
        constexpr double a_l_true = 0x0.0000002AAAAAAp+0;
        const auto [a_h, a_l] = split(a);
        CHECK_THAT(a_h, Catch::Matchers::WithinULP(a_h_true, 0));
        CHECK_THAT(a_l, Catch::Matchers::WithinULP(a_l_true, 0));
    }

    SECTION("split another number") {
        constexpr double a = 0x1.5555555555555p+0;
        constexpr double a_h_true = 0x1.5555558p+0;
        constexpr double a_l_true = a - a_h_true;
        const auto [a_h, a_l] = split(a);
        CHECK_THAT(a_h, Catch::Matchers::WithinULP(a_h_true, 0));
        CHECK_THAT(a_l, Catch::Matchers::WithinULP(a_l_true, 0));
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
        CHECK_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with error") {
        constexpr double a = 0x1.0000001p+1;
        constexpr double b = 0x1.0000008p-2;
        constexpr double p_true = 0x1.0000009p-1;
        constexpr double e_true = 0x1.0p-54;
        const auto [p, e] = two_prod_no_fma(a, b);
        CHECK_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with small error") {
        constexpr double a = 0x1.0000000000001p+1;
        constexpr double b = 0x1.0000000000001p-2;
        constexpr double p_true = 0x1.0000000000002p-1;
        constexpr double e_true = 0x1.0p-105;
        const auto [p, e] = two_prod_no_fma(a, b);
        CHECK_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("random problems") {
        double a{};
        double b{};
        double s_true{};
        double e_true{};
        std::tie(a, b, s_true, e_true) =
            GENERATE(Catch::Generators::table<double, double, double, double>({
                // NOLINTBEGIN
                // cspell: disable
                std::make_tuple(0x1.0eda2a085a8b0p+16, 0x1.35e2911f3e380p-10,
                    0x1.47dd0adfd0b7ap+6, -0x1.0b8b34c3cc000p-49),
                std::make_tuple(-0x1.1e20cf1e86a2cp-15, 0x1.9302680dbea10p+9,
                    -0x1.c270569b5d0e8p-6, 0x1.b828673249500p-60),
                std::make_tuple(0x1.6ebd2ea984d78p+8, -0x1.83fea770287bcp-21,
                    -0x1.15ea668e531bbp-12, -0x1.6c2fbf7465840p-66),
                // cspell: enable
                // NOLINTEND
            }));
        const auto [s, e] = two_prod_no_fma(a, b);
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate at compile time") {
        static constexpr auto inputs = std::to_array({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(0x1.0eda2a085a8b0p+16, 0x1.35e2911f3e380p-10,
                0x1.47dd0adfd0b7ap+6, -0x1.0b8b34c3cc000p-49),
            std::make_tuple(-0x1.1e20cf1e86a2cp-15, 0x1.9302680dbea10p+9,
                -0x1.c270569b5d0e8p-6, 0x1.b828673249500p-60),
            std::make_tuple(0x1.6ebd2ea984d78p+8, -0x1.83fea770287bcp-21,
                -0x1.15ea668e531bbp-12, -0x1.6c2fbf7465840p-66),
            // cspell: enable
            // NOLINTEND
        });
        constexpr auto outputs = [] constexpr {
            std::array<std::tuple<double, double>, inputs.size()> results{};
            for (std::size_t i = 0; i < inputs.size(); ++i) {
                const auto& [a, b, s, e] = inputs[i];
                results[i] = two_prod_no_fma(a, b);
            }
            return results;
        }();

        for (std::size_t i = 0; i < inputs.size(); ++i) {
            INFO("i = " << i);
            const auto& [a, b, s_true, e_true] = inputs[i];
            const auto& [s, e] = outputs[i];
            CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
            CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
        }
    }

    SECTION("calculate with zeros") {
        constexpr double a = 0.0;
        constexpr double b = 0.0;
        constexpr double s_true = 0.0;
        constexpr double e_true = 0.0;
        const auto [s, e] = two_prod_no_fma(a, b);
        CHECK(s == s_true);  // NOLINT
        CHECK(e == e_true);  // NOLINT
    }
}

#ifdef NUM_COLLECT_MULTI_DOUBLE_HAS_AVX2_FMA

TEST_CASE("num_collect::multi_double::impl::two_prod_fma") {
    using num_collect::multi_double::impl::two_prod_fma;

    SECTION("calculate product without error") {
        constexpr double a = 0x1.000001p+1;
        constexpr double b = 0x1.0000001p+2;
        constexpr double p_true = 0x1.0000011000001p+3;
        constexpr double e_true = 0.0;
        const auto [p, e] = two_prod_fma(a, b);
        CHECK_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with error") {
        constexpr double a = 0x1.0000001p+1;
        constexpr double b = 0x1.0000008p-2;
        constexpr double p_true = 0x1.0000009p-1;
        constexpr double e_true = 0x1.0p-54;
        const auto [p, e] = two_prod_fma(a, b);
        CHECK_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with small error") {
        constexpr double a = 0x1.0000000000001p+1;
        constexpr double b = 0x1.0000000000001p-2;
        constexpr double p_true = 0x1.0000000000002p-1;
        constexpr double e_true = 0x1.0p-105;
        const auto [p, e] = two_prod_fma(a, b);
        CHECK_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("random problems") {
        double a{};
        double b{};
        double s_true{};
        double e_true{};
        std::tie(a, b, s_true, e_true) =
            GENERATE(Catch::Generators::table<double, double, double, double>({
                // NOLINTBEGIN
                // cspell: disable
                std::make_tuple(0x1.0eda2a085a8b0p+16, 0x1.35e2911f3e380p-10,
                    0x1.47dd0adfd0b7ap+6, -0x1.0b8b34c3cc000p-49),
                std::make_tuple(-0x1.1e20cf1e86a2cp-15, 0x1.9302680dbea10p+9,
                    -0x1.c270569b5d0e8p-6, 0x1.b828673249500p-60),
                std::make_tuple(0x1.6ebd2ea984d78p+8, -0x1.83fea770287bcp-21,
                    -0x1.15ea668e531bbp-12, -0x1.6c2fbf7465840p-66),
                // cspell: enable
                // NOLINTEND
            }));
        const auto [s, e] = two_prod_fma(a, b);
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate with zeros") {
        constexpr double a = 0.0;
        constexpr double b = 0.0;
        constexpr double s_true = 0.0;
        constexpr double e_true = 0.0;
        const auto [s, e] = two_prod_fma(a, b);
        CHECK(s == s_true);  // NOLINT
        CHECK(e == e_true);  // NOLINT
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
        CHECK_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with error") {
        constexpr double a = 0x1.0000001p+1;
        constexpr double b = 0x1.0000008p-2;
        constexpr double p_true = 0x1.0000009p-1;
        constexpr double e_true = 0x1.0p-54;
        const auto [p, e] = two_prod(a, b);
        CHECK_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate product with small error") {
        constexpr double a = 0x1.0000000000001p+1;
        constexpr double b = 0x1.0000000000001p-2;
        constexpr double p_true = 0x1.0000000000002p-1;
        constexpr double e_true = 0x1.0p-105;
        const auto [p, e] = two_prod(a, b);
        CHECK_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("random problems") {
        double a{};
        double b{};
        double s_true{};
        double e_true{};
        std::tie(a, b, s_true, e_true) =
            GENERATE(Catch::Generators::table<double, double, double, double>({
                // NOLINTBEGIN
                // cspell: disable
                std::make_tuple(0x1.0eda2a085a8b0p+16, 0x1.35e2911f3e380p-10,
                    0x1.47dd0adfd0b7ap+6, -0x1.0b8b34c3cc000p-49),
                std::make_tuple(-0x1.1e20cf1e86a2cp-15, 0x1.9302680dbea10p+9,
                    -0x1.c270569b5d0e8p-6, 0x1.b828673249500p-60),
                std::make_tuple(0x1.6ebd2ea984d78p+8, -0x1.83fea770287bcp-21,
                    -0x1.15ea668e531bbp-12, -0x1.6c2fbf7465840p-66),
                // cspell: enable
                // NOLINTEND
            }));
        const auto [s, e] = two_prod(a, b);
        CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
        CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
    }

    SECTION("calculate at compile time") {
        static constexpr auto inputs = std::to_array({
            // NOLINTBEGIN
            // cspell: disable
            std::make_tuple(0x1.0eda2a085a8b0p+16, 0x1.35e2911f3e380p-10,
                0x1.47dd0adfd0b7ap+6, -0x1.0b8b34c3cc000p-49),
            std::make_tuple(-0x1.1e20cf1e86a2cp-15, 0x1.9302680dbea10p+9,
                -0x1.c270569b5d0e8p-6, 0x1.b828673249500p-60),
            std::make_tuple(0x1.6ebd2ea984d78p+8, -0x1.83fea770287bcp-21,
                -0x1.15ea668e531bbp-12, -0x1.6c2fbf7465840p-66),
            // cspell: enable
            // NOLINTEND
        });
        constexpr auto outputs = [] constexpr {
            std::array<std::tuple<double, double>, inputs.size()> results{};
            for (std::size_t i = 0; i < inputs.size(); ++i) {
                const auto& [a, b, s, e] = inputs[i];
                results[i] = two_prod(a, b);
            }
            return results;
        }();

        for (std::size_t i = 0; i < inputs.size(); ++i) {
            INFO("i = " << i);
            const auto& [a, b, s_true, e_true] = inputs[i];
            const auto& [s, e] = outputs[i];
            CHECK_THAT(s, Catch::Matchers::WithinULP(s_true, 0));
            CHECK_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
        }
    }

    SECTION("calculate with zeros") {
        constexpr double a = 0.0;
        constexpr double b = 0.0;
        constexpr double s_true = 0.0;
        constexpr double e_true = 0.0;
        const auto [s, e] = two_prod(a, b);
        CHECK(s == s_true);  // NOLINT
        CHECK(e == e_true);  // NOLINT
    }
}
