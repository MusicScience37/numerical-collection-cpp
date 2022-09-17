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
 * \brief Definition of member_function_iteration_parameter_value class.
 */
#pragma once

#include <string_view>
#include <type_traits>

#include "num_collect/base/exception.h"
#include "num_collect/logging/concepts/formattable_iteration_parameter_value.h"  // IWYU pragma: keep
#include "num_collect/logging/concepts/member_getter_of.h"  // IWYU pragma: keep

namespace num_collect::logging::iterations {

/*!
 * \brief Class of parameters values in iterations specified by member
 * functions.
 *
 * \tparam Algorithm Type of the algorithm.
 * \tparam Value Type of values.
 * \tparam Function Type of the function.
 *
 * \thread_safety Not thread-safe.
 */
template <typename Algorithm,
    concepts::formattable_iteration_parameter_value Value,
    concepts::member_getter_of<Value, Algorithm> Function>
class member_function_iteration_parameter_value {
public:
    //! Type returned by the function.
    using return_type = std::invoke_result_t<Function, Algorithm*>;

    /*!
     * \brief Constructor.
     *
     * \param[in] function Pointer to the member function.
     */
    explicit member_function_iteration_parameter_value(Function function)
        : function_(function) {}

    /*!
     * \brief Get the current value.
     *
     * \return Value.
     *
     * \warning This function always throws an exception.
     */
    [[nodiscard]] auto get() const -> return_type {
        throw invalid_argument(
            "Evaluation of this parameter value requires the pointer to the "
            "algorithm.");
    }

    /*!
     * \brief Get the current value.
     *
     * \param[in] algorithm Algorithm.
     * \return Value.
     */
    [[nodiscard]] auto get(const Algorithm* algorithm) const noexcept
        -> return_type {
        return (algorithm->*function_)();
    }

private:
    //! Function.
    Function function_;
};

}  // namespace num_collect::logging::iterations
