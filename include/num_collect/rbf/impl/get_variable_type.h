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
 * \brief Definition of get_variable_type class.
 */
#pragma once

namespace num_collect::rbf::impl {

/*!
 * \brief Get the type of variables from function signature.
 *
 * \tparam FunctionSignature Function signature.
 */
template <typename FunctionSignature>
struct get_variable_type;

/*!
 * \brief Get the type of variables from function signature.
 *
 * \tparam Variable Type of variables.
 * \tparam FunctionValue Type of function values.
 */
template <typename Variable, typename FunctionValue>
struct get_variable_type<FunctionValue(Variable)> {
    //! Type.
    using type = Variable;
};

/*!
 * \brief Get the type of variables from function signature.
 *
 * \tparam FunctionSignature Function signature.
 */
template <typename FunctionSignature>
using get_variable_type_t = typename get_variable_type<FunctionSignature>::type;

}  // namespace num_collect::rbf::impl
