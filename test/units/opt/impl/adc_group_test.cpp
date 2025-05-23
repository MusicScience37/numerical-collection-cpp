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
 * \brief Test of adc_group class.
 */
#include "num_collect/opt/impl/adc_group.h"

#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "num_collect/opt/impl/adc_ternary_vector.h"

TEST_CASE("num_collect::opt::impl::adc_group") {
    using group_type = num_collect::opt::impl::adc_group<double>;
    using rectangle_type = typename group_type::rectangle_type;
    using num_collect::opt::impl::adc_ternary_vector;

    constexpr double dist = 0.1;
    auto group = group_type(dist);

    SECTION("construct") {
        REQUIRE_THAT(group.dist(), Catch::Matchers::WithinRel(dist));
        REQUIRE(group.empty());
    }

    SECTION("push a rectangle") {
        auto point1 = adc_ternary_vector(2);
        point1.push_back(0, 0);
        point1.push_back(0, 1);
        point1.push_back(1, 0);
        constexpr double ave_value1 = 3.14;
        group.push(std::make_shared<rectangle_type>(point1, ave_value1));

        REQUIRE(group.min_rect()->vertex() == point1);
        REQUIRE_FALSE(group.empty());
    }

    SECTION("push rectangles") {
        auto point1 = adc_ternary_vector(2);
        point1.push_back(0, 0);
        point1.push_back(0, 1);
        point1.push_back(1, 0);
        constexpr double ave_value1 = 3.14;
        group.push(std::make_shared<rectangle_type>(point1, ave_value1));

        auto point2 = adc_ternary_vector(2);
        point2.push_back(0, 0);
        point2.push_back(1, 0);
        constexpr double ave_value2 = 1.23;
        group.push(std::make_shared<rectangle_type>(point2, ave_value2));

        REQUIRE(group.min_rect()->vertex() == point2);
        REQUIRE_FALSE(group.empty());
    }

    SECTION("pop rectangles") {
        auto point1 = adc_ternary_vector(2);
        point1.push_back(0, 0);
        point1.push_back(0, 1);
        point1.push_back(1, 0);
        constexpr double ave_value1 = 3.14;
        group.push(std::make_shared<rectangle_type>(point1, ave_value1));

        auto point2 = adc_ternary_vector(2);
        point2.push_back(0, 0);
        point2.push_back(1, 0);
        constexpr double ave_value2 = 1.23;
        group.push(std::make_shared<rectangle_type>(point2, ave_value2));

        auto popped_rect = group.pop();
        REQUIRE(popped_rect->vertex() == point2);
        REQUIRE(group.min_rect()->vertex() == point1);
        REQUIRE_FALSE(group.empty());

        popped_rect = group.pop();
        REQUIRE(popped_rect->vertex() == point1);
        REQUIRE(group.empty());
    }
}
