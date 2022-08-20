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
 * \brief Definition of external_external_exponential_problem class.
 */
#pragma once

#include <cmath>

#include "num_collect/ode/concepts/time_differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/evaluation_type.h"

namespace num_prob_collect::ode {

/*!
 * \brief Class of test problem to calculate exponential function using external
 * time-dependent term..
 */
class external_exponential_problem {
public:
    //! Type of variables.
    using variable_type = double;

    //! Type of scalars.
    using scalar_type = double;

    //! Type of Jacobian.
    using jacobian_type = double;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{
            .diff_coeff = true, .jacobian = true, .time_derivative = true};

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] time Time.
     */
    void evaluate_on(double time, double /*variable*/,
        num_collect::ode::evaluation_type /*evaluations*/) {
        diff_coeff_ = std::exp(time);
        time_derivative_ = diff_coeff_;
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
     * \brief Get the partial derivative with respect to time.
     *
     * \return Derivative.
     */
    [[nodiscard]] auto time_derivative() const noexcept
        -> const variable_type& {
        return time_derivative_;
    }

private:
    //! Differential coefficient.
    double diff_coeff_{};

    //! Jacobian.
    double jacobian_{1.0};

    //! Partial derivative with respect to time.
    double time_derivative_{};
};

static_assert(num_collect::ode::concepts::time_differentiable_problem<
    external_exponential_problem>);

}  // namespace num_prob_collect::ode
