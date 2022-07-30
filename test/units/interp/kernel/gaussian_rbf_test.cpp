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
 * \brief Test of gaussian_rbf class.
 */
#include "num_collect/interp/kernel/gaussian_rbf.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/interp/kernel/concepts/rbf.h"  // IWYU pragma: keep

TEST_CASE("num_collect::interp::kernel::gaussian_rbf") {
    using num_collect::interp::kernel::gaussian_rbf;

    STATIC_REQUIRE(
        num_collect::interp::kernel::concepts::rbf<gaussian_rbf<double>>);

    SECTION("calculate") {
        const auto rbf = gaussian_rbf<double>();
        constexpr double arg = 1.234;
        const double value = std::exp(-arg * arg);
        REQUIRE_THAT(rbf(arg), Catch::Matchers::WithinRel(value));
    }
}
