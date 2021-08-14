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
 * \brief Definition of create_diff_variable function.
 */
#pragma once

#include "num_collect/auto_diff/forward/variable.h"

namespace num_collect::auto_diff::forward {

/*!
 * \brief Create a variable by which functions will be differentiated
 *        (for scalar differential coefficients).
 *
 * This will set the differential coefficient to one.
 *
 * \tparam Value Type of the value.
 * \tparam Diff Type of the differential coefficient.
 * \param[in] value Value.
 * \return Variable.
 */
template <typename Value>
[[nodiscard]] inline auto create_diff_variable(const Value& value)
    -> variable<Value> {
    return variable<Value>(value, static_cast<Value>(1));
}

/*!
 * \brief Create a variable by which functions will be differentiated
 *        (for vector differential coefficients).
 *
 * \tparam Value Type of the value.
 * \tparam Diff Type of the differential coefficients.
 * \param[in] value Value.
 * \param[in] size Size of Diff.
 * \param[in] index Index of the variable.
 * \return Variable.
 */
template <typename Value, typename Diff,
    std::enable_if_t<is_eigen_vector_v<Diff>, void*> = nullptr>
[[nodiscard]] inline auto create_diff_variable(const Value& value,
    index_type size, index_type index) -> variable<Value, Diff> {
    return variable<Value, Diff>(value, Diff::Unit(size, index));
}

}  // namespace num_collect::auto_diff::forward
