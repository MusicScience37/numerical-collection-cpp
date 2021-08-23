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
 * \brief Test of full_gen_tikhonov class.
 */
#include "num_collect/regularization/full_gen_tikhonov.h"

#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "eigen_approx.h"
#include "num_prob_collect/regularization/blur_sine.h"
#include "num_prob_collect/regularization/dense_diff_matrix.h"

TEST_CASE("num_collect::regularization::full_gen_tikhonov") {
    using coeff_type = Eigen::MatrixXd;
    using data_type = Eigen::VectorXd;

    SECTION("solve") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 30;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);
        const coeff_type reg_mat =
            num_prob_collect::regularization::dense_diff_matrix<coeff_type>(
                solution_size);

        num_collect::regularization::full_gen_tikhonov<coeff_type, data_type>
            full_gen_tikhonov;
        full_gen_tikhonov.compute(prob.coeff(), prob.data(), reg_mat);
        Eigen::VectorXd solution;
        full_gen_tikhonov.solve(0.0, solution);

        REQUIRE_THAT(solution, eigen_approx(prob.solution()));
    }

    SECTION("solve with different parameters") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 30;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);
        const coeff_type reg_mat =
            num_prob_collect::regularization::dense_diff_matrix<coeff_type>(
                solution_size);

        num_collect::regularization::full_gen_tikhonov<coeff_type, data_type>
            full_gen_tikhonov;
        full_gen_tikhonov.compute(prob.coeff(), prob.data(), reg_mat);

        constexpr double param_small = 1e-2;
        Eigen::VectorXd solution_small;
        full_gen_tikhonov.solve(param_small, solution_small);

        constexpr double param_large = 1e+2;
        Eigen::VectorXd solution_large;
        full_gen_tikhonov.solve(param_large, solution_large);

        REQUIRE((reg_mat * solution_large).squaredNorm() <
            (reg_mat * solution_small).squaredNorm());
    }

    SECTION("check functions of the internal solver") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 30;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);
        const coeff_type reg_mat =
            num_prob_collect::regularization::dense_diff_matrix<coeff_type>(
                solution_size);

        num_collect::regularization::full_gen_tikhonov<coeff_type, data_type>
            full_gen_tikhonov;
        full_gen_tikhonov.compute(prob.coeff(), prob.data(), reg_mat);

        constexpr double param = 1e-2;

        REQUIRE_THAT(full_gen_tikhonov.singular_values(),
            eigen_approx(
                full_gen_tikhonov.internal_solver().singular_values()));

        REQUIRE_THAT(full_gen_tikhonov.residual_norm(param),
            Catch::Matchers::WithinRel(
                full_gen_tikhonov.internal_solver().residual_norm(param)));
        REQUIRE_THAT(full_gen_tikhonov.regularization_term(param),
            Catch::Matchers::WithinRel(
                full_gen_tikhonov.internal_solver().regularization_term(
                    param)));

        REQUIRE_THAT(full_gen_tikhonov.first_derivative_of_residual_norm(param),
            Catch::Matchers::WithinRel(
                full_gen_tikhonov.internal_solver()
                    .first_derivative_of_residual_norm(param)));
        REQUIRE_THAT(
            full_gen_tikhonov.first_derivative_of_regularization_term(param),
            Catch::Matchers::WithinRel(
                full_gen_tikhonov.internal_solver()
                    .first_derivative_of_regularization_term(param)));

        REQUIRE_THAT(
            full_gen_tikhonov.second_derivative_of_residual_norm(param),
            Catch::Matchers::WithinRel(
                full_gen_tikhonov.internal_solver()
                    .second_derivative_of_residual_norm(param)));
        REQUIRE_THAT(
            full_gen_tikhonov.second_derivative_of_regularization_term(param),
            Catch::Matchers::WithinRel(
                full_gen_tikhonov.internal_solver()
                    .second_derivative_of_regularization_term(param)));

        REQUIRE_THAT(full_gen_tikhonov.sum_of_filter_factor(param),
            Catch::Matchers::WithinRel(
                full_gen_tikhonov.internal_solver().sum_of_filter_factor(
                    param)));

        REQUIRE(full_gen_tikhonov.data_size() ==
            full_gen_tikhonov.internal_solver().data_size());

        REQUIRE(full_gen_tikhonov.param_search_region() ==
            full_gen_tikhonov.internal_solver().param_search_region());
    }

    SECTION("try to solve using reg_coeff without full row rank") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 30;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);
        coeff_type reg_mat =
            num_prob_collect::regularization::dense_diff_matrix<coeff_type>(
                solution_size);
        reg_mat.bottomRows(1).setZero();

        num_collect::regularization::full_gen_tikhonov<coeff_type, data_type>
            full_gen_tikhonov;
        REQUIRE_THROWS(
            full_gen_tikhonov.compute(prob.coeff(), prob.data(), reg_mat));
    }

    SECTION("try to solve using same matrices for coeff and reg_coeff") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 3;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);
        const auto& reg_mat = prob.coeff();

        num_collect::regularization::full_gen_tikhonov<coeff_type, data_type>
            full_gen_tikhonov;
        REQUIRE_THROWS(
            full_gen_tikhonov.compute(prob.coeff(), prob.data(), reg_mat));
    }
}
