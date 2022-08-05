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
 * \brief Definition of cubic_root_test_function class.
 */
#pragma once

namespace num_prob_collect::roots {

/*!
 * \brief Class of test function to calculate cubic root of a number.
 */
class cubic_root_test_function {
public:
    //! Type of variables.
    using variable_type = double;

    //! Type of Jacobian.
    using jacobian_type = double;

    /*!
     * \brief Constructor.
     *
     * \param[in] target Value to calculate cubic root of.
     */
    explicit cubic_root_test_function(variable_type target) : target_(target) {}

    /*!
     * \brief Evaluate on a variable.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(const variable_type& variable) {
        value_ = variable * variable * variable - target_;
        jacobian_ = 3.0 * variable * variable;  // NOLINT
    }

    /*!
     * \brief Get function value.
     *
     * \return Function value.
     */
    [[nodiscard]] auto value() const noexcept -> const variable_type& {
        return value_;
    }

    /*!
     * \brief Get Jacobian matrix.
     *
     * \return Jacobian matrix.
     */
    [[nodiscard]] auto jacobian() const noexcept -> const jacobian_type& {
        return jacobian_;
    }

private:
    //! Value to calculate cubic root of.
    variable_type target_;

    //! Function value.
    variable_type value_{};

    //! Jacobian matrix.
    jacobian_type jacobian_{};
};

}  // namespace num_prob_collect::roots
