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
 * \brief Declaration of operator_evaluator struct.
 */
#pragma once

#include "num_collect/rbf/concepts/distance_function.h"
#include "num_collect/rbf/concepts/rbf.h"

namespace num_collect::rbf::operators {

/*!
 * \brief Traits to implement evaluation of operators.
 *
 * \tparam Operator Type of the operator.
 * \tparam RBF Type of the RBF.
 * \tparam DistanceFunction Type of the distance function.
 */
template <typename Operator, concepts::rbf RBF,
    concepts::distance_function DistanceFunction>
struct operator_evaluator;

}  // namespace num_collect::rbf::operators
