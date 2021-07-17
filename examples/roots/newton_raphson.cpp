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
 * \brief Test of newton_raphson class.
 */
#include "num_collect/roots/newton_raphson.h"

#include <iomanip>
#include <iostream>

#include "num_prob_collect/roots/cubic_root_test_function.h"

auto main() -> int {
    constexpr double target = 3.0;
    auto finder = num_collect::roots::newton_raphson<
        num_prob_collect::roots::cubic_root_test_function>(
        num_prob_collect::roots::cubic_root_test_function(target));
    finder.init(target);
    finder.solve(std::cout);

    constexpr int prec = 15;
    std::cout << std::setprecision(prec);
    std::cout << "solver:   " << finder.variable() << "\n";
    const double reference = std::pow(target, 1.0 / 3.0);
    std::cout << "std::pow: " << reference << std::endl;

    return 0;
}
