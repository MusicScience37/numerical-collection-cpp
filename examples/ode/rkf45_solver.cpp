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
 * \brief Example to use rkf45_solver class.
 */
#include <cmath>
#include <iomanip>
#include <iostream>

#include "configure_logging.h"
#include "num_collect/ode/runge_kutta/rkf45_formula.h"
#include "num_prob_collect/ode/exponential_problem.h"

auto main() -> int {
    configure_logging();

    using problem_type = num_prob_collect::ode::exponential_problem;
    using solver_type =
        num_collect::ode::runge_kutta::rkf45_solver<problem_type>;

    auto solver = solver_type(problem_type());
    constexpr double init_time = 0.0;
    constexpr double init_var = 1.0;
    constexpr double end_time = 1.0;
    solver.init(init_time, init_var);
    solver.solve_till(end_time);

    constexpr int precision = 15;
    std::cout << std::setprecision(precision);
    std::cout << "Solution:  " << solver.variable() << "\n";
    std::cout << "Reference: " << std::exp(end_time) << std::endl;

    return 0;
}
