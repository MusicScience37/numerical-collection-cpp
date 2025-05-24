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
 * \brief Test of adc_sample_dict class.
 */
#include "num_collect/opt/impl/adc_sample_dict.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/opt/impl/adc_ternary_vector.h"
#include "num_prob_collect/opt/multi_quadratic_function.h"

TEST_CASE("num_collect::opt::impl::adc_sample_dict") {
    constexpr num_collect::index_type max_digits = 3;
    using dict_type = num_collect::opt::impl::adc_sample_dict<
        num_prob_collect::opt::multi_quadratic_function, max_digits>;
    using ternary_vector_type = typename dict_type::ternary_vector_type;

    auto dict = dict_type();

    SECTION("init") {
        dict.init(Eigen::VectorXd::Constant(2, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(2, 2.0));        // NOLINT
        CHECK(dict.evaluations() == 0);
        CHECK(dict.dim() == 2);
    }

    SECTION("evaluate once") {
        dict.init(Eigen::VectorXd::Constant(2, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(2, 2.0));        // NOLINT
        auto point = ternary_vector_type(2);
        point.push_back(0);
        point.push_back(0);
        point.push_back(1);

        const Eigen::VectorXd var =
            (Eigen::VectorXd(2) << 0.0, -1.0).finished();
        auto func = num_prob_collect::opt::multi_quadratic_function();
        func.evaluate_on(var);
        const auto value = func.value();
        CHECK_THAT(dict(point), Catch::Matchers::WithinRel(value));

        CHECK(dict.opt_point() == point);
        CHECK_THAT(dict.opt_variable(), eigen_approx(var));
        CHECK_THAT(dict.opt_value(), Catch::Matchers::WithinRel(value));
        CHECK(dict.evaluations() == 1);
    }

    SECTION("evaluate twice") {
        dict.init(Eigen::VectorXd::Constant(2, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(2, 2.0));        // NOLINT
        auto point = ternary_vector_type(2);
        point.push_back(0);
        point.push_back(0);
        point.push_back(1);

        const Eigen::VectorXd var =
            (Eigen::VectorXd(2) << 0.0, -1.0).finished();
        auto func = num_prob_collect::opt::multi_quadratic_function();
        func.evaluate_on(var);
        const auto value = func.value();
        CHECK_THAT(dict(point), Catch::Matchers::WithinRel(value));

        CHECK_THAT(dict.opt_variable(), eigen_approx(var));
        CHECK_THAT(dict.opt_value(), Catch::Matchers::WithinRel(value));
        CHECK(dict.evaluations() == 1);

        CHECK_THAT(dict(point), Catch::Matchers::WithinRel(value));
        CHECK(dict.evaluations() == 1);
    }

    SECTION("evaluate multiple points") {
        dict.init(Eigen::VectorXd::Constant(2, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(2, 2.0));        // NOLINT
        auto point = ternary_vector_type(2);
        point.push_back(0);
        point.push_back(0);
        point.push_back(1);

        const Eigen::VectorXd var =
            (Eigen::VectorXd(2) << 0.0, -1.0).finished();
        auto func = num_prob_collect::opt::multi_quadratic_function();
        func.evaluate_on(var);
        const auto value = func.value();
        CHECK_THAT(dict(point), Catch::Matchers::WithinRel(value));
        CHECK_THAT(dict.opt_variable(), eigen_approx(var));
        CHECK_THAT(dict.opt_value(), Catch::Matchers::WithinRel(value));
        CHECK(dict.evaluations() == 1);

        point.push_back(3);
        const Eigen::VectorXd var2 =
            (Eigen::VectorXd(2) << 0.0, 2.0).finished();
        func.evaluate_on(var2);
        const auto value2 = func.value();
        CHECK_THAT(dict(point), Catch::Matchers::WithinRel(value2));
        CHECK_THAT(dict.opt_variable(), eigen_approx(var));
        CHECK_THAT(dict.opt_value(), Catch::Matchers::WithinRel(value));
        CHECK(dict.evaluations() == 2);

        point(1, 1) = 1;
        const Eigen::VectorXd var3 =
            (Eigen::VectorXd(2) << 0.0, 0.0).finished();
        constexpr double value3 = 0.0;
        constexpr double margin = 1e-12;
        CHECK_THAT(dict(point), Catch::Matchers::WithinAbs(value3, margin));
        CHECK_THAT(dict.opt_variable(), eigen_approx(var3));
        CHECK_THAT(
            dict.opt_value(), Catch::Matchers::WithinAbs(value3, margin));
        CHECK(dict.evaluations() == 3);
    }
}
