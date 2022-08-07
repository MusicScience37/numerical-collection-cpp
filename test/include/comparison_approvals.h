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

#include <limits>

#include <ApprovalTests.hpp>
#include <fmt/format.h>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/util/format_dense_vector.h"

class comparison_approvals {
public:
    template <num_collect::concepts::real_scalar Scalar>
    static void verify_with_reference(const Scalar& actual,
        const Scalar& reference,
        num_collect::index_type precision =
            (std::numeric_limits<Scalar>::digits10 / 2)) {
        ApprovalTests::Approvals::verify(
            fmt::format("Actual:    {0:.{2}e}\n"
                        "Reference: {1:.{2}e}",
                actual, reference, precision));
    }

    template <num_collect::concepts::real_scalar Scalar>
    static void verify_with_reference_and_error(const Scalar& actual,
        const Scalar& error, const Scalar& reference,
        num_collect::index_type precision =
            (std::numeric_limits<Scalar>::digits10 / 2)) {
        ApprovalTests::Approvals::verify(
            fmt::format("Actual:     {0:.{3}e}\n"
                        "Reference:  {1:.{3}e}\n"
                        "Est. Error: {2:.1e}",
                actual, reference, error, precision));
    }

    template <num_collect::concepts::real_scalar_dense_vector Vector>
    static void verify_with_reference(const Vector& actual,
        const Vector& reference,
        num_collect::index_type precision =
            (std::numeric_limits<typename Vector::Scalar>::digits10 / 2)) {
        ApprovalTests::Approvals::verify(
            fmt::format("Actual:    {0:.{2}e}\n"
                        "Reference: {1:.{2}e}",
                num_collect::util::format_dense_vector(actual),
                num_collect::util::format_dense_vector(reference), precision));
    }
};
