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
 * \brief Test of adaptive_diagonal_curves class.
 */
#include "num_collect/opt/adaptive_diagonal_curves.h"

#include <iostream>

#include "num_prob_collect/opt/shekel_function.h"

auto main() -> int {
    num_collect::opt::adaptive_diagonal_curves<
        num_prob_collect::opt::shekel_function>
        optimizer(num_prob_collect::opt::shekel_function(5));  // NOLINT
    // NOLINTNEXTLINE
    optimizer.init(Eigen::Vector4d::Zero(), Eigen::Vector4d::Constant(10.0));
    optimizer.max_evaluations(10000);  // NOLINT
    optimizer.min_rate_imp(1e-4);     // NOLINT
    optimizer.solve(std::cout);
}
