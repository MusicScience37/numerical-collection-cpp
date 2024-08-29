/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of apply_shrinkage_operator function.
 */
#include "num_collect/regularization/impl/apply_shrinkage_operator.h"

#include <Eigen/src/Core/Matrix.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "eigen_approx.h"

TEST_CASE("num_collect::regularization::impl::apply_shrinkage_operator") {
    using num_collect::regularization::impl::apply_shrinkage_operator;

    SECTION("apply to a vector") {
        constexpr double threshold = 0.4;
        const Eigen::VectorXd target{{-0.7, -0.4, -0.3, 0.0, 0.3, 0.4, 0.7}};

        Eigen::VectorXd result = target;
        apply_shrinkage_operator(result, threshold);

        const Eigen::VectorXd expected{{-0.3, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3}};
        CHECK_THAT(result, eigen_approx(expected));
    }
}
