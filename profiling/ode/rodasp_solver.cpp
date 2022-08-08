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
 * \brief Profiling of ODE solver using RODASP formula.
 */
#include <gperftools/profiler.h>

#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_prob_collect/ode/free_fall_in_resistance_problem.h"

static void test() {
    using problem_type = num_prob_collect::ode::free_fall_in_resistance_problem;
    using solver_type =
        num_collect::ode::rosenbrock::rodasp_solver<problem_type>;

    constexpr double k = 1e+3;
    constexpr double g = 1.0;
    constexpr double init_time = 0.0;
    constexpr double end_time = 1000.0;
    const Eigen::Vector2d init_var = Eigen::Vector2d(0.0, 0.0);

    solver_type solver{problem_type{k, g}};
    solver.init(init_time, init_var);
    solver.solve_till(end_time);
}

auto main() -> int {
    constexpr std::size_t repetition = 50000;
    ProfilerStart("num_collect_prof_rodasp_solver.prof");
    for (std::size_t i = 0; i < repetition; ++i) {
        test();
    }
    ProfilerStop();
    return 0;
}
