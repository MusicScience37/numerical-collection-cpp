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
 * \brief Test of adaptive_diagonal_curves class.
 */
#include "num_collect/opt/adaptive_diagonal_curves.h"

#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "eigen_approx.h"
#include "num_prob_collect/opt/multi_quadratic_function.h"

TEST_CASE("num_collect::opt::impl::adc_sample_dict") {
    using dict_type = num_collect::opt::impl::adc_sample_dict<
        num_prob_collect::opt::multi_quadratic_function>;
    using num_collect::opt::impl::ternary_vector;

    auto dict = dict_type();

    SECTION("init") {
        dict.init(Eigen::VectorXd::Constant(2, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(2, 2.0));        // NOLINT
        REQUIRE(dict.evaluations() == 0);
    }

    SECTION("evaluate once") {
        dict.init(Eigen::VectorXd::Constant(2, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(2, 2.0));        // NOLINT
        auto point = ternary_vector(2);
        point.push_back(0, 0);
        point.push_back(0, 1);
        point.push_back(1, 0);

        const Eigen::VectorXd var =
            (Eigen::VectorXd(2) << 0.0, -1.0).finished();
        auto func = num_prob_collect::opt::multi_quadratic_function();
        func.evaluate_on(var);
        const auto value = func.value();
        REQUIRE_THAT(dict(point), Catch::Matchers::WithinRel(value));
        REQUIRE_THAT(dict.opt_variable(), eigen_approx(var));
        REQUIRE_THAT(dict.opt_value(), Catch::Matchers::WithinRel(value));
        REQUIRE(dict.evaluations() == 1);
    }

    SECTION("evaluate twice") {
        dict.init(Eigen::VectorXd::Constant(2, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(2, 2.0));        // NOLINT
        auto point = ternary_vector(2);
        point.push_back(0, 0);
        point.push_back(0, 1);
        point.push_back(1, 0);

        const Eigen::VectorXd var =
            (Eigen::VectorXd(2) << 0.0, -1.0).finished();
        auto func = num_prob_collect::opt::multi_quadratic_function();
        func.evaluate_on(var);
        const auto value = func.value();
        REQUIRE_THAT(dict(point), Catch::Matchers::WithinRel(value));
        REQUIRE_THAT(dict.opt_variable(), eigen_approx(var));
        REQUIRE_THAT(dict.opt_value(), Catch::Matchers::WithinRel(value));
        REQUIRE(dict.evaluations() == 1);

        REQUIRE_THAT(dict(point), Catch::Matchers::WithinRel(value));
        REQUIRE(dict.evaluations() == 1);
    }

    SECTION("evaluate multiple points") {
        dict.init(Eigen::VectorXd::Constant(2, -1.0),  // NOLINT
            Eigen::VectorXd::Constant(2, 2.0));        // NOLINT
        auto point = ternary_vector(2);
        point.push_back(0, 0);
        point.push_back(0, 1);
        point.push_back(1, 0);

        const Eigen::VectorXd var =
            (Eigen::VectorXd(2) << 0.0, -1.0).finished();
        auto func = num_prob_collect::opt::multi_quadratic_function();
        func.evaluate_on(var);
        const auto value = func.value();
        REQUIRE_THAT(dict(point), Catch::Matchers::WithinRel(value));
        REQUIRE_THAT(dict.opt_variable(), eigen_approx(var));
        REQUIRE_THAT(dict.opt_value(), Catch::Matchers::WithinRel(value));
        REQUIRE(dict.evaluations() == 1);

        point.push_back(1, 2);
        const Eigen::VectorXd var2 =
            (Eigen::VectorXd(2) << 0.0, 1.0).finished();
        func.evaluate_on(var);
        const auto value2 = func.value();
        REQUIRE_THAT(dict(point), Catch::Matchers::WithinRel(value2));
        REQUIRE_THAT(dict.opt_variable(), eigen_approx(var));
        REQUIRE_THAT(dict.opt_value(), Catch::Matchers::WithinRel(value));
        REQUIRE(dict.evaluations() == 2);

        point(1, 1) = 1;
        const Eigen::VectorXd var3 =
            (Eigen::VectorXd(2) << 0.0, 0.0).finished();
        constexpr double value3 = 0.0;
        constexpr double mergin = 1e-12;
        REQUIRE_THAT(dict(point), Catch::Matchers::WithinAbs(value3, mergin));
        REQUIRE_THAT(dict.opt_variable(), eigen_approx(var3));
        REQUIRE_THAT(
            dict.opt_value(), Catch::Matchers::WithinAbs(value3, mergin));
        REQUIRE(dict.evaluations() == 3);
    }
}

TEST_CASE("num_collect::opt::impl::adc_rectangle") {
    using rectangle_type = num_collect::opt::impl::adc_rectangle<double>;
    using num_collect::opt::impl::ternary_vector;

    SECTION("construct") {
        auto vertex = ternary_vector(2);
        vertex.push_back(0, 0);
        vertex.push_back(0, 1);
        vertex.push_back(1, 0);
        constexpr double ave_value = 3.14;
        const auto rect = rectangle_type(vertex, ave_value);

        REQUIRE(rect.vertex() == vertex);
        REQUIRE_THAT(rect.ave_value(), Catch::Matchers::WithinRel(ave_value));

        auto sample_points = std::make_pair(vertex, vertex);
        sample_points.first(0, 1) = static_cast<ternary_vector::digit_type>(2);
        sample_points.second(1, 0) = static_cast<ternary_vector::digit_type>(1);
        REQUIRE(rect.sample_points() == sample_points);

        const double dist = 0.5 * std::sqrt(1.0 / 9.0 + 1.0);
        REQUIRE_THAT(rect.dist(), Catch::Matchers::WithinRel(dist));
    }
}
