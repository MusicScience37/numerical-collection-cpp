/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of changing_mass_quadratic_problem class.
 */
#pragma once

#include <cmath>

#include <Eigen/Core>

#include "num_collect/ode/evaluation_type.h"

namespace num_prob_collect::ode {

/*!
 * \brief Class of a test problem.
 *
 * This solves the following equation of motion:
 * \f[
 *     e^{x} \ddot{x} = e^{x}
 * \f]
 *
 * In this class, the following equation is used to solve the above equation:
 * \f[
 *     \begin{pmatrix} e^{x} & 0 \\ 0 & 1 \end{pmatrix}
 *     \frac{d}{dt} \begin{pmatrix} \dot{x} \\ x \end{pmatrix}
 *     = \begin{pmatrix} e^{x} \\ \dot{x} \end{pmatrix}
 * \f]
 *
 * When the initial variable is (0, 0),
 * the solution is the following:
 * \f[
 *     \begin{pmatrix} \dot{x} \\ x \end{pmatrix}
 *     = \begin{pmatrix}
 *         t \\
 *         \frac{1}{2} t^2
 *     \end{pmatrix}
 * \f]
 */
class changing_mass_quadratic_problem {
public:
    //! Type of variables.
    using variable_type = Eigen::Vector2d;

    //! Type of scalars.
    using scalar_type = double;

    //! Type of Jacobian.
    using jacobian_type = Eigen::Matrix2d;

    //! Type of mass.
    using mass_type = Eigen::Matrix2d;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{
            .diff_coeff = true, .jacobian = true, .mass = true};

    /*!
     * \brief Constructor.
     */
    changing_mass_quadratic_problem() {
        jacobian_(1, 0) = 1.0;
        mass_(1, 1) = 1.0;
    }

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] variable Variable.
     * \param[in] evaluations Evaluations.
     */
    void evaluate_on(scalar_type /*time*/, const variable_type& variable,
        num_collect::ode::evaluation_type evaluations) {
        const double exp_x = std::exp(variable(1));
        diff_coeff_(0) = exp_x;
        diff_coeff_(1) = variable(0);
        if (evaluations.jacobian) {
            jacobian_(0, 1) = exp_x;
        }
        mass_(0, 0) = exp_x;
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

    /*!
     * \brief Get the mass.
     *
     * \return Mass.
     */
    [[nodiscard]] auto mass() const noexcept -> const mass_type& {
        return mass_;
    }

private:
    //! Differential coefficient.
    variable_type diff_coeff_{};

    //! Jacobian.
    jacobian_type jacobian_{jacobian_type::Zero()};

    //! Mass.
    mass_type mass_{mass_type::Zero()};
};

}  // namespace num_prob_collect::ode
