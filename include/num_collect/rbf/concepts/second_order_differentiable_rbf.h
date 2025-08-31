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
 * \brief Definition of second_order_differentiable_rbf concept.
 */
#pragma once

#include "num_collect/rbf/concepts/differentiable_rbf.h"
#include "num_collect/rbf/rbfs/differentiated.h"

namespace num_collect::rbf::concepts {

/*!
 * \brief Concept of second-order differentiable RBFs.
 *
 * \tparam T Type.
 */
template <typename T>
concept second_order_differentiable_rbf =
    differentiable_rbf<T> && differentiable_rbf<rbfs::differentiated_t<T>>;

}  // namespace num_collect::rbf::concepts
