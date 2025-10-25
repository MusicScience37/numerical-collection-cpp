/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of step_size_limits class.
 */
#include "num_collect/ode/step_size_limits.h"

#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("num_collect::ode::step_size_limits") {
    using num_collect::ode::step_size_limits;

    SECTION("apply default limits") {
        const auto limits = step_size_limits<double>();

        CHECK_THAT(limits.apply(0.9), Catch::Matchers::WithinRel(0.9));

        CHECK_THAT(limits.apply(1.0), Catch::Matchers::WithinRel(1.0));

        CHECK_THAT(limits.apply(1.1), Catch::Matchers::WithinRel(1.0));

        const double lower_limit =
            std::sqrt(std::numeric_limits<double>::epsilon());

        CHECK_THAT(limits.apply(lower_limit * 1.1),
            Catch::Matchers::WithinRel(lower_limit * 1.1));

        CHECK_THAT(limits.apply(lower_limit * 1.0),
            Catch::Matchers::WithinRel(lower_limit * 1.0));

        CHECK_THAT(limits.apply(lower_limit * 0.9),
            Catch::Matchers::WithinRel(lower_limit * 1.0));
    }
}
