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

#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

TEST_CASE("num_collect::multi_double::impl::two_prod") {
    SECTION("calculate product with error") {
        constexpr double a = 0x1.0000001p+1;
        constexpr double b = 0x1.0000008p-2;
        constexpr double p_true = 0x1.0000009p-1;
        constexpr double e_true = 0x1.0p-54;

        SECTION("two_prod_no_fma") {
            using num_collect::multi_double::impl::two_prod_no_fma;
            const auto [p, e] = two_prod_no_fma(a, b);
            REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
            REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
            BENCHMARK("two_prod_no_fma") { return two_prod_no_fma(a, b); };
        }

#ifdef __AVX2__
        SECTION("two_prod_fma") {
            using num_collect::multi_double::impl::two_prod_fma;
            const auto [p, e] = two_prod_fma(a, b);
            REQUIRE_THAT(p, Catch::Matchers::WithinULP(p_true, 0));
            REQUIRE_THAT(e, Catch::Matchers::WithinULP(e_true, 0));
            BENCHMARK("two_prod_fma") { return two_prod_fma(a, b); };
        }
#endif
    }
}
