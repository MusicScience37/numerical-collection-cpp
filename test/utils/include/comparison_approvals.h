/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of verify_with_reference function.
 */
#pragma once

#include <iterator>
#include <limits>
#include <string_view>

#include <ApprovalTests.hpp>
#include <fmt/format.h>

#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar_dense_matrix.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar_dense_vector.h"  // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "table_comparator.h"

class comparison_approvals {
public:
    template <num_collect::concepts::real_scalar Scalar>
    static void verify_with_reference(const Scalar& actual,
        const Scalar& reference,
        num_collect::index_type precision =
            (std::numeric_limits<Scalar>::digits10 / 2)) {
        constexpr num_collect::index_type whole_precision =
            std::numeric_limits<Scalar>::digits10 - 2;
        constexpr num_collect::index_type width = whole_precision + 10;

        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}{2:>{0}}{3:>{0}}\n",
            width, "Actual", "Reference", "Error");
        fmt::format_to(std::back_inserter(buffer),
            "{2:> {1}.{0}e}{3:> {1}.{0}e}{4:> {1}.{0}e}\n", whole_precision,
            width, actual, reference, actual - reference);

        const std::vector<bool> checked_columns{true, true, false};
        auto disposer =
            ApprovalTests::FileApprover::registerComparatorForExtension(".txt",
                std::make_shared<table_comparator>(
                    checked_columns, 1, precision));

        ApprovalTests::Approvals::verify(
            std::string_view(buffer.data(), buffer.size()));
    }

    template <num_collect::concepts::real_scalar Scalar>
    static void verify_with_reference_and_error(const Scalar& actual,
        const Scalar& error, const Scalar& reference,
        num_collect::index_type precision =
            (std::numeric_limits<Scalar>::digits10 / 2)) {
        constexpr num_collect::index_type whole_precision =
            std::numeric_limits<Scalar>::digits10 - 2;
        constexpr num_collect::index_type width = whole_precision + 10;

        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer),
            "{1:>{0}}{2:>{0}}{3:>{0}}{4:>{0}}\n", width, "Actual", "Reference",
            "Est. Error", "Actual Error");
        fmt::format_to(std::back_inserter(buffer),
            "{2:> {1}.{0}e}{3:> {1}.{0}e}{4:> {1}.{0}e}{5:> {1}.{0}e}\n",
            whole_precision, width, actual, reference, error,
            actual - reference);

        const std::vector<bool> checked_columns{true, true, true, false};
        auto disposer =
            ApprovalTests::FileApprover::registerComparatorForExtension(".txt",
                std::make_shared<table_comparator>(
                    checked_columns, 1, precision));

        ApprovalTests::Approvals::verify(
            std::string_view(buffer.data(), buffer.size()));
    }

    template <num_collect::concepts::real_scalar_dense_vector Vector>
    static void verify_with_reference(const Vector& actual,
        const Vector& reference,
        num_collect::index_type precision =
            (std::numeric_limits<typename Vector::Scalar>::digits10 / 2)) {
        REQUIRE(actual.size() > 0);
        REQUIRE(reference.size() == actual.size());

        constexpr num_collect::index_type whole_precision =
            std::numeric_limits<typename Vector::Scalar>::digits10 - 2;
        constexpr num_collect::index_type width = whole_precision + 10;
        const num_collect::index_type lines = actual.size();

        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Index");
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Actual");
        fmt::format_to(
            std::back_inserter(buffer), "{1:>{0}}", width, "Reference");
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Error");
        buffer.push_back('\n');

        for (num_collect::index_type l = 0; l < lines; ++l) {
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}}",
                whole_precision, width, l);
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, actual(l));
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, reference(l));
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, actual(l) - reference(l));
            buffer.push_back('\n');
        }

        const std::vector<bool> checked_columns{true, true, true, false};
        auto disposer =
            ApprovalTests::FileApprover::registerComparatorForExtension(".txt",
                std::make_shared<table_comparator>(
                    checked_columns, lines, precision));

        ApprovalTests::Approvals::verify(
            std::string_view(buffer.data(), buffer.size()));
    }

    template <num_collect::concepts::real_scalar_dense_vector Vector>
    static void verify_with_reference_and_error(const Vector& actual,
        const Vector& est_error, const Vector& reference,
        num_collect::index_type precision =
            (std::numeric_limits<typename Vector::Scalar>::digits10 / 2)) {
        REQUIRE(actual.size() > 0);
        REQUIRE(reference.size() == actual.size());

        constexpr num_collect::index_type whole_precision =
            std::numeric_limits<typename Vector::Scalar>::digits10 - 2;
        constexpr num_collect::index_type width = whole_precision + 10;
        const num_collect::index_type lines = actual.size();

        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Index");
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Actual");
        fmt::format_to(
            std::back_inserter(buffer), "{1:>{0}}", width, "Reference");
        fmt::format_to(
            std::back_inserter(buffer), "{1:>{0}}", width, "Est. Error");
        fmt::format_to(
            std::back_inserter(buffer), "{1:>{0}}", width, "Actual Error");
        buffer.push_back('\n');

        for (num_collect::index_type l = 0; l < lines; ++l) {
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}}",
                whole_precision, width, l);
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, actual(l));
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, reference(l));
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, est_error(l));
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                whole_precision, width, actual(l) - reference(l));
            buffer.push_back('\n');
        }

        const std::vector<bool> checked_columns{true, true, true, true, false};
        auto disposer =
            ApprovalTests::FileApprover::registerComparatorForExtension(".txt",
                std::make_shared<table_comparator>(
                    checked_columns, lines, precision));

        ApprovalTests::Approvals::verify(
            std::string_view(buffer.data(), buffer.size()));
    }

    template <num_collect::concepts::real_scalar_dense_matrix Matrix>
    static void verify_with_reference(const Matrix& actual,
        const Matrix& reference,
        num_collect::index_type precision =
            (std::numeric_limits<typename Matrix::Scalar>::digits10 / 2)) {
        REQUIRE(actual.rows() > 0);
        REQUIRE(reference.rows() == actual.rows());
        REQUIRE(actual.cols() > 0);
        REQUIRE(reference.cols() == actual.cols());

        constexpr num_collect::index_type whole_precision =
            std::numeric_limits<typename Matrix::Scalar>::digits10 - 2;
        constexpr num_collect::index_type width = whole_precision + 10;
        const num_collect::index_type lines = actual.size();

        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Row");
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Column");
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Actual");
        fmt::format_to(
            std::back_inserter(buffer), "{1:>{0}}", width, "Reference");
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Error");
        buffer.push_back('\n');

        for (num_collect::index_type r = 0; r < actual.rows(); ++r) {
            for (num_collect::index_type c = 0; c < actual.cols(); ++c) {
                fmt::format_to(std::back_inserter(buffer), "{2:> {1}}",
                    whole_precision, width, r);
                fmt::format_to(std::back_inserter(buffer), "{2:> {1}}",
                    whole_precision, width, c);
                fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                    whole_precision, width, actual(r, c));
                fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                    whole_precision, width, reference(r, c));
                fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                    whole_precision, width, actual(r, c) - reference(r, c));
                buffer.push_back('\n');
            }
        }

        const std::vector<bool> checked_columns{true, true, true, true, false};
        auto disposer =
            ApprovalTests::FileApprover::registerComparatorForExtension(".txt",
                std::make_shared<table_comparator>(
                    checked_columns, lines, precision));

        ApprovalTests::Approvals::verify(
            std::string_view(buffer.data(), buffer.size()));
    }
};
