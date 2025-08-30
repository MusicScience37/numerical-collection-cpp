/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of third_order_derivative_operator class.
 */
#pragma once

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/rbf/operators/laplacian_gradient_operator.h"

namespace num_collect::rbf::operators {

/*!
 * \brief Class of operators to evaluate third-order derivatives.
 *
 * \tparam Variable Type of variables.
 *
 * \note This type is defined only for scalar variables.
 */
template <base::concepts::real_scalar Variable>
using third_order_derivative_operator = laplacian_gradient_operator<Variable>;

}  // namespace num_collect::rbf::operators
