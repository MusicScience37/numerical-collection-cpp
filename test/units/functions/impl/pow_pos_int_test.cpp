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
 * \brief Test of pow_pos_int function.
 */
#include "num_collect/functions/impl/pow_pos_int.h"

#include <cmath>
#include <utility>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../check_constexpr_function.h"

TEMPLATE_TEST_CASE("num_collect::functions::impl::pow_pos_int", "",
    (std::pair<float, int>), (std::pair<double, unsigned int>)) {
    using num_collect::functions::impl::pow_pos_int;
    using Base = typename TestType::first_type;
    using Exp = typename TestType::second_type;

    SECTION("for exponent = 0") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(1.234), static_cast<Exp>(0)), pow_pos_int,
            std::pow);
    }

    SECTION("for exponent = 1") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(1.234), static_cast<Exp>(1)), pow_pos_int,
            std::pow);
    }

    SECTION("for exponent = 2") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(1.234), static_cast<Exp>(2)), pow_pos_int,
            std::pow);
    }

    SECTION("for exponent = 3") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(1.234), static_cast<Exp>(3)), pow_pos_int,
            std::pow);
    }

    SECTION("for exponent = 4") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(1.234), static_cast<Exp>(4)), pow_pos_int,
            std::pow);
    }

    SECTION("for exponent = 5") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(1.234), static_cast<Exp>(5)), pow_pos_int,
            std::pow);
    }

    SECTION("for exponent = 6") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(1.234), static_cast<Exp>(6)), pow_pos_int,
            std::pow);
    }

    SECTION("for exponent = 10") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(1.234), static_cast<Exp>(10)), pow_pos_int,
            std::pow);
    }

    SECTION("for exponent = 100") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(1.234), static_cast<Exp>(100)), pow_pos_int,
            std::pow);
    }
}
