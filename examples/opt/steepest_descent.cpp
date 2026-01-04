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
 * \brief Example to use steepest_descent class.
 */
#include "num_collect/opt/steepest_descent.h"

#include <Eigen/Core>

#include "configure_logging.h"
#include "num_prob_collect/opt/multi_quadratic_function.h"

auto main() -> int {
    configure_logging();
    num_collect::opt::steepest_descent<
        num_prob_collect::opt::multi_quadratic_function>
        optimizer;
    const Eigen::VectorXd init_var =
        (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
    optimizer.init(init_var);
    optimizer.solve();
}
