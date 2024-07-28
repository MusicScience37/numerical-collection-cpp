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
 * \brief Definition of symplectic_forest4_formula class.
 */
#pragma once

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/cbrt.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/ode/concepts/multi_variate_problem.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/formula_base.h"
#include "num_collect/ode/simple_solver.h"

namespace num_collect::ode::symplectic {

/*!
 * \brief Class of fourth-order symplectic integration formula in
 * \cite Forest1990.
 *
 * This formula solves initial value problems of ODEs with following structure:
 *
 * \f[
 *     \frac{d}{dt}
 *     \begin{pmatrix} \boldsymbol{p} \\ \boldsymbol{q} \end{pmatrix}
 *     =
 *     \begin{pmatrix}
 *         -\frac{\partial V}{\partial \boldsymbol{q}} \\
 *         \frac{\partial T}{\partial \boldsymbol{p}}
 *     \end{pmatrix}
 * \f]
 *
 * This formula assumes that
 * the lower half of the solution vector is position \f$\boldsymbol{q}\f$,
 * and the upper half is moment \f$\boldsymbol{p}\f$.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::multi_variate_problem Problem>
class symplectic_forest4_formula
    : public formula_base<symplectic_forest4_formula<Problem>, Problem> {
public:
    //! Type of base class.
    using base_type =
        formula_base<symplectic_forest4_formula<Problem>, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::problem;

    static_assert(!problem_type::allowed_evaluations.mass,
        "Mass matrix is not supported.");

protected:
    using base_type::coeff;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 7;

    //! Order of this formula.
    static constexpr index_type order = 4;

    //! Log tag.
    static constexpr auto log_tag = logging::log_tag_view(
        "num_collect::ode::symplectic::symplectic_forest4_formula");

    //! A constant of this formula \f$\alpha = 1 - 2^{1/3}\f$.
    static constexpr scalar_type alpha = static_cast<scalar_type>(1) -
        constants::cbrt(static_cast<scalar_type>(2));

    /*!
     * \name Coefficients of this formula.
     *
     * - `bp` is coefficients to update moment.
     * - `bq` is coefficients to update position.
     */
    ///@{
    //! Coefficients of this formula.
    static constexpr scalar_type bp1 = static_cast<scalar_type>(1) /
        (static_cast<scalar_type>(2) * (static_cast<scalar_type>(1) + alpha));
    static constexpr scalar_type bq1 =
        static_cast<scalar_type>(1) / (static_cast<scalar_type>(1) + alpha);
    static constexpr scalar_type bp2 = alpha /
        (static_cast<scalar_type>(2) * (static_cast<scalar_type>(1) + alpha));
    static constexpr scalar_type bq2 = (alpha - static_cast<scalar_type>(1)) /
        (static_cast<scalar_type>(1) + alpha);
    static constexpr scalar_type bp3 = bp2;
    static constexpr scalar_type bq3 = bq1;
    static constexpr scalar_type bp4 = bp1;
    ///@}

    //! \copydoc ode::formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        const index_type dim = current.size();
        const index_type half_dim = dim / 2;
        if (dim % 2 == 1) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "This formula requires vectors with even dimensions.");
        }

        estimate = current;

        constexpr auto evaluations = evaluation_type{.diff_coeff = true};

        problem().evaluate_on(time, estimate, evaluations);
        estimate.head(half_dim) +=
            step_size * bp1 * problem().diff_coeff().head(half_dim);

        problem().evaluate_on(time, estimate, evaluations);
        estimate.tail(half_dim) +=
            step_size * bq1 * problem().diff_coeff().tail(half_dim);

        problem().evaluate_on(time, estimate, evaluations);
        estimate.head(half_dim) +=
            step_size * bp2 * problem().diff_coeff().head(half_dim);

        problem().evaluate_on(time, estimate, evaluations);
        estimate.tail(half_dim) +=
            step_size * bq2 * problem().diff_coeff().tail(half_dim);

        problem().evaluate_on(time, estimate, evaluations);
        estimate.head(half_dim) +=
            step_size * bp3 * problem().diff_coeff().head(half_dim);

        problem().evaluate_on(time, estimate, evaluations);
        estimate.tail(half_dim) +=
            step_size * bq3 * problem().diff_coeff().tail(half_dim);

        problem().evaluate_on(time, estimate, evaluations);
        estimate.head(half_dim) +=
            step_size * bp4 * problem().diff_coeff().head(half_dim);
    }
};

/*!
 * \brief Class of fourth-order symplectic integration formula in
 * \cite Forest1990.
 *
 * This formula solves initial value problems of ODEs with following structure:
 *
 * \f[
 *     \frac{d}{dt}
 *     \begin{pmatrix} \boldsymbol{p} \\ \boldsymbol{q} \end{pmatrix}
 *     =
 *     \begin{pmatrix}
 *         -\frac{\partial V}{\partial \boldsymbol{q}} \\
 *         \frac{\partial T}{\partial \boldsymbol{p}}
 *     \end{pmatrix}
 * \f]
 *
 * This formula assumes that
 * the lower half of the solution vector is position \f$\boldsymbol{q}\f$,
 * and the upper half is moment \f$\boldsymbol{p}\f$.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using symplectic_forest4_solver =
    simple_solver<symplectic_forest4_formula<Problem>>;

}  // namespace num_collect::ode::symplectic
