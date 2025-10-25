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
 * \brief Test of differentiate_polynomial_term function.
 */
#include "num_collect/rbf/impl/differentiate_polynomial_term.h"

#include <catch2/catch_test_macros.hpp>

#include "num_collect/rbf/polynomial_term.h"

TEST_CASE("num_collect::rbf::impl::differentiate_polynomial_term") {
    using num_collect::rbf::polynomial_term;
    using num_collect::rbf::impl::differentiate_polynomial_term;

    SECTION("differentiate one-dimensional polynomial terms") {
        SECTION("1st-order derivative of x^3") {
            const auto input = polynomial_term<1>(3);
            const Eigen::Vector<int, 1> orders{{1}};

            const auto result =
                differentiate_polynomial_term<double>(input, orders);

            REQUIRE(result.has_value());
            CHECK(result->first.degree() == 2);
            CHECK(result->second == 3.0);
        }

        SECTION("2nd-order derivative of x^3") {
            const auto input = polynomial_term<1>(3);
            const Eigen::Vector<int, 1> orders{{2}};

            const auto result =
                differentiate_polynomial_term<double>(input, orders);

            REQUIRE(result.has_value());
            CHECK(result->first.degree() == 1);
            CHECK(result->second == 6.0);
        }

        SECTION("3rd-order derivative of x^3") {
            const auto input = polynomial_term<1>(3);
            const Eigen::Vector<int, 1> orders{{3}};

            const auto result =
                differentiate_polynomial_term<double>(input, orders);

            REQUIRE(result.has_value());
            CHECK(result->first.degree() == 0);
            CHECK(result->second == 6.0);
        }

        SECTION("3rd-order derivative of x^2") {
            const auto input = polynomial_term<1>(2);
            const Eigen::Vector<int, 1> orders{{3}};

            const auto result =
                differentiate_polynomial_term<double>(input, orders);

            CHECK_FALSE(result.has_value());
        }

        SECTION("0th-order derivative of x^3") {
            const auto input = polynomial_term<1>(3);
            const Eigen::Vector<int, 1> orders{{0}};

            const auto result =
                differentiate_polynomial_term<double>(input, orders);

            REQUIRE(result.has_value());
            CHECK(result->first.degree() == 3);
            CHECK(result->second == 1.0);
        }
    }

    SECTION("differentiate two-dimensional polynomial terms") {
        SECTION("partially differentiate (x^2 y) by x") {
            const auto term = polynomial_term(Eigen::Vector2i{{2, 1}});
            const Eigen::Vector<int, 2> orders{{1, 0}};

            const auto result =
                differentiate_polynomial_term<double>(term, orders);

            REQUIRE(result.has_value());
            CHECK(result->first.degrees() == Eigen::Vector2i{{1, 1}});
            CHECK(result->second == 2.0);
        }

        SECTION("partially differentiate (x y^3) once by x and twice by y") {
            const auto term = polynomial_term(Eigen::Vector2i{{1, 3}});
            const Eigen::Vector<int, 2> orders{{1, 2}};

            const auto result =
                differentiate_polynomial_term<double>(term, orders);

            REQUIRE(result.has_value());
            CHECK(result->first.degrees() == Eigen::Vector2i{{0, 1}});
            CHECK(result->second == 6.0);
        }

        SECTION("partially differentiate (x^2 y^4) twice by x and twice by y") {
            const auto term = polynomial_term(Eigen::Vector2i{{2, 4}});
            const Eigen::Vector<int, 2> orders{{2, 2}};

            const auto result =
                differentiate_polynomial_term<double>(term, orders);

            REQUIRE(result.has_value());
            CHECK(result->first.degrees() == Eigen::Vector2i{{0, 2}});
            CHECK(result->second == 24.0);
        }

        SECTION("partially differentiate (x^2 y) once by x and twice by y") {
            const auto term = polynomial_term(Eigen::Vector2i{{2, 1}});
            const Eigen::Vector<int, 2> orders{{1, 2}};

            const auto result =
                differentiate_polynomial_term<double>(term, orders);

            CHECK_FALSE(result.has_value());
        }

        SECTION("no differentiation of (x^2 y)") {
            const auto term = polynomial_term(Eigen::Vector2i{{2, 1}});
            const Eigen::Vector<int, 2> orders{{0, 0}};

            const auto result =
                differentiate_polynomial_term<double>(term, orders);

            REQUIRE(result.has_value());
            CHECK(result->first.degrees() == Eigen::Vector2i{{2, 1}});
            CHECK(result->second == 1.0);
        }
    }
}
