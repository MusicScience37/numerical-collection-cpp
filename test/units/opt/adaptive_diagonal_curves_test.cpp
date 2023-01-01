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

#include <functional>
#include <string>
#include <type_traits>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <fmt/format.h>

#include "eigen_approx.h"
#include "num_collect/opt/concepts/box_constrained_optimizer.h"  // IWYU pragma: keep
#include "num_collect/opt/concepts/optimizer.h"  // IWYU pragma: keep
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
        REQUIRE(dict.dim() == 2);
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
        REQUIRE(dict.opt_point() == point);
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

        point.push_back(1, 3);
        const Eigen::VectorXd var2 =
            (Eigen::VectorXd(2) << 0.0, 2.0).finished();
        func.evaluate_on(var2);
        const auto value2 = func.value();
        REQUIRE_THAT(dict(point), Catch::Matchers::WithinRel(value2));
        REQUIRE_THAT(dict.opt_variable(), eigen_approx(var));
        REQUIRE_THAT(dict.opt_value(), Catch::Matchers::WithinRel(value));
        REQUIRE(dict.evaluations() == 2);

        point(1, 1) = 1;
        const Eigen::VectorXd var3 =
            (Eigen::VectorXd(2) << 0.0, 0.0).finished();
        constexpr double value3 = 0.0;
        constexpr double margin = 1e-12;
        REQUIRE_THAT(dict(point), Catch::Matchers::WithinAbs(value3, margin));
        REQUIRE_THAT(dict.opt_variable(), eigen_approx(var3));
        REQUIRE_THAT(
            dict.opt_value(), Catch::Matchers::WithinAbs(value3, margin));
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

TEST_CASE("num_collect::opt::impl::adc_group") {
    using group_type = num_collect::opt::impl::adc_group<double>;
    using rectangle_type = typename group_type::rectangle_type;
    using num_collect::opt::impl::ternary_vector;

    constexpr double dist = 0.1;
    auto group = group_type(dist);

    SECTION("construct") {
        REQUIRE_THAT(group.dist(), Catch::Matchers::WithinRel(dist));
        REQUIRE(group.empty());
    }

    SECTION("push a rectangle") {
        auto point1 = ternary_vector(2);
        point1.push_back(0, 0);
        point1.push_back(0, 1);
        point1.push_back(1, 0);
        constexpr double ave_value1 = 3.14;
        group.push(std::make_shared<rectangle_type>(point1, ave_value1));

        REQUIRE(group.min_rect()->vertex() == point1);
        REQUIRE_FALSE(group.empty());
    }

    SECTION("push rectangles") {
        auto point1 = ternary_vector(2);
        point1.push_back(0, 0);
        point1.push_back(0, 1);
        point1.push_back(1, 0);
        constexpr double ave_value1 = 3.14;
        group.push(std::make_shared<rectangle_type>(point1, ave_value1));

        auto point2 = ternary_vector(2);
        point2.push_back(0, 0);
        point2.push_back(1, 0);
        constexpr double ave_value2 = 1.23;
        group.push(std::make_shared<rectangle_type>(point2, ave_value2));

        REQUIRE(group.min_rect()->vertex() == point2);
        REQUIRE_FALSE(group.empty());
    }

    SECTION("pop rectangles") {
        auto point1 = ternary_vector(2);
        point1.push_back(0, 0);
        point1.push_back(0, 1);
        point1.push_back(1, 0);
        constexpr double ave_value1 = 3.14;
        group.push(std::make_shared<rectangle_type>(point1, ave_value1));

        auto point2 = ternary_vector(2);
        point2.push_back(0, 0);
        point2.push_back(1, 0);
        constexpr double ave_value2 = 1.23;
        group.push(std::make_shared<rectangle_type>(point2, ave_value2));

        auto popped_rect = group.pop();
        REQUIRE(popped_rect->vertex() == point2);
        REQUIRE(group.min_rect()->vertex() == point1);
        REQUIRE_FALSE(group.empty());

        popped_rect = group.pop();
        REQUIRE(popped_rect->vertex() == point1);
        REQUIRE(group.empty());
    }
}

TEST_CASE("num_collect::opt::adaptive_diagonal_curves") {
    using num_collect::opt::adaptive_diagonal_curves;
    using num_prob_collect::opt::multi_quadratic_function;

    SECTION("concepts") {
        STATIC_REQUIRE(num_collect::opt::concepts::optimizer<
            adaptive_diagonal_curves<multi_quadratic_function>>);
        STATIC_REQUIRE(num_collect::opt::concepts::box_constrained_optimizer<
            adaptive_diagonal_curves<multi_quadratic_function>>);
    }

    SECTION("init") {
        auto opt = adaptive_diagonal_curves<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -2.0),  // NOLINT
            Eigen::VectorXd::Constant(3, 2.0));       // NOLINT
        REQUIRE(opt.iterations() == 0);
        REQUIRE(opt.evaluations() == 2);
        REQUIRE_THAT(opt.opt_variable(),
            eigen_approx(Eigen::VectorXd::Constant(3, -2.0)));  // NOLINT
    }

    SECTION("iterate") {
        auto opt = adaptive_diagonal_curves<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -2.0),  // NOLINT
            Eigen::VectorXd::Constant(3, 2.0));       // NOLINT
        const auto prev_value = opt.opt_value();

        opt.iterate();
        REQUIRE(opt.iterations() == 1);
        REQUIRE(opt.evaluations() == 4);
        REQUIRE(opt.opt_value() <= prev_value);
    }

    SECTION("solve") {
        auto opt = adaptive_diagonal_curves<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -2.0),  // NOLINT
            Eigen::VectorXd::Constant(3, 2.0));       // NOLINT
        constexpr double sol_tol = 1e-2;
        opt.max_evaluations(1000);  // NOLINT
        opt.solve();
        REQUIRE_THAT(opt.opt_variable(),
            eigen_approx(Eigen::VectorXd::Zero(3), sol_tol));
        REQUIRE_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, sol_tol));
    }

    SECTION("check global phase execution") {
        auto opt = adaptive_diagonal_curves<multi_quadratic_function>();
        opt.init(Eigen::VectorXd::Constant(3, -2.0),  // NOLINT
            Eigen::VectorXd::Constant(3, 2.0));       // NOLINT
        constexpr double sol_tol = 1e-2;
        opt.max_evaluations(1000);     // NOLINT
        opt.decrease_rate_bound(1.0);  // NOLINT
        REQUIRE_NOTHROW(opt.solve());
        REQUIRE_THAT(opt.opt_variable(),
            eigen_approx(Eigen::VectorXd::Zero(3), sol_tol));
        REQUIRE_THAT(opt.opt_value(), Catch::Matchers::WithinAbs(0.0, sol_tol));
    }
}
