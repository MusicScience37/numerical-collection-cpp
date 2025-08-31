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
 * \brief Test of polynomial_term_generator class.
 */
#include "num_collect/rbf/polynomial_term_generator.h"

#include <iterator>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include "fmt_approval_tests.h"

TEST_CASE("num_collect::rbf::polynomial_term_generator") {
    using num_collect::rbf::polynomial_term_generator;

    const auto format_terms = [](const auto& terms) -> std::string {
        fmt::memory_buffer buffer;
        for (const auto& term : terms) {
            fmt::format_to(std::back_inserter(buffer), "{}\n",
                fmt::join(term.degrees(), " "));
        }
        return std::string(buffer.data(), buffer.size());
    };

    SECTION("NumDimensions = 1") {
        constexpr int num_dimensions = 1;

        SECTION("max_degree = 0") {
            constexpr int max_degree = 0;
            polynomial_term_generator<num_dimensions> generator(max_degree);

            ApprovalTests::Approvals::verify(format_terms(generator.terms()));
        }

        SECTION("max_degree = 1") {
            constexpr int max_degree = 1;
            polynomial_term_generator<num_dimensions> generator(max_degree);

            ApprovalTests::Approvals::verify(format_terms(generator.terms()));
        }

        SECTION("max_degree = 2") {
            constexpr int max_degree = 2;
            polynomial_term_generator<num_dimensions> generator(max_degree);

            ApprovalTests::Approvals::verify(format_terms(generator.terms()));
        }
    }

    SECTION("NumDimensions = 2") {
        constexpr int num_dimensions = 2;

        SECTION("max_degree = 0") {
            constexpr int max_degree = 0;
            polynomial_term_generator<num_dimensions> generator(max_degree);

            ApprovalTests::Approvals::verify(format_terms(generator.terms()));
        }

        SECTION("max_degree = 1") {
            constexpr int max_degree = 1;
            polynomial_term_generator<num_dimensions> generator(max_degree);

            ApprovalTests::Approvals::verify(format_terms(generator.terms()));
        }

        SECTION("max_degree = 2") {
            constexpr int max_degree = 2;
            polynomial_term_generator<num_dimensions> generator(max_degree);

            ApprovalTests::Approvals::verify(format_terms(generator.terms()));
        }
    }

    SECTION("NumDimensions = 3") {
        constexpr int num_dimensions = 3;

        SECTION("max_degree = 0") {
            constexpr int max_degree = 0;
            polynomial_term_generator<num_dimensions> generator(max_degree);

            ApprovalTests::Approvals::verify(format_terms(generator.terms()));
        }

        SECTION("max_degree = 1") {
            constexpr int max_degree = 1;
            polynomial_term_generator<num_dimensions> generator(max_degree);

            ApprovalTests::Approvals::verify(format_terms(generator.terms()));
        }

        SECTION("max_degree = 2") {
            constexpr int max_degree = 2;
            polynomial_term_generator<num_dimensions> generator(max_degree);

            ApprovalTests::Approvals::verify(format_terms(generator.terms()));
        }
    }

    SECTION("throws when max_degree is negative") {
        constexpr int num_dimensions = 1;
        constexpr int max_degree = -1;
        CHECK_THROWS(
            (void)polynomial_term_generator<num_dimensions>(max_degree));
    }
}
