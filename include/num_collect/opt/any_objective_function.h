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
 * \brief Definition of any_objective_function class.
 */
#pragma once

#include <functional>
#include <type_traits>
#include <utility>

#include "num_collect/base/concepts/invocable_as.h"

namespace num_collect::opt {

/*!
 * \brief Class to store any type of objects of objective functions.
 *
 * \tparam Signature Signature of the objective function.
 */
template <typename Signature>
class any_objective_function;

/*!
 * \brief Class to store any type of objects of objective functions.
 *
 * \tparam Value Type of function values.
 * \tparam Variable Type of variables.
 */
template <typename Value, typename Variable>
class any_objective_function<Value(Variable)> {
public:
    //! Type of variables.
    using variable_type = std::decay_t<Variable>;

    //! Type of function values.
    using value_type = std::decay_t<Value>;

    /*!
     * \brief Constructor.
     *
     * \note This constructor creates an empty object.
     */
    any_objective_function() = default;

    /*!
     * \brief Constructor.
     *
     * \param[in] function Function object of the objective function.
     */
    template <base::concepts::invocable_as<Value(Variable)> Function>
    any_objective_function(  // NOLINT(*-explicit-constructor,*-explicit-conversions)
        Function&& function)
        : function_(std::forward<Function>(function)) {}

    /*!
     * \brief Assign a function object.
     *
     * \param[in] function Function object of the objective function.
     * \return This object.
     */
    template <base::concepts::invocable_as<Value(Variable)> Function>
    auto operator=(  // NOLINT(*-explicit-constructor,*-explicit-conversions)
        Function&& function) -> any_objective_function& {
        function_ = std::forward<Function>(function);
        return *this;
    }

    /*!
     * \brief Evaluate function value on a variable.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(const variable_type& variable) {
        value_ = function_(variable);
    }

    /*!
     * \brief Get function value.
     *
     * \return Function value.
     */
    [[nodiscard]] auto value() const noexcept -> const value_type& {
        return value_;
    }

private:
    //! Function object of the objective function.
    std::function<Value(Variable)> function_{};

    //! Function value.
    value_type value_{};
};

}  // namespace num_collect::opt
