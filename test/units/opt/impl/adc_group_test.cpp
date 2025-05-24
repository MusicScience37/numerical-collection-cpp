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
    constexpr num_collect::index_type dim = 2;
    constexpr num_collect::index_type max_digits = 2;
    using variable_type = Eigen::Vector<double, dim>;
    using ternary_vector_type =
        num_collect::opt::impl::adc_ternary_vector<variable_type, max_digits>;
    using group_type =
        num_collect::opt::impl::adc_group<double, ternary_vector_type>;
    using rectangle_type = typename group_type::rectangle_type;

    constexpr double dist = 0.1;
    auto group = group_type(dist);

    SECTION("construct") {
        CHECK_THAT(group.dist(), Catch::Matchers::WithinRel(dist));
        CHECK(group.empty());
    }

    SECTION("push a rectangle") {
        auto point1 = ternary_vector_type(dim);
        point1.push_back(0);
        point1.push_back(0);
        point1.push_back(1);
        constexpr double ave_value1 = 3.14;
        group.push(rectangle_type(point1, ave_value1));

        CHECK(group.min_rect().vertex() == point1);
        CHECK_FALSE(group.empty());
    }

    SECTION("push rectangles") {
        auto point1 = ternary_vector_type(dim);
        point1.push_back(0);
        point1.push_back(0);
        point1.push_back(1);
        constexpr double ave_value1 = 3.14;
        group.push(rectangle_type(point1, ave_value1));

        auto point2 = ternary_vector_type(dim);
        point2.push_back(0);
        point2.push_back(0);
        point2.push_back(0);
        constexpr double ave_value2 = 1.23;
        group.push(rectangle_type(point2, ave_value2));

        CHECK(group.min_rect().vertex() == point2);
        CHECK_FALSE(group.empty());
    }

    SECTION("pop rectangles") {
        auto point1 = ternary_vector_type(dim);
        point1.push_back(0);
        point1.push_back(0);
        point1.push_back(1);
        constexpr double ave_value1 = 3.14;
        group.push(rectangle_type(point1, ave_value1));

        auto point2 = ternary_vector_type(dim);
        point2.push_back(0);
        point2.push_back(0);
        point2.push_back(0);
        constexpr double ave_value2 = 1.23;
        group.push(rectangle_type(point2, ave_value2));

        auto popped_rect = group.pop();
        CHECK(popped_rect.vertex() == point2);
        CHECK(group.min_rect().vertex() == point1);
        CHECK_FALSE(group.empty());

        popped_rect = group.pop();
        CHECK(popped_rect.vertex() == point1);
        CHECK(group.empty());
    }

    SECTION("check whether dividable") {
        SECTION("empty group") { CHECK_FALSE(group.is_dividable()); }

        SECTION("non-full digits") {
            auto point1 = ternary_vector_type(dim);
            point1.push_back(0);
            point1.push_back(0);
            point1.push_back(0);
            constexpr double ave_value1 = 3.14;
            group.push(rectangle_type(point1, ave_value1));

            CHECK(group.is_dividable());
        }

        SECTION("full digits") {
            auto point1 = ternary_vector_type(dim);
            point1.push_back(0);
            point1.push_back(0);
            point1.push_back(0);
            point1.push_back(0);
            constexpr double ave_value1 = 3.14;
            group.push(rectangle_type(point1, ave_value1));

            CHECK_FALSE(group.is_dividable());
        }
    }
}
