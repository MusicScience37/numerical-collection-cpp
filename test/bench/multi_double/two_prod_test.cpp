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
#include <celero/Celero.h>

#include "num_collect/multi_double/impl/basic_operations.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

// NOLINTNEXTLINE: external library
BASELINE(multi_double_two_prod, two_prod_no_fma, 0, 0) {
    constexpr double a = 0x1.0000001p+1;
    constexpr double b = 0x1.0000008p-2;
    constexpr double p_true = 0x1.0000009p-1;
    constexpr double e_true = 0x1.0p-54;
    using num_collect::multi_double::impl::two_prod_no_fma;
    celero::DoNotOptimizeAway(two_prod_no_fma(a, b));
}

#ifdef __AVX2__
// NOLINTNEXTLINE: external library
BENCHMARK(multi_double_two_prod, two_prod_fma, 0, 0) {
    constexpr double a = 0x1.0000001p+1;
    constexpr double b = 0x1.0000008p-2;
    constexpr double p_true = 0x1.0000009p-1;
    constexpr double e_true = 0x1.0p-54;
    using num_collect::multi_double::impl::two_prod_fma;
    celero::DoNotOptimizeAway(two_prod_fma(a, b));
}
#endif
