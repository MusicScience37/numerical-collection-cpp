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
 * \brief Test of gauss_legendre_integrator class.
 */
#include "num_collect/integration/gauss_legendre_integrator.h"

#include <cmath>
#include <complex>
#include <limits>
#include <ostream>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/constants/napier.h"  // IWYU pragma: keep
#include "num_collect/constants/pi.h"      // IWYU pragma: keep

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::integration::gauss_legendre_integrator", "", float, double) {
    SECTION("integrate cos") {
        const auto integrator =
            num_collect::integration::gauss_legendre_integrator<TestType(
                TestType)>();

        constexpr auto left = static_cast<TestType>(0);
        constexpr auto right =
            static_cast<TestType>(0.5) * num_collect::constants::pi<TestType>;
        const auto val =
            integrator([](TestType x) { return std::cos(x); }, left, right);

        const auto true_val = static_cast<TestType>(1);
        constexpr auto tol = std::numeric_limits<TestType>::epsilon() *
            static_cast<TestType>(1e+4);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val, tol));
    }

    SECTION("integrate exp") {
        const auto integrator =
            num_collect::integration::gauss_legendre_integrator<TestType(
                TestType)>();

        constexpr auto left = static_cast<TestType>(0);
        constexpr auto right = static_cast<TestType>(1);
        const auto val =
            integrator([](TestType x) { return std::exp(x); }, left, right);

        const TestType true_val = num_collect::constants::napier<TestType> -
            num_collect::constants::one<TestType>;
        constexpr auto tol = std::numeric_limits<TestType>::epsilon() *
            static_cast<TestType>(1e+4);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val, tol));
    }

    SECTION("integrate x^(3/2)") {
        constexpr num_collect::index_type degree = 50;
        const auto integrator =
            num_collect::integration::gauss_legendre_integrator<TestType(
                TestType)>(degree);

        constexpr auto left = static_cast<TestType>(0);
        constexpr auto right = static_cast<TestType>(1);
        const auto val = integrator(
            [](TestType x) {
                constexpr auto exponent = static_cast<TestType>(1.5);
                return std::pow(x, exponent);
            },
            left, right);

        const auto true_val = static_cast<TestType>(0.4);
        const auto tol = std::sqrt(std::numeric_limits<TestType>::epsilon());
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val, tol));
    }

    SECTION("integrate half circle") {
        auto integrator =
            num_collect::integration::gauss_legendre_integrator<TestType(
                TestType)>();
        constexpr num_collect::index_type degree = 50;
        integrator.prepare(degree);

        constexpr auto left = static_cast<TestType>(-1);
        constexpr auto right = static_cast<TestType>(1);
        const auto val = integrator(
            [](TestType x) {
                return std::sqrt(static_cast<TestType>(1) - x * x);
            },
            left, right);

        const auto true_val =
            static_cast<TestType>(0.5) * num_collect::constants::pi<TestType>;
        constexpr auto tol = static_cast<TestType>(1e-4);
        REQUIRE_THAT(val, Catch::Matchers::WithinRel(true_val, tol));
    }

    SECTION("integrate exp(ix)") {
        auto integrator = num_collect::integration::gauss_legendre_integrator<
            std::complex<TestType>(TestType)>();

        constexpr auto left = static_cast<TestType>(0);
        constexpr auto right =
            static_cast<TestType>(2) * num_collect::constants::pi<TestType>;
        const auto val = integrator(
            [](TestType x) {
                return std::exp(
                    std::complex<TestType>(static_cast<TestType>(0), x));
            },
            left, right);

        constexpr auto tol = std::numeric_limits<TestType>::epsilon() *
            static_cast<TestType>(1e+4);
        REQUIRE_THAT(val.real(),
            Catch::Matchers::WithinAbs(static_cast<TestType>(0), tol));
        REQUIRE_THAT(val.imag(),
            Catch::Matchers::WithinAbs(static_cast<TestType>(0), tol));
    }
}
