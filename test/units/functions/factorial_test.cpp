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
 * \brief Test of factorial function.
 */
#include "num_collect/functions/factorial.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "check_constexpr_function.h"

TEMPLATE_TEST_CASE_SIG("num_collect::functions::factorial", "",
    ((typename Result, typename Integer), Result, Integer),
    // Integer to integer.
    (int, int), (unsigned int, unsigned int),
    // Integer to floating-point number.
    (double, int), (double, unsigned int)) {
    SECTION("n = 0") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<Integer>(0)),
            num_collect::functions::factorial<Result>,
            [](Integer /*input*/) -> Result { return static_cast<Result>(1); });
    }

    SECTION("n = 1") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<Integer>(1)),
            num_collect::functions::factorial<Result>,
            [](Integer /*input*/) -> Result { return static_cast<Result>(1); });
    }

    SECTION("n = 2") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<Integer>(2)),
            num_collect::functions::factorial<Result>,
            [](Integer /*input*/) -> Result { return static_cast<Result>(2); });
    }

    SECTION("n = 3") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<Integer>(3)),
            num_collect::functions::factorial<Result>,
            [](Integer /*input*/) -> Result { return static_cast<Result>(6); });
    }

    SECTION("n = 4") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<Integer>(4)),
            num_collect::functions::factorial<Result>,
            [](Integer /*input*/) -> Result {
                return static_cast<Result>(24);
            });
    }

    SECTION("n = 5") {
        CHECK_CONSTEXPR_FUNCTION_EQUAL((static_cast<Integer>(5)),
            num_collect::functions::factorial<Result>,
            [](Integer /*input*/) -> Result {
                return static_cast<Result>(120);
            });
    }

    SECTION("n = -1") {
        if constexpr (std::signed_integral<Integer>) {
            constexpr auto n = static_cast<Integer>(-1);
            constexpr auto value_at_compile_time =
                num_collect::functions::factorial<Result>(n);
            const auto value_runtime =
                num_collect::functions::factorial<Result>(n);
            if constexpr (std::floating_point<Result>) {
                CHECK(std::isnan(value_at_compile_time));
                CHECK(std::isnan(value_runtime));
            } else {
                constexpr auto true_val = static_cast<Result>(0);
                CHECK(value_at_compile_time == true_val);
                CHECK(value_runtime == true_val);
            }
        }
    }
}
