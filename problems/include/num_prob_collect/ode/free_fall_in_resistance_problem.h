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
 * \brief Definition of free_fall_in_resistance_problem class.
 */
#pragma once

#include <Eigen/Core>

namespace num_prob_collect::ode {

/*!
 * \brief Class of test problem of free fall in air resistance.
 *
 * This solves the following equation of motion:
 * \f[
 *     \ddot{x} = -k \dot{x} - g
 * \f]
 * Here, k and g are constants.
 *
 * In this class, the following equation is used to solve the above equation:
 * \f[
 *     \frac{d}{dt} \begin{pmatrix} \dot{x} \\ x \end{pmatrix}
 *     = \begin{pmatrix} -k \dot{x} - g \\ \dot{x} \end{pmatrix}
 * \f]
 *
 * When the initial variable is (0, 0),
 * the solution is the following:
 * \f[
 *     \begin{pmatrix} \dot{x} \\ x \end{pmatrix}
 *     = \begin{pmatrix}
 *         \frac{g}{k}e^{-kt}    - \frac{g}{k} \\
 *         -\frac{g}{k^2}e^{-kt} - \frac{g}{k}t + \frac{g}{k^2}
 *     \end{pmatrix}
 * \f]
 */
class free_fall_in_resistance_problem {
public:
    //! Type of variables.
    using variable_type = Eigen::Vector2d;

    //! Type of scalars.
    using scalar_type = double;

    //! Type of Jacobian.
    using jacobian_type = Eigen::Matrix2d;

    /*!
     * \brief Construct.
     *
     * \param[in] k Coefficient of resistance.
     * \param[in] g Gravity.
     */
    free_fall_in_resistance_problem(scalar_type k, scalar_type g)
        : k_(k), g_(g) {
        jacobian_ << -k_, 0.0, 1.0, 0.0;
    }

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(scalar_type /*time*/, const variable_type& variable,
        bool /*needs_jacobian*/ = false) {
        diff_coeff_[0] = -k_ * variable[0] - g_;
        diff_coeff_[1] = variable[0];
    }

    /*!
     * \brief Get the differential coefficient.
     *
     * \return Differential coefficient.
     */
    [[nodiscard]] auto diff_coeff() const noexcept -> const variable_type& {
        return diff_coeff_;
    }

    /*!
     * \brief Get the Jacobian.
     *
     * \return Jacobian.
     */
    [[nodiscard]] auto jacobian() const noexcept -> const jacobian_type& {
        return jacobian_;
    }

private:
    //! Coefficient of resistance.
    scalar_type k_;

    //! Gravity.
    scalar_type g_;

    //! Differential coefficient.
    variable_type diff_coeff_{};

    //! Jacobian.
    jacobian_type jacobian_{};
};

}  // namespace num_prob_collect::ode
