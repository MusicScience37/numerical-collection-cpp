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
 * \brief Definition of function_object_wrapper class.
 */
#pragma once

#include <type_traits>
#include <utility>

namespace num_collect::opt {

/*!
 * \brief Wrapper class of a function object to use as an objective function.
 *
 * \see make_function_object_wrapper
 *
 * \tparam Signature Signature of the function.
 * \tparam Function Type of the function.
 */
template <typename Signature, typename Function>
class function_object_wrapper;

/*!
 * \brief Wrapper class of a function object to use as an objective function.
 *
 * \see make_function_object_wrapper
 *
 * \tparam Value Type of function values.
 * \tparam Variable Type of variables.
 * \tparam Function Type of the function.
 */
template <typename Value, typename Variable, typename Function>
class function_object_wrapper<Value(Variable), Function> {
public:
    //! Type of variables.
    using variable_type = std::decay_t<Variable>;

    //! Type of function values.
    using value_type = std::decay_t<Value>;

    //! Type of the function.
    using function_type = std::decay_t<Function>;

    /*!
     * \brief Construct.
     *
     * \param[in] function Function.
     */
    explicit function_object_wrapper(function_type function)
        : function_(std::move(function)) {}

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
    [[nodiscard]] auto value() const -> value_type { return value_; }

private:
    //! Function.
    function_type function_;

    //! Function value.
    value_type value_{};
};

/*!
 * \brief Create function_object_wrapper object.
 *
 * \tparam Signature Signature of the function.
 * \tparam Function Type of the function.
 * \param[in] function Function.
 * \return function_object_wrapper object.
 */
template <typename Signature, typename Function>
[[nodiscard]] inline auto make_function_object_wrapper(Function&& function)
    -> function_object_wrapper<Signature, Function> {
    return function_object_wrapper<Signature, Function>(
        std::forward<Function>(function));
}

}  // namespace num_collect::opt
