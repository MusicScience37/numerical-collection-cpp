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

    SECTION("singular_values") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 30;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);

        num_collect::regularization::tikhonov<coeff_type, data_type> tikhonov;
        tikhonov.compute(prob.coeff(), prob.data());

        REQUIRE(tikhonov.singular_values().size() == solution_size);
        for (num_collect::index_type i = 0; i < solution_size; ++i) {
            INFO("i = " << i);
            REQUIRE(tikhonov.singular_values()(i) > 0.0);
        }
    }

    SECTION("calculate norms") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 10;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);

        num_collect::regularization::tikhonov<coeff_type, data_type> tikhonov;
        tikhonov.compute(prob.coeff(), prob.data());

        constexpr double param = 1.0;
        Eigen::VectorXd solution;
        tikhonov.solve(param, solution);

        constexpr double rel_tol = 1e-6;

        SECTION("residual_norm") {
            const double expected =
                (prob.coeff() * solution - prob.data()).squaredNorm();
            REQUIRE_THAT(tikhonov.residual_norm(param),
                Catch::Matchers::WithinRel(expected, rel_tol));
        }

        SECTION("regularization term") {
            const double expected = solution.squaredNorm();
            REQUIRE_THAT(tikhonov.regularization_term(param),
                Catch::Matchers::WithinRel(expected, rel_tol));
        }
    }

    SECTION("calculate the first-order derivatives of norms") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 10;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);

        num_collect::regularization::tikhonov<coeff_type, data_type> tikhonov;
        tikhonov.compute(prob.coeff(), prob.data());

        constexpr double param = 1.0;
        Eigen::VectorXd solution;
        tikhonov.solve(param, solution);

        constexpr double param_diff = param * 1e-3;
        constexpr double param_plus = param + param_diff;
        Eigen::VectorXd solution_plus;
        tikhonov.solve(param_plus, solution_plus);

        constexpr double rel_tol = 1e-2;

        SECTION("first_derivative_of_residual_norm") {
            const double expected =
                ((prob.coeff() * solution_plus - prob.data()).squaredNorm() -
                    (prob.coeff() * solution - prob.data()).squaredNorm()) /
                param_diff;
            REQUIRE_THAT(tikhonov.first_derivative_of_residual_norm(param),
                Catch::Matchers::WithinRel(expected, rel_tol));
        }

        SECTION("first_derivative_of_regularization_term") {
            const double expected =
                (solution_plus.squaredNorm() - solution.squaredNorm()) /
                param_diff;
            REQUIRE_THAT(
                tikhonov.first_derivative_of_regularization_term(param),
                Catch::Matchers::WithinRel(expected, rel_tol));
        }
    }

    SECTION("calculate the second-order derivatives of norms") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 10;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);

        num_collect::regularization::tikhonov<coeff_type, data_type> tikhonov;
        tikhonov.compute(prob.coeff(), prob.data());

        constexpr double param = 1.0;
        Eigen::VectorXd solution;
        tikhonov.solve(param, solution);

        constexpr double param_diff = param * 1e-3;
        constexpr double param_plus = param + param_diff;
        Eigen::VectorXd solution_plus;
        tikhonov.solve(param_plus, solution_plus);

        constexpr double param_minus = param - param_diff;
        Eigen::VectorXd solution_minus;
        tikhonov.solve(param_minus, solution_minus);

        constexpr double rel_tol = 1e-2;

        SECTION("second_derivative_of_residual_norm") {
            const double expected =
                ((prob.coeff() * solution_plus - prob.data()).squaredNorm() -
                    2.0 *
                        (prob.coeff() * solution - prob.data()).squaredNorm() +
                    (prob.coeff() * solution_minus - prob.data())
                        .squaredNorm()) /
                (param_diff * param_diff);
            REQUIRE_THAT(tikhonov.second_derivative_of_residual_norm(param),
                Catch::Matchers::WithinRel(expected, rel_tol));
        }

        SECTION("second_derivative_of_regularization_term") {
            const double expected =
                (solution_plus.squaredNorm() - 2.0 * solution.squaredNorm() +
                    solution_minus.squaredNorm()) /
                (param_diff * param_diff);
            REQUIRE_THAT(
                tikhonov.second_derivative_of_regularization_term(param),
                Catch::Matchers::WithinRel(expected, rel_tol));
        }
    }

    SECTION("sum_of_filter_factor") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 10;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);

        num_collect::regularization::tikhonov<coeff_type, data_type> tikhonov;
        tikhonov.compute(prob.coeff(), prob.data());

        constexpr double param = 1.0;

        const double expected = (tikhonov.singular_values().array().square() /
            (tikhonov.singular_values().array().square() + param))
                                    .sum();
        REQUIRE_THAT(tikhonov.sum_of_filter_factor(param),
            Catch::Matchers::WithinRel(expected));
    }

    SECTION("data_size") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 10;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);

        num_collect::regularization::tikhonov<coeff_type, data_type> tikhonov;
        tikhonov.compute(prob.coeff(), prob.data());

        REQUIRE(tikhonov.data_size() == data_size);
    }

    SECTION("param_search_region") {
        constexpr num_collect::index_type solution_size = 15;
        constexpr num_collect::index_type data_size = 10;
        const auto prob = num_prob_collect::regularization::blur_sine(
            data_size, solution_size);

        num_collect::regularization::tikhonov<coeff_type, data_type> tikhonov;
        tikhonov.compute(prob.coeff(), prob.data());

        const double max_singular_value = tikhonov.singular_values().maxCoeff();
        const double squared_max_singular_value =
            max_singular_value * max_singular_value;
        const auto [min_param, max_param] = tikhonov.param_search_region();
        REQUIRE(min_param < squared_max_singular_value);
        REQUIRE(max_param > squared_max_singular_value);
    }
}
