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
 * \brief Definition of double_cubic_test_function class.
 */
#pragma once

#include <Eigen/Core>

namespace num_prob_collect::roots {

/*!
 * \brief Class of test function with two cubic functions.
 *
 * Solution is [3, 2].
 */
class double_cubic_test_function {
public:
    //! Type of variables.
    using variable_type = Eigen::Vector2d;

    //! Type of Jacobian matrices.
    using jacobian_type = Eigen::Matrix2d;

    /*!
     * \brief Evaluate on a variable.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(const variable_type& variable) {
        const variable_type cubic = variable.array().pow(3);
        const variable_type dif = 3.0 * variable.array().pow(2);  // NOLINT
        value_[0] = cubic[0] * 2.0 - cubic[1] - 46.0;             // NOLINT
        value_[1] = cubic[1] - 8.0;                               // NOLINT
        jacobian_(0, 0) = 2.0 * dif[0];                           // NOLINT
        jacobian_(0, 1) = -dif[1];                                // NOLINT
        jacobian_(1, 0) = dif[0];                                 // NOLINT
        jacobian_(1, 1) = 0.0;                                    // NOLINT
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
    //! Function value.
    variable_type value_{};

    //! Jacobian matrix.
    jacobian_type jacobian_{};
};

}  // namespace num_prob_collect::roots
