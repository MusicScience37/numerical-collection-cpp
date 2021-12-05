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
 * \brief Test of explicit_gcv class.
 */
#include "num_collect/regularization/explicit_gcv.h"

#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "eigen_approx.h"
#include "num_collect/regularization/tikhonov.h"
#include "num_prob_collect/regularization/blur_sine.h"

TEST_CASE("num_collect::regularization::explicit_gcv") {
    using coeff_type = Eigen::MatrixXd;
    using data_type = Eigen::VectorXd;
    using solver_type =
        num_collect::regularization::tikhonov<coeff_type, data_type>;
    using param_searcher_type =
        num_collect::regularization::explicit_gcv<solver_type>;

    SECTION("solve") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 30;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);

        num_collect::regularization::tikhonov<coeff_type, data_type> tikhonov;
        tikhonov.compute(prob.coeff(), prob.data());

        param_searcher_type searcher{tikhonov};
        searcher.search();
        REQUIRE(std::log10(searcher.opt_param()) < 0.0);

        constexpr double tol_sol = 1e-6;

        Eigen::VectorXd solution;
        searcher.solve(solution);
        REQUIRE_THAT(solution, eigen_approx(prob.solution(), tol_sol));
    }
}
