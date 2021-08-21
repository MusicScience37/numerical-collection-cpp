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
 * \brief Test of tikhonov class.
 */
#include "num_collect/regularization/tikhonov.h"

#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "eigen_approx.h"
#include "num_prob_collect/regularization/blur_sine.h"

TEST_CASE("num_collect::regularization::tikhonov") {
    using coeff_type = Eigen::MatrixXd;
    using data_type = Eigen::VectorXd;

    SECTION("solve") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 30;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);

        num_collect::regularization::tikhonov<coeff_type, data_type> tikhonov;
        tikhonov.compute(prob.coeff(), prob.data());
        Eigen::VectorXd solution;
        tikhonov.solve(0.0, solution);

        REQUIRE_THAT(solution, eigen_approx(prob.solution()));
    }

    SECTION("solve with different parameters") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 30;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);

        num_collect::regularization::tikhonov<coeff_type, data_type> tikhonov;
        tikhonov.compute(prob.coeff(), prob.data());

        constexpr double param_small = 1e-2;
        Eigen::VectorXd solution_small;
        tikhonov.solve(param_small, solution_small);

        constexpr double param_large = 1e+2;
        Eigen::VectorXd solution_large;
        tikhonov.solve(param_large, solution_large);

        REQUIRE(solution_large.squaredNorm() < solution_small.squaredNorm());
    }
}
