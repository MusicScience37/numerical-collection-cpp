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
 * \brief Definition of vibrated_quadratic_function class.
 */
#pragma once

#include <cmath>

namespace num_prob_collect::opt {

/*!
 * \brief Quadratic function with vibration for test of optimization.
 */
class vibrated_quadratic_function {
public:
    //! Type of variables.
    using variable_type = double;
    //! Type of function values.
    using value_type = double;
    //! Type of Hessian.
    using hessian_type = double;

    /*!
     * \brief Evaluate function value on variable.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(double variable) {
        value_ = coeff_ * variable * variable - std::cos(variable);
        constexpr double two = 2.0;
        grad_ = two * coeff_ * variable + std::sin(variable);
        hessian_ = two * coeff_ + std::cos(variable);
    }

    /*!
     * \brief Get function value.
     *
     * \return Function value.
     */
    [[nodiscard]] auto value() const -> double { return value_; }

    /*!
     * \brief Get gradient.
     *
     * \return Gradient.
     */
    [[nodiscard]] auto gradient() const -> double { return grad_; }

    /*!
     * \brief Get Hessian.
     *
     * \return Hessian.
     */
    [[nodiscard]] auto hessian() const -> double { return hessian_; }

private:
    //! Coefficient.
    double coeff_{0.25};  // NOLINT

    //! Function value.
    double value_{0.0};

    //! Gradient.
    double grad_{};

    //! Hessian.
    double hessian_{};
};

}  // namespace num_prob_collect::opt
