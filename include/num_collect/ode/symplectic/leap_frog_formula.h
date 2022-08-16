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
 * \brief Definition of leap_frog_formula class.
 */
#pragma once

#include <string_view>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/half.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/multi_variate_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/formula_base.h"
#include "num_collect/ode/simple_solver.h"

namespace num_collect::ode::symplectic {

/*!
 * \brief Class of leap-frog formula.
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
 * \note For theoretical discussion, see \cite Forest1990.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::multi_variate_problem Problem>
class leap_frog_formula
    : public formula_base<leap_frog_formula<Problem>, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<leap_frog_formula<Problem>, Problem>;

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
    static constexpr index_type stages = 3;

    //! Order of this formula.
    static constexpr index_type order = 2;

    //! Log tag.
    static constexpr auto log_tag = logging::log_tag_view(
        "num_collect::ode::symplectic::leap_frog_formula");

    //! \copydoc ode::formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        const index_type dim = current.size();
        const index_type half_dim = dim / 2;
        if (dim % 2 == 1) {
            throw invalid_argument(
                "This formula requires vectors with even dimensions.");
        }

        estimate = current;

        constexpr auto evaluations = evaluation_type{.diff_coeff = true};

        problem().evaluate_on(time, estimate, evaluations);
        estimate.head(half_dim) += step_size * constants::half<scalar_type> *
            problem().diff_coeff().head(half_dim);

        problem().evaluate_on(time, estimate, evaluations);
        estimate.tail(half_dim) +=
            step_size * problem().diff_coeff().tail(half_dim);

        problem().evaluate_on(time, estimate, evaluations);
        estimate.head(half_dim) += step_size * constants::half<scalar_type> *
            problem().diff_coeff().head(half_dim);
    }
};

/*!
 * \brief Class of solver using leap-frog formula.
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
 * \note For theoretical discussion, see \cite Forest1990.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using leap_frog_solver = simple_solver<leap_frog_formula<Problem>>;

}  // namespace num_collect::ode::symplectic
