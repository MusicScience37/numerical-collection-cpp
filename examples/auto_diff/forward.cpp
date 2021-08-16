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
 * \brief Example of forward-mode automatic differentiation.
 */
#include <cmath>
#include <iomanip>
#include <iostream>

#include "num_collect/auto_diff/forward/create_diff_variable.h"
#include "num_collect/auto_diff/forward/variable.h"
#include "num_collect/auto_diff/forward/variable_math.h"
#include "xexp.h"

auto main() -> int {
    using num_collect::auto_diff::forward::create_diff_variable;
    using num_collect::auto_diff::forward::variable;

    const variable<double> var = create_diff_variable(1.234);
    const variable<double> val = xexp(var);

    constexpr int precision = 15;
    std::cout << std::setprecision(precision);
    std::cout << "Value:      " << val.value() << std::endl;
    std::cout << "Derivative: " << val.diff() << std::endl;
    std::cout << "Reference:  "
              << std::exp(var.value()) + var.value() * std::exp(var.value())
              << std::endl;

    return 0;
}
