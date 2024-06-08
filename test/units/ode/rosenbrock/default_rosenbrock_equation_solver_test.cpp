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
 * \brief Test of default_rosenbrock_equation_solver class.
 */
#include "num_collect/ode/rosenbrock/default_rosenbrock_equation_solver.h"

#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/ode/concepts/rosenbrock_equation_solver.h"
#include "num_collect/ode/rosenbrock/lu_rosenbrock_equation_solver.h"
#include "num_collect/ode/rosenbrock/scalar_rosenbrock_equation_solver.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"
#include "num_prob_collect/ode/string_wave_1d_problem.h"

TEST_CASE(
    "num_collect::ode::rosenbrock::default_rosenbrock_equation_solver_t") {
    using num_collect::ode::concepts::rosenbrock_equation_solver;
    using num_collect::ode::rosenbrock::default_rosenbrock_equation_solver_t;

    SECTION("for single-variate differentiable problem") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        STATIC_REQUIRE(
            std::is_same_v<default_rosenbrock_equation_solver_t<problem_type>,
                num_collect::ode::rosenbrock::scalar_rosenbrock_equation_solver<
                    problem_type>>);
    }

    SECTION("for multi-variate differentiable problem") {
        using problem_type = num_prob_collect::ode::spring_movement_problem;
        STATIC_REQUIRE(
            std::is_same_v<default_rosenbrock_equation_solver_t<problem_type>,
                num_collect::ode::rosenbrock::lu_rosenbrock_equation_solver<
                    problem_type>>);
    }

    SECTION("for multi-variate non-differentiable problem") {
        using problem_type = num_prob_collect::ode::string_wave_1d_problem;
        STATIC_REQUIRE(
            std::is_same_v<default_rosenbrock_equation_solver_t<problem_type>,
                num_collect::ode::rosenbrock::
                    bicgstab_rosenbrock_equation_solver<problem_type>>);
    }
}
