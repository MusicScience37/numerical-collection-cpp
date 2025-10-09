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
 * \brief Test of basic operations in multi-double calculations.
 */
#include <string>
#include <tuple>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/do_not_optimize.h>

#include "num_collect/multi_double/impl/basic_operations.h"

STAT_BENCH_MAIN

constexpr double a = 0x1.0000001p+1;
constexpr double b = 0x1.0000008p-2;

// NOLINTNEXTLINE
STAT_BENCH_CASE("multi_double_two_prod", "two_prod_no_fma") {
    using num_collect::multi_double::impl::two_prod_no_fma;
    STAT_BENCH_MEASURE() {
        stat_bench::do_not_optimize(two_prod_no_fma(a, b));
    };
}

#ifdef NUM_COLLECT_MULTI_DOUBLE_HAS_AVX2_FMA
// NOLINTNEXTLINE
STAT_BENCH_CASE("multi_double_two_prod", "two_prod_fma") {
    using num_collect::multi_double::impl::two_prod_fma;
    STAT_BENCH_MEASURE() { stat_bench::do_not_optimize(two_prod_fma(a, b)); };
}
#endif
