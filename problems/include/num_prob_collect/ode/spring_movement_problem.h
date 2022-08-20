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
 * \brief Definition of spring_movement_problem class.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/ode/evaluation_type.h"

namespace num_prob_collect::ode {

/*!
 * \brief Class of test problem to spring movement.
 *
 * This solves the following equation of motion of a spring:
 * \f[
 *     \ddot{x} = -x
 * \f]
 *
 * In this class, the following equation is used to solve the above equation:
 * \f[
 *     \frac{d}{dt} \begin{pmatrix} \dot{x} \\ x \end{pmatrix}
 *     = \begin{pmatrix} -x \\ \dot{x} \end{pmatrix}
 * \f]
 */
class spring_movement_problem {
public:
    //! Type of variables.
    using variable_type = Eigen::Vector2d;

    //! Type of scalars.
    using scalar_type = double;

    //! Type of Jacobian.
    using jacobian_type = Eigen::Matrix2d;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{.diff_coeff = true, .jacobian = true};

    /*!
     * \brief Constructor.
     */
    spring_movement_problem() { jacobian_ << 0.0, -1.0, 1.0, 0.0; }

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(scalar_type /*time*/, const variable_type& variable,
        num_collect::ode::evaluation_type /*evaluations*/) {
        diff_coeff_ = jacobian_ * variable;
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
    //! Differential coefficient.
    variable_type diff_coeff_{};

    //! Jacobian.
    jacobian_type jacobian_{};
};

}  // namespace num_prob_collect::ode
