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
 * \brief Example of backward-mode automatic differentiation.
 */
#include <cmath>
#include <iomanip>
#include <iostream>

#include "num_collect/auto_diff/backward/differentiate.h"
#include "num_collect/auto_diff/backward/variable.h"
#include "num_collect/auto_diff/backward/variable_math.h"  // IWYU pragma: keep
#include "xexp.h"

auto main() -> int {
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable;
    using num_collect::auto_diff::backward::variable_tag;

    const auto var = variable<double>(1.234, variable_tag());
    const variable<double> val = xexp(var);
    const double derivative = differentiate(val, var);

    constexpr int precision = 15;
    std::cout << std::setprecision(precision);
    std::cout << "Value:      " << val.value() << std::endl;
    std::cout << "Derivative: " << derivative << std::endl;
    std::cout << "Reference:  "
              << std::exp(var.value()) + var.value() * std::exp(var.value())
              << std::endl;

    return 0;
}
