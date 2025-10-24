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
 * \brief Test of exp_maclaurin function.
 */
#include "num_collect/constants/impl/exp_maclaurin.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../check_constexpr_function.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::impl::exp_maclaurin", "", float, double) {
    using num_collect::constants::impl::exp_maclaurin;

    SECTION("x = 0") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(0)), exp_maclaurin, std::exp);
    }

    SECTION("x = 0.2") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(0.2)), exp_maclaurin, std::exp);
    }

    SECTION("x = 0.5") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(0.5)), exp_maclaurin, std::exp);
    }

    SECTION("x = 1") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(1)), exp_maclaurin, std::exp);
    }
}
