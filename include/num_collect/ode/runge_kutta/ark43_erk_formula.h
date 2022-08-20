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
 * \brief Definition of ark43_erk_formula class.
 */
#pragma once

#include <string_view>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/problem.h"  // IWYU pragma: keep
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/formula_base.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of ARK4(3)6L[2]SA-ERK formula in \cite Kennedy2003 .
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
class ark43_erk_formula
    : public formula_base<ark43_erk_formula<Problem>, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<ark43_erk_formula<Problem>, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    static_assert(!problem_type::allowed_evaluations.mass,
        "Mass matrix is not supported.");

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 6;

    //! Order of this formula.
    static constexpr index_type order = 4;

    //! Log tag.
    static constexpr auto log_tag = logging::log_tag_view(
        "num_collect::ode::runge_kutta::ark43_erk_formula");

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 3;

    /*!
     * \name Coefficients in Butcher array.
     *
     * - `a` is coefficients of intermidiate variables in calculation of
     *   intermidiate variables.
     * - `b` is coefficients of time in calculation of intermidiate variables.
     * - `c` is coefficients of intermidiate variables in calculation of
     *   estimates of next variables.
     */
    ///@{
    //! Coefficient in Butcher array.
    static constexpr scalar_type a21 = coeff(1, 2);
    static constexpr scalar_type a31 = coeff(13861, 62500);
    static constexpr scalar_type a32 = coeff(6889, 62500);
    static constexpr scalar_type a41 = coeff(-116923316275, 2393684061468);
    static constexpr scalar_type a42 = coeff(-2731218467317, 15368042101831);
    static constexpr scalar_type a43 = coeff(9408046702089, 11113171139209);
    static constexpr scalar_type a51 = coeff(-451086348788, 2902428689909);
    static constexpr scalar_type a52 = coeff(-2682348792572, 7519795681897);
    static constexpr scalar_type a53 = coeff(12662868775082, 11960479115383);
    static constexpr scalar_type a54 = coeff(3355817975965, 11060851509271);
    static constexpr scalar_type a61 = coeff(647845179188, 3216320057751);
    static constexpr scalar_type a62 = coeff(73281519250, 8382639484533);
    static constexpr scalar_type a63 = coeff(552539513391, 3454668386233);
    static constexpr scalar_type a64 = coeff(3354512671639, 8306763924573);
    static constexpr scalar_type a65 = coeff(4040, 17871);

    // b1 = 0
    static constexpr scalar_type b2 = coeff(1, 2);
    static constexpr scalar_type b3 = coeff(83, 250);
    static constexpr scalar_type b4 = coeff(31, 50);
    static constexpr scalar_type b5 = coeff(17, 20);
    static constexpr scalar_type b6 = coeff(1);

    static constexpr scalar_type c1 = coeff(82889, 524892);
    // c2 = 0
    static constexpr scalar_type c3 = coeff(15625, 83664);
    static constexpr scalar_type c4 = coeff(69875, 102672);
    static constexpr scalar_type c5 = coeff(-2260, 8211);
    static constexpr scalar_type c6 = coeff(1, 4);

    static constexpr scalar_type cw1 = coeff(4586570599, 29645900160);
    // cw2 = 0
    static constexpr scalar_type cw3 = coeff(178811875, 945068544);
    static constexpr scalar_type cw4 = coeff(814220225, 1159782912);
    static constexpr scalar_type cw5 = coeff(-3700637, 11593932);
    static constexpr scalar_type cw6 = coeff(61727, 225920);

    static constexpr scalar_type ce1 = c1 - cw1;
    // ce2 = 0
    static constexpr scalar_type ce3 = c3 - cw3;
    static constexpr scalar_type ce4 = c4 - cw4;
    static constexpr scalar_type ce5 = c5 - cw5;
    static constexpr scalar_type ce6 = c6 - cw6;
    ///@}

    //! \copydoc ode::formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        variable_type unused;
        step_embedded(time, step_size, current, estimate, unused);
    }

    /*!
     * \brief Compute the next variable and weak estimate of it with embedded
     * formula.
     *
     * \param[in] time Current time.
     * \param[in] step_size Step size.
     * \param[in] current Current variable.
     * \param[out] estimate Estimate of the next variable.
     * \param[out] error Estimate of error.
     */
    void step_embedded(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate,
        variable_type& error) {
        constexpr auto evaluations = evaluation_type{.diff_coeff = true};

        problem().evaluate_on(time, current, evaluations);
        k1_ = problem().diff_coeff();

        problem().evaluate_on(time + b2 * step_size,
            current + step_size * a21 * k1_, evaluations);
        k2_ = problem().diff_coeff();

        problem().evaluate_on(time + b3 * step_size,
            current + step_size * (a31 * k1_ + a32 * k2_), evaluations);
        k3_ = problem().diff_coeff();

        problem().evaluate_on(time + b4 * step_size,
            current + step_size * (a41 * k1_ + a42 * k2_ + a43 * k3_),
            evaluations);
        k4_ = problem().diff_coeff();

        problem().evaluate_on(time + b5 * step_size,
            current +
                step_size * (a51 * k1_ + a52 * k2_ + a53 * k3_ + a54 * k4_),
            evaluations);
        k5_ = problem().diff_coeff();

        problem().evaluate_on(time + b6 * step_size,
            current +
                step_size *
                    (a61 * k1_ + a62 * k2_ + a63 * k3_ + a64 * k4_ + a65 * k5_),
            evaluations);
        k6_ = problem().diff_coeff();

        estimate = current +
            step_size * (c1 * k1_ + c3 * k3_ + c4 * k4_ + c5 * k5_ + c6 * k6_);
        error = step_size *
            (ce1 * k1_ + ce3 * k3_ + ce4 * k4_ + ce5 * k5_ + ce6 * k6_);
    }

private:
    /*!
     * \name Intermediate variables.
     */
    ///@{
    //! Intermediate variable.
    variable_type k1_{};
    variable_type k2_{};
    variable_type k3_{};
    variable_type k4_{};
    variable_type k5_{};
    variable_type k6_{};
    ///@}
};

/*!
 * \brief Class of solver using ARK4(3)6L[2]SA-ERK formula in \cite Kennedy2003.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
using ark43_erk_solver = embedded_solver<ark43_erk_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
