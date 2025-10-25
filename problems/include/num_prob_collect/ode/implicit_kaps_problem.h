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
 * \brief Definition of implicit_kaps_problem class.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/multi_variate_differentiable_problem.h"
#include "num_collect/ode/evaluation_type.h"

namespace num_prob_collect::ode {

/*!
 * \brief Class of Kaps' problem \cite Kennedy2003.
 *
 * This ODE is as following:
 *
 * \f{aligned}{
 *     \varepsilon \dot{y_1} & = -(1 + 2 \varepsilon) y_1 + y_2^2 \\
 *     \dot{y_2} &= y_1 - y_2 - y_2^2
 * \f}
 *
 * For \f$ 0 \le t \le 1 \f$, exact solution is as following (for all
 * \f$\varepsilon\f$):
 *
 * \f{aligned}{
 *     y_1 &= \exp(-2t) \\
 *     y_2 &= \exp(-t) \\
 * \f}
 *
 * This problem is stiff for small \f$\varepsilon\f$.
 */
class implicit_kaps_problem {
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
     *
     * \param[in] epsilon Parameter in the equation.
     */
    explicit implicit_kaps_problem(double epsilon)
        : epsilon_(epsilon), mass_({{epsilon_, 0.0}, {0.0, 1.0}}) {}

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] variable Variable.
     * \param[in] evaluations Evaluation types.
     */
    void evaluate_on(double /*time*/, const variable_type& variable,
        num_collect::ode::evaluation_type evaluations) {
        diff_coeff_(0) =
            -(1.0 + 2.0 * epsilon_) * variable(0) + variable(1) * variable(1);
        diff_coeff_(1) = variable(0) - variable(1) - variable(1) * variable(1);

        if (evaluations.jacobian) {
            jacobian_(0, 0) = -(1.0 + 2.0 * epsilon_);
            jacobian_(0, 1) = 2.0 * variable(1);
            jacobian_(1, 0) = 1.0;
            jacobian_(1, 1) = -1.0 - 2.0 * variable(1);
        }
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
    //! Parameter.
    double epsilon_;

    //! Differential coefficient.
    variable_type diff_coeff_{};

    //! Jacobian.
    jacobian_type jacobian_{};

    //! Mass.
    mass_type mass_{};
};

static_assert(num_collect::ode::concepts::multi_variate_differentiable_problem<
    implicit_kaps_problem>);
static_assert(num_collect::ode::concepts::mass_problem<implicit_kaps_problem>);

}  // namespace num_prob_collect::ode
