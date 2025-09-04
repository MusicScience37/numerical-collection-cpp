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
 * \brief Test of spline_l_curve class.
 */
#include "num_collect/regularization/spline_l_curve.h"

#include <random>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/regularization/full_gen_tikhonov.h"
#include "num_collect/regularization/tikhonov.h"
#include "num_prob_collect/regularization/blur_sine.h"
#include "num_prob_collect/regularization/dense_diff_matrix.h"

TEST_CASE("num_collect::regularization::spline_l_curve") {
    using coeff_type = Eigen::MatrixXd;
    using data_type = Eigen::VectorXd;
    using solver_type =
        num_collect::regularization::tikhonov<coeff_type, data_type>;
    using param_searcher_type =
        num_collect::regularization::spline_l_curve<solver_type>;

    SECTION("solve") {
        constexpr num_collect::index_type solution_size = 60;
        constexpr num_collect::index_type data_size = 60;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);
        const coeff_type reg_mat =
            num_prob_collect::regularization::dense_diff_matrix<coeff_type>(
                solution_size);

        constexpr double error_rate = 0.01;

        std::mt19937 engine;  // NOLINT
        std::normal_distribution<double> dist{0.0,
            std::sqrt(prob.data().squaredNorm() /
                static_cast<double>(prob.data().size()) * error_rate)};
        auto data_with_error = prob.data();
        for (num_collect::index_type i = 0; i < data_with_error.size(); ++i) {
            data_with_error(i) += dist(engine);
        }

        solver_type tikhonov;
        tikhonov.compute(prob.coeff(), data_with_error);

        Eigen::VectorXd initial_solution = Eigen::VectorXd::Zero(solution_size);
        param_searcher_type searcher{
            tikhonov, data_with_error, initial_solution};
        searcher.search();
        CHECK(std::log10(searcher.opt_param()) < 0.0);

        constexpr double tol_sol = 0.5;

        Eigen::VectorXd solution;
        searcher.solve(solution);
        CHECK_THAT(solution, eigen_approx(prob.solution(), tol_sol));
    }
}
