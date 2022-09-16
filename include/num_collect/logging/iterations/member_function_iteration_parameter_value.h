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

#include "num_collect/base/concepts/decayed_to.h"
#include "num_collect/base/exception.h"
#include "num_collect/logging/concepts/formattable_iteration_parameter_value.h"  // IWYU pragma: keep

namespace num_collect::logging::iterations {

/*!
 * \brief Class of parameters values in iterations specified by member
 * functions.
 *
 * \tparam Algorithm Type of the algorithm.
 * \tparam Value Type of values.
 * \tparam ReturnType Type of returned value.
 *
 * \thread_safety Not thread-safe.
 */
template <typename Algorithm,
    concepts::formattable_iteration_parameter_value Value,
    base::concepts::decayed_to<Value> ReturnType>
class member_function_iteration_parameter_value {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] func Pointer to the member function.
     */
    explicit member_function_iteration_parameter_value(
        ReturnType (Algorithm::*func)() const)
        : func_(func) {}

    /*!
     * \brief Get the current value.
     *
     * \return Value.
     *
     * \warning This function always throws an exception.
     */
    [[nodiscard]] auto get() const -> ReturnType {
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
        -> ReturnType {
        return (algorithm->*func_)();
    }

private:
    //! Pointer to the member function.
    ReturnType (Algorithm::*func_)() const;
};

}  // namespace num_collect::logging::iterations
