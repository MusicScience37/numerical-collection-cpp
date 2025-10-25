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
 * \brief Test of approx_l_curve class.
 */
#include "num_collect/regularization/approx_l_curve.h"

#include <random>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/regularization/explicit_l_curve.h"
#include "num_collect/regularization/tikhonov.h"
#include "num_prob_collect/regularization/blur_sine.h"
#include "num_prob_collect/regularization/dense_diff_matrix.h"

TEST_CASE("num_collect::regularization::approx_l_curve") {
    using coeff_type = Eigen::MatrixXd;
    using data_type = Eigen::VectorXd;
    using solver_type =
        num_collect::regularization::tikhonov<coeff_type, data_type>;
    using param_searcher_type =
        num_collect::regularization::approx_l_curve<solver_type>;
    using reference_param_searcher_type =
        num_collect::regularization::explicit_l_curve<solver_type>;

    SECTION("solve") {
        constexpr num_collect::index_type solution_size = 60;
        constexpr num_collect::index_type data_size = 60;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);
        const coeff_type reg_mat =
            num_prob_collect::regularization::dense_diff_matrix<coeff_type>(
                solution_size);

        constexpr double error_rate = 0.01;

        std::mt19937 engine;
        std::normal_distribution<double> dist{0.0,
            std::sqrt(prob.data().squaredNorm() /
                static_cast<double>(prob.data().size()) * error_rate)};
        auto data_with_error = prob.data();
        for (num_collect::index_type i = 0; i < data_with_error.size(); ++i) {
            data_with_error(i) += dist(engine);
        }

        solver_type tikhonov;
        tikhonov.compute(prob.coeff(), data_with_error);

        const Eigen::VectorXd initial_solution =
            Eigen::VectorXd::Zero(solution_size);

        reference_param_searcher_type reference_searcher{tikhonov};
        reference_searcher.search();

        SECTION("with default settings") {
            param_searcher_type searcher{tikhonov, initial_solution};
            searcher.search();

            CHECK(searcher.opt_param() > 0.1 * reference_searcher.opt_param());
            CHECK(searcher.opt_param() < 10.0 * reference_searcher.opt_param());

            constexpr double tol_sol = 0.5;

            Eigen::VectorXd solution;
            searcher.solve(solution);
            CHECK_THAT(solution, eigen_approx(prob.solution(), tol_sol));
        }

        SECTION("with too many sample points") {
            param_searcher_type searcher{tikhonov, initial_solution};
            searcher.num_sample_points(100);
            searcher.min_distance_between_points(0.3);
            REQUIRE_NOTHROW(searcher.search());

            CHECK(searcher.opt_param() > 0.1 * reference_searcher.opt_param());
            CHECK(searcher.opt_param() < 10.0 * reference_searcher.opt_param());
        }
    }

    SECTION("failure in finding positive curvature") {
        // L-curve tends to fail when the problem is too easy.
        constexpr num_collect::index_type solution_size = 10;
        constexpr num_collect::index_type data_size = solution_size;
        const coeff_type coeff = coeff_type::Identity(data_size, solution_size);
        const data_type solution = data_type::Ones(data_size);
        const data_type data = coeff * solution;

        solver_type tikhonov;
        tikhonov.compute(coeff, data);

        const Eigen::VectorXd initial_solution =
            Eigen::VectorXd::Zero(solution_size);
        param_searcher_type searcher{tikhonov, initial_solution};
        CHECK_THROWS(searcher.search());
    }

    SECTION("check invalid parameters") {
        constexpr num_collect::index_type solution_size = 10;
        solver_type tikhonov;
        const Eigen::VectorXd initial_solution =
            Eigen::VectorXd::Zero(solution_size);
        param_searcher_type searcher{tikhonov, initial_solution};

        CHECK_THROWS(searcher.num_sample_points(2));
        CHECK_NOTHROW(searcher.num_sample_points(3));

        CHECK_THROWS(searcher.min_distance_between_points(0.0));
        CHECK_THROWS(searcher.min_distance_between_points(-1.0));
        CHECK_NOTHROW(searcher.min_distance_between_points(1e-10));

        CHECK_THROWS(searcher.min_curvature_for_optimal_point(0.0));
        CHECK_THROWS(searcher.min_curvature_for_optimal_point(-1.0));
        CHECK_NOTHROW(searcher.min_curvature_for_optimal_point(1e-10));
    }
}
