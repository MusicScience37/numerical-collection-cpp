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
 * \brief Definition of changing_mass_exponential_problem class.
 */
#pragma once

#include <cmath>

#include "num_collect/ode/concepts/differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/mass_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/evaluation_type.h"

namespace num_prob_collect::ode {

/*!
 * \brief Class of test problem to calculate exponential function.
 */
class changing_mass_exponential_problem {
public:
    //! Type of variables.
    using variable_type = double;

    //! Type of scalars.
    using scalar_type = double;

    //! Type of Jacobian.
    using jacobian_type = double;

    //! Type of mass.
    using mass_type = double;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{
            .diff_coeff = true, .jacobian = true, .mass = true};

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(double /*time*/, double variable,
        num_collect::ode::evaluation_type /*evaluations*/) {
        const double exp_x = std::exp(variable);
        diff_coeff_ = variable * exp_x;
        jacobian_ = exp_x + diff_coeff_;
        mass_ = exp_x;
    }

    /*!
     * \brief Get the differential coefficient.
     *
     * \return Differential coefficient.
     */
    [[nodiscard]] auto diff_coeff() const -> const double& {
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
    double diff_coeff_{};

    //! Mass.
    double mass_{};

    //! Jacobian.
    double jacobian_{};
};

static_assert(num_collect::ode::concepts::differentiable_problem<
    changing_mass_exponential_problem>);
static_assert(num_collect::ode::concepts::mass_problem<
    changing_mass_exponential_problem>);

}  // namespace num_prob_collect::ode
