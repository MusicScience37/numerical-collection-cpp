/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of range_adaptor_closure class.
 */
#include "num_collect/util/impl/range_adaptor_closure.h"

#include <list>
#include <type_traits>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

// Closure for testing light_range_adaptor_closure.
// This closure works like `std::ranges::to`.
class test_light_range_adaptor_closure
    : public num_collect::util::impl::light_range_adaptor_closure<
          test_light_range_adaptor_closure> {
public:
    template <std::ranges::viewable_range Range>
    auto operator()(Range&& range) const {
        std::vector<std::ranges::range_value_t<Range>> result;
        for (const auto& value : range) {
            // NOLINTNEXTLINE
            result.push_back(value);
        }
        return result;
    }
};

TEST_CASE("num_collect::util::impl::light_range_adaptor_closure") {
    SECTION("use a closure") {
        const std::list<int> input{1, 2, 3};

        const auto result = input | test_light_range_adaptor_closure();

        STATIC_REQUIRE(
            std::same_as<std::decay_t<decltype(result)>, std::vector<int>>);
        CHECK_THAT(result, Catch::Matchers::RangeEquals({1, 2, 3}));
    }
}

// Closure for testing range_adaptor_closure.
// This closure works like `std::ranges::to`.
class test_range_adaptor_closure
    : public num_collect::util::impl::range_adaptor_closure<
          test_range_adaptor_closure> {
public:
    template <std::ranges::viewable_range Range>
    auto operator()(Range&& range) const {
        std::vector<std::ranges::range_value_t<Range>> result;
        for (const auto& value : range) {
            // NOLINTNEXTLINE
            result.push_back(value);
        }
        return result;
    }
};

TEST_CASE("num_collect::util::impl::range_adaptor_closure") {
    SECTION("use a closure") {
        const std::list<int> input{1, 2, 3};

        const auto result = input | test_range_adaptor_closure();

        STATIC_REQUIRE(
            std::same_as<std::decay_t<decltype(result)>, std::vector<int>>);
        CHECK_THAT(result, Catch::Matchers::RangeEquals({1, 2, 3}));
    }
}
