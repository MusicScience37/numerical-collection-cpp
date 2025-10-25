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
 * \brief Test of log1m_maclaurin function.
 */
#include "num_collect/constants/impl/log1m_maclaurin.h"

#include <cmath>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../check_constexpr_function.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::constants::impl::log1m_maclaurin", "", float, double) {
    const auto log1m_reference = [](TestType x) -> TestType {
        return std::log1p(-x);
    };

    SECTION("x = 0") {
        CHECK_CONSTEXPR_FUNCTION_ABSOLUTE((static_cast<TestType>(0)),
            num_collect::constants::impl::log1m_maclaurin, log1m_reference);
    }

    SECTION("x = 0.1") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.1)),
            num_collect::constants::impl::log1m_maclaurin, log1m_reference);
    }

    SECTION("x = 0.2") {
        CHECK_CONSTEXPR_FUNCTION_RELATIVE((static_cast<TestType>(0.2)),
            num_collect::constants::impl::log1m_maclaurin, log1m_reference);
    }
}
