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
 * \brief Definition of ode_approvals class.
 */
#pragma once

#include <iterator>
#include <limits>
#include <string_view>
#include <vector>

#include <ApprovalTests.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "num_collect/base/concepts/dense_vector_of.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "table_comparator.h"

class ode_approvals {
public:
    template <typename Solver, num_collect::concepts::real_scalar Scalar>
    static void verify_with_reference(const std::vector<Scalar>& time,
        const std::vector<Scalar>& actual, const std::vector<Scalar>& reference,
        std::string_view problem_condition,
        num_collect::index_type precision =
            (std::numeric_limits<Scalar>::digits10 / 2)) {
        REQUIRE(actual.size() > 0);
        REQUIRE(reference.size() == actual.size());
        REQUIRE(reference.size() == time.size());

        constexpr num_collect::index_type whole_precision =
            std::numeric_limits<Scalar>::digits10 - 2;
        constexpr num_collect::index_type width = whole_precision + 10;
        const std::size_t lines = actual.size();

        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Time");
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Actual");
        fmt::format_to(
            std::back_inserter(buffer), "{1:>{0}}", width, "Reference");
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Error");
        buffer.push_back('\n');

        for (std::size_t l = 0; l < lines; ++l) {
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, time.at(l));
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, actual.at(l));
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, reference.at(l));
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, actual.at(l) - reference.at(l));
            buffer.push_back('\n');
        }

        const std::vector<bool> checked_columns{true, true, true, false};
        auto disposer =
            ApprovalTests::FileApprover::registerComparatorForExtension(".txt",
                std::make_shared<table_comparator>(
                    checked_columns, lines, precision));

        ApprovalTests::Approvals::verify(
            std::string_view(buffer.data(), buffer.size()),
            ApprovalTests::Options().withNamer(
                ApprovalTests::TemplatedCustomNamer::create(
                    generate_approval_file_path_template<Solver>(
                        problem_condition))));
    }

    template <typename Solver, num_collect::concepts::real_scalar Scalar,
        num_collect::concepts::dense_vector_of<Scalar> Vector>
    static void verify_with_reference(const std::vector<Scalar>& time,
        const std::vector<Vector>& actual, const std::vector<Vector>& reference,
        std::string_view problem_condition,
        num_collect::index_type precision =
            (std::numeric_limits<Scalar>::digits10 / 2)) {
        REQUIRE(actual.size() > 0);
        REQUIRE(reference.size() == actual.size());
        REQUIRE(reference.size() == time.size());

        constexpr num_collect::index_type whole_precision =
            std::numeric_limits<Scalar>::digits10 - 2;
        const num_collect::index_type width = whole_precision + 10;
        const num_collect::index_type dimension = actual.at(0).size();
        const std::size_t lines = actual.size();

        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Time");
        for (num_collect::index_type i = 0; i < dimension; ++i) {
            fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width,
                fmt::format("Actual{}", i));
            fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width,
                fmt::format("Reference{}", i));
            fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width,
                fmt::format("Error{}", i));
        }
        buffer.push_back('\n');

        for (std::size_t l = 0; l < lines; ++l) {
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, time.at(l));
            for (num_collect::index_type i = 0; i < dimension; ++i) {
                fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                    whole_precision, width, actual.at(l)(i));
                fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                    whole_precision, width, reference.at(l)(i));
                fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                    whole_precision, width,
                    actual.at(l)(i) - reference.at(l)(i));
            }
            buffer.push_back('\n');
        }

        std::vector<bool> checked_columns;
        checked_columns.push_back(true);  // Time
        for (num_collect::index_type i = 0; i < dimension; ++i) {
            checked_columns.push_back(true);   // Actual
            checked_columns.push_back(true);   // Reference
            checked_columns.push_back(false);  // Error
        }
        auto disposer =
            ApprovalTests::FileApprover::registerComparatorForExtension(".txt",
                std::make_shared<table_comparator>(
                    checked_columns, lines, precision));

        ApprovalTests::Approvals::verify(
            std::string_view(buffer.data(), buffer.size()),
            ApprovalTests::Options().withNamer(
                ApprovalTests::TemplatedCustomNamer::create(
                    generate_approval_file_path_template<Solver>(
                        problem_condition))));
    }

private:
    template <typename Solver>
    [[nodiscard]] static auto generate_approval_file_path_template(
        std::string_view problem_condition) -> std::string {
        fmt::memory_buffer buffer;
        buffer.append(
            std::string_view("{TestSourceDirectory}/{ApprovalsSubdirectory}/"));
        if (!problem_condition.empty()) {
            fmt::format_to(
                std::back_inserter(buffer), "{}_", problem_condition);
        }
        const std::string_view formula_name =
            Solver::formula_type::log_tag.name();
        if (auto pos = formula_name.find_last_of(':');
            pos != std::string_view::npos) {
            buffer.append(formula_name.substr(pos + 1U));
        } else {
            buffer.append(formula_name);
        }
        buffer.append(
            std::string_view(".{ApprovedOrReceived}.{FileExtension}"));
        return std::string(buffer.data(), buffer.size());
    }
};
