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
 * \brief Test of get_least_known_order function.
 */
#include "num_collect/ode/impl/get_least_known_order.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/ode/runge_kutta/rk4_formula.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE("num_collect::ode::impl::get_least_known_order") {
    using num_collect::ode::impl::get_least_known_order;
    using num_collect::ode::runge_kutta::rk4_formula;
    using num_collect::ode::runge_kutta::rkf45_formula;
    using num_prob_collect::ode::exponential_problem;
    using num_prob_collect::ode::spring_movement_problem;

    SECTION("get") {
        SECTION("when the lesser order is known") {
            using problem_type = spring_movement_problem;
            using formula_type = rkf45_formula<problem_type>;
            STATIC_CHECK(get_least_known_order<formula_type>() == 4);
            CHECK(get_least_known_order<formula_type>() == 4);
        }

        SECTION("when the lesser order is not known") {
            using problem_type = spring_movement_problem;
            using formula_type = rk4_formula<problem_type>;
            STATIC_CHECK(get_least_known_order<formula_type>() == 4);
            CHECK(get_least_known_order<formula_type>() == 4);
        }
    }
}
