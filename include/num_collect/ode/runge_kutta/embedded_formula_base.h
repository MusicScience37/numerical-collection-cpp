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
 * \brief Definition of embedded_formula_base class.
 */
#pragma once

#include "num_collect/ode/runge_kutta/formula_base.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of embedded formulas in Runge-Kutta method.
 *
 * \tparam Derived Type of derived class.
 * \tparam Problem Type of problem.
 */
template <typename Derived, typename Problem>
class embedded_formula_base : public formula_base<Derived, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<Derived, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::derived;

public:
    /*!
     * \brief Compute the next variable and weak estimate of it with embedded
     * formula.
     *
     * \param[in] time Current time.
     * \param[in] step_size Step size.
     * \param[in] current Current variable.
     * \param[out] estimate Estimate of the next variable.
     * \param[out] weak_estimate Weak estimate of the next variable with less
     * order coefficients.
     */
    void step_embedded(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate,
        variable_type& weak_estimate) {
        derived().step(time, step_size, current, estimate, weak_estimate);
    }
};

}  // namespace num_collect::ode::runge_kutta
