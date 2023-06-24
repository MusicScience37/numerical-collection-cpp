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

class ode_approvals {
public:
    template <num_collect::concepts::real_scalar Scalar>
    static void verify_with_reference(const std::vector<Scalar>& time,
        const std::vector<Scalar>& actual, const std::vector<Scalar>& reference,
        num_collect::index_type precision =
            (std::numeric_limits<Scalar>::digits10 / 2)) {
        REQUIRE(actual.size() > 0);
        REQUIRE(reference.size() == actual.size());
        REQUIRE(reference.size() == time.size());

        const num_collect::index_type width = precision + 10;
        const num_collect::index_type lines = actual.size();

        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Time");
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Actual");
        fmt::format_to(
            std::back_inserter(buffer), "{1:>{0}}", width, "Reference");
        buffer.push_back('\n');

        for (num_collect::index_type l = 0; l < lines; ++l) {
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                precision, width, time.at(l));
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                precision, width, actual.at(l));
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                precision, width, reference.at(l));
            buffer.push_back('\n');
        }

        ApprovalTests::Approvals::verify(
            std::string_view(buffer.data(), buffer.size()));
    }

    template <num_collect::concepts::real_scalar Scalar,
        num_collect::concepts::dense_vector_of<Scalar> Vector>
    static void verify_with_reference(const std::vector<Scalar>& time,
        const std::vector<Vector>& actual, const std::vector<Vector>& reference,
        num_collect::index_type precision =
            (std::numeric_limits<Scalar>::digits10 / 2)) {
        REQUIRE(actual.size() > 0);
        REQUIRE(reference.size() == actual.size());
        REQUIRE(reference.size() == time.size());

        const num_collect::index_type width = precision + 5;
        const num_collect::index_type dimension = actual.at(0).size();
        const num_collect::index_type lines = actual.size();

        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width, "Time");
        for (num_collect::index_type i = 0; i < dimension; ++i) {
            fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width,
                fmt::format("Actual{}", i));
            fmt::format_to(std::back_inserter(buffer), "{1:>{0}}", width,
                fmt::format("Reference{}", i));
        }
        buffer.push_back('\n');

        for (num_collect::index_type l = 0; l < lines; ++l) {
            fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                precision, width, time.at(l));
            for (num_collect::index_type i = 0; i < dimension; ++i) {
                fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                    precision, width, actual.at(l)(i));
                fmt::format_to(std::back_inserter(buffer), "{2:> {1}.{0}e}",
                    precision, width, reference.at(l)(i));
            }
            buffer.push_back('\n');
        }

        ApprovalTests::Approvals::verify(
            std::string_view(buffer.data(), buffer.size()));
    }
};
