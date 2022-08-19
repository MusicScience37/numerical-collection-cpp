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
 * \brief Test of functions objects to compare values.
 */
#include "num_collect/util/comparators.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/util/concepts/comparator.h"

TEST_CASE("num_collect::util::less") {
    using num_collect::util::less;
    using num_collect::util::concepts::comparator;

    SECTION("compare") {
        struct right {};
        struct left {
            [[nodiscard]] auto operator<(const right& /*r*/) const -> bool {
                return true;
            }
        };

        STATIC_REQUIRE(comparator<less<left, right>, left, right>);
        REQUIRE(less<left, right>()(left(), right()));
    }
}

TEST_CASE("num_collect::util::less_equal") {
    using num_collect::util::less_equal;
    using num_collect::util::concepts::comparator;

    SECTION("compare") {
        struct right {};
        struct left {
            [[nodiscard]] auto operator<=(const right& /*r*/) const -> bool {
                return true;
            }
        };

        STATIC_REQUIRE(comparator<less_equal<left, right>, left, right>);
        REQUIRE(less_equal<left, right>()(left(), right()));
    }
}

TEST_CASE("num_collect::util::greater") {
    using num_collect::util::greater;
    using num_collect::util::concepts::comparator;

    SECTION("compare") {
        struct right {};
        struct left {
            [[nodiscard]] auto operator>(const right& /*r*/) const -> bool {
                return true;
            }
        };

        STATIC_REQUIRE(comparator<greater<left, right>, left, right>);
        REQUIRE(greater<left, right>()(left(), right()));
    }
}

TEST_CASE("num_collect::util::greater_equal") {
    using num_collect::util::greater_equal;
    using num_collect::util::concepts::comparator;

    SECTION("compare") {
        struct right {};
        struct left {
            [[nodiscard]] auto operator>=(const right& /*r*/) const -> bool {
                return true;
            }
        };

        STATIC_REQUIRE(comparator<greater_equal<left, right>, left, right>);
        REQUIRE(greater_equal<left, right>()(left(), right()));
    }
}

TEST_CASE("num_collect::util::equal") {
    using num_collect::util::equal;
    using num_collect::util::concepts::comparator;

    SECTION("compare") {
        struct right {};
        struct left {
            [[nodiscard]] auto operator==(const right& /*r*/) const -> bool {
                return true;
            }
        };

        STATIC_REQUIRE(comparator<equal<left, right>, left, right>);
        REQUIRE(equal<left, right>()(left(), right()));
    }
}

TEST_CASE("num_collect::util::not_equal") {
    using num_collect::util::not_equal;
    using num_collect::util::concepts::comparator;

    SECTION("compare") {
        struct right {};
        struct left {
            [[nodiscard]] auto operator!=(const right& /*r*/) const -> bool {
                return true;
            }
        };

        STATIC_REQUIRE(comparator<not_equal<left, right>, left, right>);
        REQUIRE(not_equal<left, right>()(left(), right()));
    }
}
