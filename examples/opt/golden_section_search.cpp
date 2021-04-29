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
 * \brief Test of golden_section_search class.
 */
#include "num_collect/opt/golden_section_search.h"

#include <iostream>

#include "num_prob_collect/opt/quadratic_function.h"

auto main() -> int {
    num_collect::opt::golden_section_search<
        num_prob_collect::opt::quadratic_function>
        optimizer;
    constexpr double left = -1.0;
    constexpr double right = 2.0;
    optimizer.init(left, right);
    optimizer.solve(std::cout);
}
