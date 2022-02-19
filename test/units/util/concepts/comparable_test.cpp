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
 * \brief Test of comparable concept.
 */
#include "num_collect/util/concepts/comparable.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::util::concepts::less_than_comparable") {
    using num_collect::util::concepts::less_than_comparable;

    SECTION("check") {
        STATIC_REQUIRE(less_than_comparable<int, int>);
        STATIC_REQUIRE_FALSE(less_than_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator<(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(less_than_comparable<left, right>);
    }
}

TEST_CASE("num_collect::util::concepts::less_than_or_equal_to_comparable") {
    using num_collect::util::concepts::less_than_or_equal_to_comparable;

    SECTION("check") {
        STATIC_REQUIRE(less_than_or_equal_to_comparable<int, int>);
        STATIC_REQUIRE_FALSE(
            less_than_or_equal_to_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator<=(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(less_than_or_equal_to_comparable<left, right>);
    }
}

TEST_CASE("num_collect::util::concepts::greater_than_comparable") {
    using num_collect::util::concepts::greater_than_comparable;

    SECTION("check") {
        STATIC_REQUIRE(greater_than_comparable<int, int>);
        STATIC_REQUIRE_FALSE(greater_than_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator>(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(greater_than_comparable<left, right>);
    }
}

TEST_CASE("num_collect::util::concepts::greater_than_or_equal_to_comparable") {
    using num_collect::util::concepts::greater_than_or_equal_to_comparable;

    SECTION("check") {
        STATIC_REQUIRE(greater_than_or_equal_to_comparable<int, int>);
        STATIC_REQUIRE_FALSE(
            greater_than_or_equal_to_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator>=(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(greater_than_or_equal_to_comparable<left, right>);
    }
}

TEST_CASE("num_collect::util::concepts::equal_to_comparable") {
    using num_collect::util::concepts::equal_to_comparable;

    SECTION("check") {
        STATIC_REQUIRE(equal_to_comparable<int, int>);
        STATIC_REQUIRE_FALSE(equal_to_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator==(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(equal_to_comparable<left, right>);
    }
}

TEST_CASE("num_collect::util::concepts::not_equal_to_comparable") {
    using num_collect::util::concepts::not_equal_to_comparable;

    SECTION("check") {
        STATIC_REQUIRE(not_equal_to_comparable<int, int>);
        STATIC_REQUIRE_FALSE(not_equal_to_comparable<int, std::string>);

        struct right {};
        struct left {
            [[nodiscard]] auto operator!=(const right& r) const -> bool {
                return true;
            }
        };
        STATIC_REQUIRE(not_equal_to_comparable<left, right>);
    }
}

TEST_CASE("num_collect::util::concepts::comparable") {
    using num_collect::util::concepts::comparable;

    SECTION("check") {
        STATIC_REQUIRE(comparable<int, int>);
        STATIC_REQUIRE_FALSE(comparable<int, std::string>);
    }
}
