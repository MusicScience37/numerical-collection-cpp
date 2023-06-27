/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of implicit_problem_wrapper class.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/implicit_problem_wrapper.h"
#include "num_prob_collect/ode/changing_mass_exponential_problem.h"
#include "num_prob_collect/ode/changing_mass_quadratic_problem.h"
#include "trompeloeil_catch2.h"

TEST_CASE("num_collect::ode::single_variate_implicit_problem_wrapper") {
    using num_collect::ode::single_variate_implicit_problem_wrapper;
    using num_collect::ode::wrap_implicit_problem;
    using num_prob_collect::ode::changing_mass_exponential_problem;
    using num_prob_collect::ode::changing_mass_quadratic_problem;

    SECTION("wrap a single-variate problem") {
        auto wrapped =
            wrap_implicit_problem(changing_mass_exponential_problem());

        constexpr double time = 0.5;
        constexpr double variable = 1.234;
        constexpr num_collect::ode::evaluation_type evaluations{
            .diff_coeff = true};
        wrapped.evaluate_on(time, variable, evaluations);

        CHECK_THAT(wrapped.diff_coeff(), Catch::Matchers::WithinRel(variable));
    }

    SECTION("wrap a multi-variate problem") {
        auto wrapped = wrap_implicit_problem(changing_mass_quadratic_problem());

        constexpr double time = 0.5;
        const auto variable = Eigen::Vector2d(0.5, 0.125);
        constexpr num_collect::ode::evaluation_type evaluations{
            .diff_coeff = true};
        wrapped.evaluate_on(time, variable, evaluations);

        const auto expected_diff = Eigen::Vector2d(1.0, 0.5);
        CHECK_THAT(wrapped.diff_coeff(), eigen_approx(expected_diff));
    }
}
