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
 * \brief Test of avf2_formula class.
 */
#include "num_collect/ode/avf/avf2_formula.h"

#include <cmath>
#include <string>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <fmt/format.h>

#include "comparison_approvals.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::avf::avf2_formula") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using formula_type = num_collect::ode::avf::avf2_formula<problem_type>;

    SECTION("static definition") { STATIC_REQUIRE(formula_type::order == 2); }

    SECTION("initialize") {
        auto formula = formula_type(problem_type());
        (void)formula;
    }

    SECTION("step") {
        auto formula = formula_type(problem_type());

        constexpr double time = 0.0;
        constexpr double step_size = 1e-4;
        const Eigen::Vector2d prev_var = Eigen::Vector2d(1.0, 0.0);
        Eigen::Vector2d next_var;
        formula.step(time, step_size, prev_var, next_var);

        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(step_size), std::sin(step_size));
        comparison_approvals::verify_with_reference(next_var, reference);
    }
}
