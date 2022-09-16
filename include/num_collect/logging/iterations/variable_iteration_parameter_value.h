/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of variable_iteration_parameter_value class.
 */
#pragma once

#include "num_collect/logging/concepts/formattable_iteration_parameter_value.h"  // IWYU pragma: keep

namespace num_collect::logging::iterations {

/*!
 * \brief Class of parameters values in iterations specified by variables.
 *
 * \tparam Algorithm Type of the algorithm.
 * \tparam Value Type of values.
 */
template <typename Algorithm,
    concepts::formattable_iteration_parameter_value Value>
class variable_iteration_parameter_value {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] value Value.
     */
    explicit variable_iteration_parameter_value(const Value& value)
        : value_(&value) {}

    /*!
     * \brief Get the current value.
     *
     * \return Value.
     */
    [[nodiscard]] auto get() const noexcept -> const Value& { return *value_; }

    /*!
     * \brief Get the current value.
     *
     * \return Value.
     */
    [[nodiscard]] auto get(const Algorithm* /*algorithm*/) const noexcept
        -> const Value& {
        return get();
    }

private:
    //! Pointer to the value.
    const Value* value_;
};

}  // namespace num_collect::logging::iterations
