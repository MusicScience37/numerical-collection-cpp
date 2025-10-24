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
 * \brief Test of root function.
 */
#include "num_collect/constants/root.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "check_constexpr_function.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::impl::root_at_compile_time", "", float, double) {
    const auto reference_function = [](TestType x, int n) -> TestType {
        if (x < static_cast<TestType>(0)) {
            return -static_cast<TestType>(std::pow(
                -static_cast<double>(x), 1.0 / static_cast<double>(n)));
        }
        return static_cast<TestType>(
            std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n)));
    };

    SECTION("x < 0, n is even") {
        constexpr auto x = static_cast<TestType>(-1.0);
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr TestType value_at_compile_time =
                num_collect::constants::impl::root_at_compile_time(x, n);
            const TestType value_runtime =
                num_collect::constants::impl::root_at_compile_time(x, n);
            CHECK(std::isnan(value_at_compile_time));
            CHECK(std::isnan(value_runtime));
        }
        SECTION("n = 4") {
            constexpr auto n = 4;
            constexpr TestType value_at_compile_time =
                num_collect::constants::impl::root_at_compile_time(x, n);
            const TestType value_runtime =
                num_collect::constants::impl::root_at_compile_time(x, n);
            CHECK(std::isnan(value_at_compile_time));
            CHECK(std::isnan(value_runtime));
        }
    }

    SECTION("x < 0, n is odd") {
        constexpr auto x = static_cast<TestType>(-1.234);
        SECTION("n = 3") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(3)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
        SECTION("n = 5") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(5)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
        SECTION("n = 11") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(11)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
    }

    SECTION("x = 0") {
        constexpr auto x = static_cast<TestType>(0);
        SECTION("n = 2") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(2)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
        SECTION("n = 3") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(3)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
        SECTION("n = 4") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(4)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
    }

    SECTION("x > 0") {
        constexpr auto x = static_cast<TestType>(1.234);
        SECTION("n = 2") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(2)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
        SECTION("n = 3") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(3)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
        SECTION("n = 4") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(4)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
        SECTION("n = 5") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(5)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
        SECTION("n = 10") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(10)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
        SECTION("n = 11") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(11)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
    }

    SECTION("x >> 0") {
        constexpr auto x = static_cast<TestType>(1.234e+10);
        SECTION("n = 2") {
            constexpr auto n = 2;
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(n)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(n)),
                num_collect::constants::impl::root_at_compile_time,
                reference_function);
        }
    }

    SECTION("x == infinity") {
        constexpr auto x = std::numeric_limits<TestType>::infinity();
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr TestType value_at_compile_time =
                num_collect::constants::impl::root_at_compile_time(x, n);
            const TestType value_runtime =
                num_collect::constants::impl::root_at_compile_time(x, n);
            CHECK(std::isinf(value_at_compile_time));
            CHECK(
                value_at_compile_time > num_collect::constants::zero<TestType>);
            CHECK(std::isinf(value_runtime));
            CHECK(value_runtime > num_collect::constants::zero<TestType>);
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr TestType value_at_compile_time =
                num_collect::constants::impl::root_at_compile_time(x, n);
            const TestType value_runtime =
                num_collect::constants::impl::root_at_compile_time(x, n);
            CHECK(std::isinf(value_at_compile_time));
            CHECK(
                value_at_compile_time > num_collect::constants::zero<TestType>);
            CHECK(std::isinf(value_runtime));
            CHECK(value_runtime > num_collect::constants::zero<TestType>);
        }
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("num_collect::constants::root", "", float, double) {
    const auto reference_function = [](TestType x, int n) -> TestType {
        if (x < static_cast<TestType>(0)) {
            return -static_cast<TestType>(std::pow(
                -static_cast<double>(x), 1.0 / static_cast<double>(n)));
        }
        return static_cast<TestType>(
            std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n)));
    };

    SECTION("x < 0, n is even") {
        constexpr auto x = static_cast<TestType>(-1.0);
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr TestType value_at_compile_time =
                num_collect::constants::root(x, n);
            const TestType value_runtime = num_collect::constants::root(x, n);
            CHECK(std::isnan(value_at_compile_time));
            CHECK(std::isnan(value_runtime));
        }
        SECTION("n = 4") {
            constexpr auto n = 4;
            constexpr TestType value_at_compile_time =
                num_collect::constants::root(x, n);
            const TestType value_runtime = num_collect::constants::root(x, n);
            CHECK(std::isnan(value_at_compile_time));
            CHECK(std::isnan(value_runtime));
        }
    }

    SECTION("x < 0, n is odd") {
        constexpr auto x = static_cast<TestType>(-1.234);
        SECTION("n = 3") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(3)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 5") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(5)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 11") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(11)),
                num_collect::constants::root, reference_function);
        }
    }

    SECTION("x = 0") {
        constexpr auto x = static_cast<TestType>(0);
        SECTION("n = 2") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(2)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 3") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(3)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 4") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(4)),
                num_collect::constants::root, reference_function);
        }
    }

    SECTION("x > 0") {
        constexpr auto x = static_cast<TestType>(1.234);
        SECTION("n = 2") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(2)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 3") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(3)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 4") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(4)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 5") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(5)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 10") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(10)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 11") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(11)),
                num_collect::constants::root, reference_function);
        }
    }

    SECTION("x >> 0") {
        constexpr auto x = static_cast<TestType>(1.234e+10);
        SECTION("n = 2") {
            constexpr auto n = 2;
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(n)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(n)),
                num_collect::constants::root, reference_function);
        }
    }

    SECTION("x == infinity") {
        constexpr auto x = std::numeric_limits<TestType>::infinity();
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr TestType value_at_compile_time =
                num_collect::constants::root(x, n);
            const TestType value_runtime = num_collect::constants::root(x, n);
            CHECK(std::isinf(value_at_compile_time));
            CHECK(
                value_at_compile_time > num_collect::constants::zero<TestType>);
            CHECK(std::isinf(value_runtime));
            CHECK(value_runtime > num_collect::constants::zero<TestType>);
        }
        SECTION("n = 3") {
            constexpr auto n = 3;
            constexpr TestType value_at_compile_time =
                num_collect::constants::root(x, n);
            const TestType value_runtime = num_collect::constants::root(x, n);
            CHECK(std::isinf(value_at_compile_time));
            CHECK(
                value_at_compile_time > num_collect::constants::zero<TestType>);
            CHECK(std::isinf(value_runtime));
            CHECK(value_runtime > num_collect::constants::zero<TestType>);
        }
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::root (integers)", "", int, long long) {
    const auto reference_function = [](TestType x, int n) -> double {
        if (x < static_cast<TestType>(0)) {
            return -std::pow(
                -static_cast<double>(x), 1.0 / static_cast<double>(n));
        }
        return std::pow(static_cast<double>(x), 1.0 / static_cast<double>(n));
    };

    SECTION("x < 0, n is even") {
        constexpr auto x = static_cast<TestType>(-2);
        SECTION("n = 2") {
            constexpr auto n = 2;
            constexpr double value_at_compile_time =
                num_collect::constants::root(x, n);
            const double value_runtime = num_collect::constants::root(x, n);
            CHECK(std::isnan(value_at_compile_time));
            CHECK(std::isnan(value_runtime));
        }
        SECTION("n = 4") {
            constexpr auto n = 4;
            constexpr double value_at_compile_time =
                num_collect::constants::root(x, n);
            const double value_runtime = num_collect::constants::root(x, n);
            CHECK(std::isnan(value_at_compile_time));
            CHECK(std::isnan(value_runtime));
        }
    }

    SECTION("x < 0, n is odd") {
        constexpr auto x = static_cast<TestType>(-5);
        SECTION("n = 3") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(3)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 5") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(5)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 11") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(11)),
                num_collect::constants::root, reference_function);
        }
    }

    SECTION("x = 0") {
        constexpr auto x = static_cast<TestType>(0);
        // true_val not needed when using macro below
        SECTION("n = 2") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(2)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 3") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(3)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 4") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(4)),
                num_collect::constants::root, reference_function);
        }
    }

    SECTION("x > 0") {
        constexpr auto x = static_cast<TestType>(123);
        SECTION("n = 2") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(2)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 3") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(3)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 4") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(4)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 5") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(5)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 10") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(10)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 11") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(11)),
                num_collect::constants::root, reference_function);
        }
    }

    SECTION("x >> 0") {
        constexpr auto x = static_cast<TestType>(1234566789);
        SECTION("n = 2") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(2)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 3") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(3)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 4") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(4)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 5") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(5)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 10") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(10)),
                num_collect::constants::root, reference_function);
        }
        SECTION("n = 11") {
            CHECK_CONSTEXPR_FUNCTION_RELATIVE((x, static_cast<int>(11)),
                num_collect::constants::root, reference_function);
        }
    }
}
