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
 * \brief Test of format_dense_vector function.
 */
#include "num_collect/util/format_dense_vector.h"

#include <string>

#include <ApprovalTests.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::util::format_dense_vector") {
    using num_collect::util::format_dense_vector;

    SECTION("format dynamic vector") {
        const auto vec = Eigen::VectorXd{{1.0, -2.0, 3.0}};

        ApprovalTests::Approvals::verify(
            fmt::format("{:> 6.3f}", format_dense_vector(vec)));
    }
}
