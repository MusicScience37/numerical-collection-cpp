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
 * \brief Example of kahan_adder class.
 */
#include "num_collect/util/kahan_adder.h"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>

#include "num_collect/constants/pi.h"

auto main() -> int {
    // calculate zeta(4) using kahan_adder
    auto sum = num_collect::util::kahan_adder<double>();
    constexpr std::size_t terms = 100000;
    for (std::size_t i = 1; i <= terms; ++i) {
        const auto i_d = static_cast<double>(i);
        const double term = 1.0 / (i_d * i_d * i_d * i_d);
        sum += term;
    }
    const double val = sum;

    const double reference = std::pow(num_collect::constants::pid, 4) / 90.0;
    std::cout << std::setprecision(15);  // NOLINT
    std::cout << "Value:     " << val << std::endl;
    std::cout << "Reference: " << reference << std::endl;

    return 0;
}
