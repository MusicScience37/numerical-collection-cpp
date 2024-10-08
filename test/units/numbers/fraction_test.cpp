/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of fraction class.
 */
#include "num_collect/numbers/fraction.h"

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <type_traits>  // IWYU pragma: keep

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

TEMPLATE_TEST_CASE(
    "num_collect::numbers::fraction (signed)", "", std::int32_t, std::int64_t) {
    using integer_type = TestType;
    using fraction_type = num_collect::numbers::fraction<integer_type>;

    SECTION("exception safety") {
        STATIC_REQUIRE(std::is_nothrow_default_constructible_v<fraction_type>);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<fraction_type>);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<fraction_type>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<fraction_type>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<fraction_type>);
    }

    SECTION("default constructor") {
        const auto frac = fraction_type();

        CHECK(frac.numerator() == 0);
        CHECK(frac.denominator() == 1);
    }

    SECTION("construct from an integer") {
        const auto integer = static_cast<integer_type>(-2);

        fraction_type frac = integer;

        CHECK(frac.numerator() == integer);
        CHECK(frac.denominator() == 1);
    }

    SECTION("construct from two integers") {
        const auto numerator = static_cast<integer_type>(-2);
        const auto denominator = static_cast<integer_type>(5);

        const auto frac = fraction_type(numerator, denominator);

        CHECK(frac.numerator() == numerator);
        CHECK(frac.denominator() == denominator);
    }

    SECTION("normalize in constructor") {
        SECTION("ordinary case") {
            const auto numerator = static_cast<integer_type>(-4);
            const auto denominator = static_cast<integer_type>(-6);

            const auto frac = fraction_type(numerator, denominator);

            CHECK(frac.numerator() == 2);
            CHECK(frac.denominator() == 3);
        }

        SECTION("when an integer is positive and another is negative") {
            const auto numerator = static_cast<integer_type>(4);
            const auto denominator = static_cast<integer_type>(-6);

            const auto frac = fraction_type(numerator, denominator);

            CHECK(frac.numerator() == -2);
            CHECK(frac.denominator() == 3);
        }

        SECTION("when the numerator is zero") {
            const auto numerator = static_cast<integer_type>(0);
            const auto denominator = static_cast<integer_type>(-6);

            const auto frac = fraction_type(numerator, denominator);

            CHECK(frac.numerator() == 0);
            CHECK(frac.denominator() == 1);
        }
    }

    SECTION("check denominator in constructor") {
        CHECK_NOTHROW((void)fraction_type(1, 1));
        CHECK_NOTHROW((void)fraction_type(1, -1));
        CHECK_THROWS((void)fraction_type(1, 0));
    }

    SECTION("add with operator+=") {
        SECTION("when denominator is same") {
            const auto left = fraction_type(2, 5);
            const auto right = fraction_type(4, 5);

            const auto res = fraction_type(left) += right;

            CHECK(res.numerator() == 6);
            CHECK(res.denominator() == 5);
        }

        SECTION("when denominator is different") {
            const auto left = fraction_type(1, 2);
            const auto right = fraction_type(1, 3);

            const auto res = fraction_type(left) += right;

            CHECK(res.numerator() == 5);
            CHECK(res.denominator() == 6);
        }

        SECTION("when normalization after addition is needed") {
            const auto left = fraction_type(1, 3);
            const auto right = fraction_type(1, 6);

            const auto res = fraction_type(left) += right;

            CHECK(res.numerator() == 1);
            CHECK(res.denominator() == 2);
        }
    }

    SECTION("add with operator+") {
        SECTION("when denominator is same") {
            const auto left = fraction_type(2, 5);
            const auto right = fraction_type(4, 5);

            const auto res = left + right;

            CHECK(res.numerator() == 6);
            CHECK(res.denominator() == 5);
        }

        SECTION("when denominator is different") {
            const auto left = fraction_type(1, 2);
            const auto right = fraction_type(1, 3);

            const auto res = left + right;

            CHECK(res.numerator() == 5);
            CHECK(res.denominator() == 6);
        }

        SECTION("when normalization after addition is needed") {
            const auto left = fraction_type(1, 3);
            const auto right = fraction_type(1, 6);

            const auto res = left + right;

            CHECK(res.numerator() == 1);
            CHECK(res.denominator() == 2);
        }
    }

    SECTION("subtract with operator-=") {
        SECTION("when denominator is same") {
            const auto left = fraction_type(3, 5);
            const auto right = fraction_type(2, 5);

            const auto res = fraction_type(left) -= right;

            CHECK(res.numerator() == 1);
            CHECK(res.denominator() == 5);
        }

        SECTION("when denominator is different") {
            const auto left = fraction_type(4, 3);
            const auto right = fraction_type(1, 2);

            const auto res = fraction_type(left) -= right;

            CHECK(res.numerator() == 5);
            CHECK(res.denominator() == 6);
        }

        SECTION("when normalization after subtraction is needed") {
            const auto left = fraction_type(5, 6);
            const auto right = fraction_type(1, 2);

            const auto res = fraction_type(left) -= right;

            CHECK(res.numerator() == 1);
            CHECK(res.denominator() == 3);
        }
    }

    SECTION("subtract with operator-") {
        SECTION("when denominator is same") {
            const auto left = fraction_type(3, 5);
            const auto right = fraction_type(2, 5);

            const auto res = left - right;

            CHECK(res.numerator() == 1);
            CHECK(res.denominator() == 5);
        }

        SECTION("when denominator is different") {
            const auto left = fraction_type(4, 3);
            const auto right = fraction_type(1, 2);

            const auto res = left - right;

            CHECK(res.numerator() == 5);
            CHECK(res.denominator() == 6);
        }

        SECTION("when normalization after subtraction is needed") {
            const auto left = fraction_type(5, 6);
            const auto right = fraction_type(1, 2);

            const auto res = left - right;

            CHECK(res.numerator() == 1);
            CHECK(res.denominator() == 3);
        }
    }

    SECTION("multiply with operator*=") {
        const auto left = fraction_type(2, 5);
        const auto right = fraction_type(3, 4);

        const auto res = fraction_type(left) *= right;

        CHECK(res.numerator() == 3);
        CHECK(res.denominator() == 10);
    }

    SECTION("multiply with operator*") {
        const auto left = fraction_type(2, 5);
        const auto right = fraction_type(3, 4);

        const auto res = left * right;

        CHECK(res.numerator() == 3);
        CHECK(res.denominator() == 10);
    }

    SECTION("multiply with operator/=") {
        const auto left = fraction_type(6, 7);
        const auto right = fraction_type(3, 5);

        const auto res = fraction_type(left) /= right;

        CHECK(res.numerator() == 10);
        CHECK(res.denominator() == 7);
    }

    SECTION("multiply with operator/") {
        const auto left = fraction_type(6, 7);
        const auto right = fraction_type(3, 5);

        const auto res = left / right;

        CHECK(res.numerator() == 10);
        CHECK(res.denominator() == 7);
    }

    SECTION("compare with operator==") {
        const auto frac1 = fraction_type(3, 5);
        const auto frac2 = fraction_type(6, 10);
        const auto frac3 = fraction_type(4, 5);
        const auto frac4 = fraction_type(3, 4);

        CHECK(frac1 == frac1);
        CHECK(frac1 == frac2);
        CHECK_FALSE(frac1 == frac3);
        CHECK_FALSE(frac1 == frac4);
    }

    SECTION("compare with operator!=") {
        const auto frac1 = fraction_type(3, 5);
        const auto frac2 = fraction_type(6, 10);
        const auto frac3 = fraction_type(4, 5);
        const auto frac4 = fraction_type(3, 4);

        CHECK_FALSE(frac1 != frac1);
        CHECK_FALSE(frac1 != frac2);
        CHECK(frac1 != frac3);
        CHECK(frac1 != frac4);
    }

    SECTION("format using fmt library") {
        const auto frac = fraction_type(-2, 3);

        const auto str = fmt::format("{}", frac);

        CHECK(str == "-2 / 3");
    }

    SECTION("format using std::ostream") {
        const auto frac = fraction_type(-2, 3);

        std::ostringstream stream;
        stream << frac;

        CHECK(stream.str() == "-2 / 3");
    }
}

TEMPLATE_TEST_CASE("num_collect::numbers::fraction (unsigned)", "",
    std::uint32_t, std::uint64_t) {
    using integer_type = TestType;
    using fraction_type = num_collect::numbers::fraction<integer_type>;

    SECTION("exception safety") {
        STATIC_REQUIRE(std::is_nothrow_default_constructible_v<fraction_type>);
        STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<fraction_type>);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<fraction_type>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<fraction_type>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<fraction_type>);
    }

    SECTION("default constructor") {
        const auto frac = fraction_type();

        CHECK(frac.numerator() == 0U);
        CHECK(frac.denominator() == 1U);
    }

    SECTION("construct from an integer") {
        const auto integer = static_cast<integer_type>(3);

        fraction_type frac = integer;

        CHECK(frac.numerator() == integer);
        CHECK(frac.denominator() == 1U);
    }

    SECTION("construct from two integers") {
        const auto numerator = static_cast<integer_type>(2);
        const auto denominator = static_cast<integer_type>(5);

        const auto frac = fraction_type(numerator, denominator);

        CHECK(frac.numerator() == numerator);
        CHECK(frac.denominator() == denominator);
    }

    SECTION("normalize in constructor") {
        SECTION("ordinary case") {
            const auto numerator = static_cast<integer_type>(4);
            const auto denominator = static_cast<integer_type>(6);

            const auto frac = fraction_type(numerator, denominator);

            CHECK(frac.numerator() == 2U);
            CHECK(frac.denominator() == 3U);
        }

        SECTION("when the numerator is zero") {
            const auto numerator = static_cast<integer_type>(0);
            const auto denominator = static_cast<integer_type>(6);

            const auto frac = fraction_type(numerator, denominator);

            CHECK(frac.numerator() == 0U);
            CHECK(frac.denominator() == 1U);
        }
    }

    SECTION("check denominator in constructor") {
        CHECK_NOTHROW((void)fraction_type(1, 1));
        CHECK_THROWS((void)fraction_type(1, 0));
    }

    SECTION("add with operator+=") {
        SECTION("when denominator is same") {
            const auto left = fraction_type(2U, 5U);
            const auto right = fraction_type(4U, 5U);

            const auto res = fraction_type(left) += right;

            CHECK(res.numerator() == 6U);
            CHECK(res.denominator() == 5U);
        }

        SECTION("when denominator is different") {
            const auto left = fraction_type(1U, 2U);
            const auto right = fraction_type(1U, 3U);

            const auto res = fraction_type(left) += right;

            CHECK(res.numerator() == 5U);
            CHECK(res.denominator() == 6U);
        }

        SECTION("when normalization after addition is needed") {
            const auto left = fraction_type(1U, 3U);
            const auto right = fraction_type(1U, 6U);

            const auto res = fraction_type(left) += right;

            CHECK(res.numerator() == 1U);
            CHECK(res.denominator() == 2U);
        }
    }

    SECTION("add with operator+") {
        SECTION("when denominator is same") {
            const auto left = fraction_type(2U, 5U);
            const auto right = fraction_type(4U, 5U);

            const auto res = left + right;

            CHECK(res.numerator() == 6U);
            CHECK(res.denominator() == 5U);
        }

        SECTION("when denominator is different") {
            const auto left = fraction_type(1U, 2U);
            const auto right = fraction_type(1U, 3U);

            const auto res = left + right;

            CHECK(res.numerator() == 5U);
            CHECK(res.denominator() == 6U);
        }

        SECTION("when normalization after addition is needed") {
            const auto left = fraction_type(1U, 3U);
            const auto right = fraction_type(1U, 6U);

            const auto res = left + right;

            CHECK(res.numerator() == 1U);
            CHECK(res.denominator() == 2U);
        }
    }

    SECTION("subtract with operator-=") {
        SECTION("when denominator is same") {
            const auto left = fraction_type(3U, 5U);
            const auto right = fraction_type(2U, 5U);

            const auto res = fraction_type(left) -= right;

            CHECK(res.numerator() == 1U);
            CHECK(res.denominator() == 5U);
        }

        SECTION("when denominator is different") {
            const auto left = fraction_type(4U, 3U);
            const auto right = fraction_type(1U, 2U);

            const auto res = fraction_type(left) -= right;

            CHECK(res.numerator() == 5U);
            CHECK(res.denominator() == 6U);
        }

        SECTION("when normalization after subtraction is needed") {
            const auto left = fraction_type(5U, 6U);
            const auto right = fraction_type(1U, 2U);

            const auto res = fraction_type(left) -= right;

            CHECK(res.numerator() == 1U);
            CHECK(res.denominator() == 3U);
        }
    }

    SECTION("subtract with operator-") {
        SECTION("when denominator is same") {
            const auto left = fraction_type(3U, 5U);
            const auto right = fraction_type(2U, 5U);

            const auto res = left - right;

            CHECK(res.numerator() == 1U);
            CHECK(res.denominator() == 5U);
        }

        SECTION("when denominator is different") {
            const auto left = fraction_type(4U, 3U);
            const auto right = fraction_type(1U, 2U);

            const auto res = left - right;

            CHECK(res.numerator() == 5U);
            CHECK(res.denominator() == 6U);
        }

        SECTION("when normalization after subtraction is needed") {
            const auto left = fraction_type(5U, 6U);
            const auto right = fraction_type(1U, 2U);

            const auto res = left - right;

            CHECK(res.numerator() == 1U);
            CHECK(res.denominator() == 3U);
        }
    }

    SECTION("multiply with operator*=") {
        const auto left = fraction_type(2U, 5U);
        const auto right = fraction_type(3U, 4U);

        const auto res = fraction_type(left) *= right;

        CHECK(res.numerator() == 3U);
        CHECK(res.denominator() == 10U);
    }

    SECTION("multiply with operator*") {
        const auto left = fraction_type(2U, 5U);
        const auto right = fraction_type(3U, 4U);

        const auto res = left * right;

        CHECK(res.numerator() == 3U);
        CHECK(res.denominator() == 10U);
    }

    SECTION("multiply with operator/=") {
        const auto left = fraction_type(6U, 7U);
        const auto right = fraction_type(3U, 5U);

        const auto res = fraction_type(left) /= right;

        CHECK(res.numerator() == 10U);
        CHECK(res.denominator() == 7U);
    }

    SECTION("multiply with operator/") {
        const auto left = fraction_type(6U, 7U);
        const auto right = fraction_type(3U, 5U);

        const auto res = left / right;

        CHECK(res.numerator() == 10U);
        CHECK(res.denominator() == 7U);
    }

    SECTION("compare with operator==") {
        const auto frac1 = fraction_type(3U, 5U);
        const auto frac2 = fraction_type(6U, 10U);
        const auto frac3 = fraction_type(4U, 5U);
        const auto frac4 = fraction_type(3U, 4U);

        CHECK(frac1 == frac1);
        CHECK(frac1 == frac2);
        CHECK_FALSE(frac1 == frac3);
        CHECK_FALSE(frac1 == frac4);
    }

    SECTION("compare with operator!=") {
        const auto frac1 = fraction_type(3U, 5U);
        const auto frac2 = fraction_type(6U, 10U);
        const auto frac3 = fraction_type(4U, 5U);
        const auto frac4 = fraction_type(3U, 4U);

        CHECK_FALSE(frac1 != frac1);
        CHECK_FALSE(frac1 != frac2);
        CHECK(frac1 != frac3);
        CHECK(frac1 != frac4);
    }

    SECTION("format using fmt library") {
        const auto frac = fraction_type(2U, 3U);

        const auto str = fmt::format("{}", frac);

        CHECK(str == "2 / 3");
    }

    SECTION("format using std::ostream") {
        const auto frac = fraction_type(2U, 3U);

        std::ostringstream stream;
        stream << frac;

        CHECK(stream.str() == "2 / 3");
    }
}
