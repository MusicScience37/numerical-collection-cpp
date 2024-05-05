/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of get_default_scalar_type type.
 */
#pragma once

#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/impl/get_variable_type.h"

namespace num_collect::rbf::impl {

/*!
 * \brief Get the default type of scalars from function signature.
 *
 * \tparam FunctionSignature Function signature.
 */
template <typename FunctionSignature>
using get_default_scalar_type =
    typename distance_functions::euclidean_distance_function<
        impl::get_variable_type_t<FunctionSignature>>::value_type;

}  // namespace num_collect::rbf::impl
