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
 * \brief Test of adc_rectangle class.
 */
#include "num_collect/opt/impl/adc_rectangle.h"

#include <cmath>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/opt/impl/adc_ternary_vector.h"

TEST_CASE("num_collect::opt::impl::adc_rectangle") {
    using rectangle_type = num_collect::opt::impl::adc_rectangle<double>;
    using num_collect::opt::impl::adc_ternary_vector;

    SECTION("construct") {
        auto vertex = adc_ternary_vector(2);
        vertex.push_back(0, 0);
        vertex.push_back(0, 1);
        vertex.push_back(1, 0);
        constexpr double ave_value = 3.14;
        const auto rect = rectangle_type(vertex, ave_value);

        REQUIRE(rect.vertex() == vertex);
        REQUIRE_THAT(rect.ave_value(), Catch::Matchers::WithinRel(ave_value));

        auto sample_points = std::make_pair(vertex, vertex);
        sample_points.first(0, 1) =
            static_cast<adc_ternary_vector::digit_type>(2);
        sample_points.second(1, 0) =
            static_cast<adc_ternary_vector::digit_type>(1);
        REQUIRE(rect.sample_points() == sample_points);

        const double dist = 0.5 * std::sqrt(1.0 / 9.0 + 1.0);
        REQUIRE_THAT(rect.dist(), Catch::Matchers::WithinRel(dist));
    }
}
