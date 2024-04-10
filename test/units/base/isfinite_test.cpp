/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of isfinite function.
 */
#include "num_collect/base/isfinite.h"

#include <cmath>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::base::isfinite") {
    using num_collect::base::isfinite;

    SECTION("check float") {
        CHECK(isfinite(1.2F));                                 // NOLINT
        CHECK_FALSE(isfinite(static_cast<float>(NAN)));        // NOLINT
        CHECK_FALSE(isfinite(static_cast<float>(INFINITY)));   // NOLINT
        CHECK_FALSE(isfinite(static_cast<float>(-INFINITY)));  // NOLINT
    }

    SECTION("check double") {
        CHECK(isfinite(1.2));                                   // NOLINT
        CHECK_FALSE(isfinite(static_cast<double>(NAN)));        // NOLINT
        CHECK_FALSE(isfinite(static_cast<double>(INFINITY)));   // NOLINT
        CHECK_FALSE(isfinite(static_cast<double>(-INFINITY)));  // NOLINT
    }

    SECTION("check complex<double>") {
        CHECK(isfinite(std::complex<double>(1.2, 2.3)));              // NOLINT
        CHECK_FALSE(isfinite(std::complex<double>(NAN, 2.3)));        // NOLINT
        CHECK_FALSE(isfinite(std::complex<double>(1.2, NAN)));        // NOLINT
        CHECK_FALSE(isfinite(std::complex<double>(INFINITY, 2.3)));   // NOLINT
        CHECK_FALSE(isfinite(std::complex<double>(1.2, INFINITY)));   // NOLINT
        CHECK_FALSE(isfinite(std::complex<double>(-INFINITY, 2.3)));  // NOLINT
        CHECK_FALSE(isfinite(std::complex<double>(1.2, -INFINITY)));  // NOLINT
    }
}
