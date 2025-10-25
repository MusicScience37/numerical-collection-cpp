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
 * \brief Test of pow function.
 */
#include "num_collect/functions/pow.h"

#include <cmath>
#include <utility>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "check_constexpr_function.h"

TEMPLATE_TEST_CASE("num_collect::functions::pow(Float, Int)", "",
    (std::pair<float, long long>), (std::pair<double, int>)) {
    using Base = typename TestType::first_type;
    using Exp = typename TestType::second_type;

    SECTION("ordinary") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(4.321), static_cast<Exp>(3)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("negative exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(4.321), static_cast<Exp>(-3)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("low exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(4.321), static_cast<Exp>(-30)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("zero exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(4.321), static_cast<Exp>(0)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("large exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(4.321), static_cast<Exp>(30)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("negative base") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(-4.321), static_cast<Exp>(3)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("low base") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(-4.321e+5), static_cast<Exp>(3)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("large base") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<Base>(4.321e+5), static_cast<Exp>(3)),
            num_collect::functions::pow, std::pow);
    }
}

TEMPLATE_TEST_CASE(
    "num_collect::functions::impl::pow_at_compile_time(Float, Float)", "",
    float, double) {
    SECTION("ordinary") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(4.321), static_cast<TestType>(1.234)),
            num_collect::functions::impl::pow_at_compile_time, std::pow);
    }

    SECTION("negative exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(4.321), static_cast<TestType>(-2.345)),
            num_collect::functions::impl::pow_at_compile_time, std::pow);
    }

    SECTION("low exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(1.234), static_cast<TestType>(-123.456)),
            num_collect::functions::impl::pow_at_compile_time, std::pow);
    }

    SECTION("zero exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(4.321), static_cast<TestType>(0)),
            num_collect::functions::impl::pow_at_compile_time, std::pow);
    }

    SECTION("large exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(1.234), static_cast<TestType>(123.456)),
            num_collect::functions::impl::pow_at_compile_time, std::pow);
    }

    SECTION("small base") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(4.321e-10), static_cast<TestType>(1.234)),
            num_collect::functions::impl::pow_at_compile_time, std::pow);
    }

    SECTION("large base") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(4.321e+10), static_cast<TestType>(1.234)),
            num_collect::functions::impl::pow_at_compile_time, std::pow);
    }
}

TEMPLATE_TEST_CASE(
    "num_collect::functions::pow(Float, Float)", "", float, double) {
    SECTION("ordinary") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(4.321), static_cast<TestType>(1.234)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("negative exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(4.321), static_cast<TestType>(-2.345)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("low exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(1.234), static_cast<TestType>(-123.456)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("zero exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(4.321), static_cast<TestType>(0)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("large exponent") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(1.234), static_cast<TestType>(123.456)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("small base") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(4.321e-10), static_cast<TestType>(1.234)),
            num_collect::functions::pow, std::pow);
    }

    SECTION("large base") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE(
            (static_cast<TestType>(4.321e+10), static_cast<TestType>(1.234)),
            num_collect::functions::pow, std::pow);
    }
}
