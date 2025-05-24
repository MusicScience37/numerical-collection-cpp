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
    constexpr num_collect::index_type dim = 2;
    constexpr num_collect::index_type max_digits = 3;
    using variable_type = Eigen::Vector<double, dim>;
    using ternary_vector_type =
        num_collect::opt::impl::adc_ternary_vector<variable_type, max_digits>;
    using rectangle_type =
        num_collect::opt::impl::adc_rectangle<double, ternary_vector_type>;

    SECTION("construct") {
        auto vertex = ternary_vector_type(dim);
        vertex.push_back(0);
        vertex.push_back(0);
        vertex.push_back(1);
        constexpr double ave_value = 3.14;
        const auto rect = rectangle_type(vertex, ave_value);

        CHECK(rect.vertex() == vertex);
        CHECK_THAT(rect.ave_value(), Catch::Matchers::WithinRel(ave_value));

        auto sample_points = std::make_pair(vertex, vertex);
        sample_points.first(0, 1) =
            static_cast<typename ternary_vector_type::digit_type>(2);
        sample_points.second(1, 0) =
            static_cast<typename ternary_vector_type::digit_type>(1);
        CHECK(rect.sample_points() == sample_points);

        const double dist = 0.5 * std::sqrt(1.0 / 9.0 + 1.0);
        CHECK_THAT(rect.dist(), Catch::Matchers::WithinRel(dist));
    }
}
