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
 * \brief Definition of differentiate function.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/auto_diff/backward/graph/node.h"
#include "num_collect/auto_diff/backward/graph/node_differentiator.h"
#include "num_collect/auto_diff/backward/variable.h"

namespace num_collect::auto_diff::backward {

/*!
 * \brief Compute a differential coefficient.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] func_value Variable of the function value.
 * \param[in] arg Variable of the argument.
 * \return Differential coefficient.
 */
template <typename Scalar>
[[nodiscard]] inline auto differentiate(
    const variable<Scalar>& func_value, const variable<Scalar>& arg) -> Scalar {
    graph::node_differentiator<Scalar> diff;
    diff.compute(func_value.node());
    return diff.coeff(arg.node());
}

}  // namespace num_collect::auto_diff::backward
