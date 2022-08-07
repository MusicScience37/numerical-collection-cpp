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
 * \brief Definition of rodasp_formula class.
 */
#pragma once

#include <string_view>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/rosenbrock_equation_solver.h"  // IWYU pragma: keep
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/formula_base.h"
#include "num_collect/ode/rosenbrock/default_rosenbrock_equation_solver.h"

namespace num_collect::ode::rosenbrock {

/*!
 * \brief Class of RODASP formula for Rosenbrock method.
 *
 * \note For coefficients, refer to \cite Blom2016.
 *
 * \tparam Problem Type of problem.
 * \tparam EquationSolver Type of class to solve equations in Rosenbrock
 * methods.
 */
template <concepts::problem Problem,
    concepts::rosenbrock_equation_solver EquationSolver =
        default_rosenbrock_equation_solver_t<Problem>>
class rodasp_formula
    : public formula_base<rodasp_formula<Problem, EquationSolver>, Problem> {
public:
    //! Type of base class.
    using base_type =
        formula_base<rodasp_formula<Problem, EquationSolver>, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    //! Type of class to solve equations in Rosenbrock methods.
    using equation_solver_type = EquationSolver;

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 6;

    //! Order of this formula.
    static constexpr index_type order = 4;

    // TODO: lesser_order couldn't be found.

    //! Log tag.
    static constexpr auto log_tag =
        logging::log_tag_view("num_collect::ode::rosenbrock::rodasp_formula");

    /*!
     * \name Coefficients in Rosenbrock method.
     *
     * - `a` is coefficients of intermidiate variables in calculation of
     *   intermidiate variables.
     * - `b` is coefficients of time in calculation of intermidiate variables.
     * - `c` is coefficients of intermidiate variables in calculation of
     *   estimates of next variables.
     * - `g` is coefficients of intermidiate variables in calculation of
     *   intermidiate variables.
     */
    ///@{
    //! Coefficient in Rosenbrock method.
    static constexpr scalar_type a21 = coeff(7.5e-1);
    static constexpr scalar_type a31 = coeff(8.6120400814e-2);
    static constexpr scalar_type a32 = coeff(1.2387959919e-1);
    static constexpr scalar_type a41 = coeff(7.7403453551e-1);
    static constexpr scalar_type a42 = coeff(1.4926515495e-1);
    static constexpr scalar_type a43 = coeff(-2.9419969046e-1);
    static constexpr scalar_type a51 = coeff(5.3087466826);
    static constexpr scalar_type a52 = coeff(1.3308921400);
    static constexpr scalar_type a53 = coeff(-5.3741378117);
    static constexpr scalar_type a54 = coeff(-2.6550101103e-1);
    static constexpr scalar_type a61 = coeff(-1.7644376488);
    static constexpr scalar_type a62 = coeff(-4.7475655721e-1);
    static constexpr scalar_type a63 = coeff(2.3696918469);
    static constexpr scalar_type a64 = coeff(6.1950235906e-1);
    static constexpr scalar_type a65 = coeff(2.5e-1);

    static constexpr scalar_type b1 = coeff(0);
    static constexpr scalar_type b2 = a21;
    static constexpr scalar_type b3 = a31 + a32;
    static constexpr scalar_type b4 = a41 + a42 + a43;
    static constexpr scalar_type b5 = a51 + a52 + a53 + a54;
    static constexpr scalar_type b6 = a61 + a62 + a63 + a64 + a65;

    static constexpr scalar_type g21 = coeff(-7.5e-1);
    static constexpr scalar_type g31 = coeff(-1.3551200000e-1);
    static constexpr scalar_type g32 = coeff(-1.3799200000e-1);
    static constexpr scalar_type g41 = coeff(-1.2560800000);
    static constexpr scalar_type g42 = coeff(-2.5014500000e-1);
    static constexpr scalar_type g43 = coeff(1.2209300000);
    static constexpr scalar_type g51 = coeff(-7.0731800000);
    static constexpr scalar_type g52 = coeff(-1.8056500000);
    static constexpr scalar_type g53 = coeff(7.7438300000);
    static constexpr scalar_type g54 = coeff(8.8500300000e-1);
    static constexpr scalar_type g61 = coeff(1.6840700000);
    static constexpr scalar_type g62 = coeff(4.1826600000e-1);
    static constexpr scalar_type g63 = coeff(-1.8814100000);
    static constexpr scalar_type g64 = coeff(-1.1378600000e-1);
    static constexpr scalar_type g65 = coeff(-3.5714300000e-1);
    static constexpr scalar_type g = coeff(2.5e-1);

    static constexpr scalar_type c1 = coeff(-8.0368370789e-2);
    static constexpr scalar_type c2 = coeff(-5.6490613592e-2);
    static constexpr scalar_type c3 = coeff(4.8828563004e-1);
    static constexpr scalar_type c4 = coeff(5.0571621148e-1);
    static constexpr scalar_type c5 = coeff(-1.0714285714e-1);
    static constexpr scalar_type c6 = coeff(2.5e-1);

    static constexpr scalar_type cw1 = coeff(-1.7644376488);
    static constexpr scalar_type cw2 = coeff(-4.7475655721e-1);
    static constexpr scalar_type cw3 = coeff(2.3696918469);
    static constexpr scalar_type cw4 = coeff(6.1950235906e-1);
    static constexpr scalar_type cw5 = coeff(2.5e-1);

    static constexpr scalar_type ce1 = c1 - cw1;
    static constexpr scalar_type ce2 = c2 - cw2;
    static constexpr scalar_type ce3 = c3 - cw3;
    static constexpr scalar_type ce4 = c4 - cw4;
    static constexpr scalar_type ce5 = c5 - cw5;
    static constexpr scalar_type ce6 = c6;
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
        solver_.update_jacobian(problem(), time, step_size, current);

        solver_.solve(problem().diff_coeff(), k1_);

        problem().evaluate_on(time + b2 * step_size,
            current + step_size * (a21 * k1_),
            evaluation_type{.diff_coeff = true});
        solver_.solve(problem().diff_coeff() +
                step_size * solver_.jacobian() * (g21 * k1_),
            k2_);

        problem().evaluate_on(time + b3 * step_size,
            current + step_size * (a31 * k1_ + a32 * k2_),
            evaluation_type{.diff_coeff = true});
        solver_.solve(problem().diff_coeff() +
                step_size * solver_.jacobian() * (g31 * k1_ + g32 * k2_),
            k3_);

        problem().evaluate_on(time + b4 * step_size,
            current + step_size * (a41 * k1_ + a42 * k2_ + a43 * k3_),
            evaluation_type{.diff_coeff = true});
        solver_.solve(problem().diff_coeff() +
                step_size * solver_.jacobian() *
                    (g41 * k1_ + g42 * k2_ + g43 * k3_),
            k4_);

        problem().evaluate_on(time + b5 * step_size,
            current +
                step_size * (a51 * k1_ + a52 * k2_ + a53 * k3_ + a54 * k4_),
            evaluation_type{.diff_coeff = true});
        solver_.solve(problem().diff_coeff() +
                step_size * solver_.jacobian() *
                    (g51 * k1_ + g52 * k2_ + g53 * k3_ + g54 * k4_),
            k5_);

        problem().evaluate_on(time + b6 * step_size,
            current +
                step_size *
                    (a61 * k1_ + a62 * k2_ + a63 * k3_ + a64 * k4_ + a65 * k5_),
            evaluation_type{.diff_coeff = true});
        solver_.solve(problem().diff_coeff() +
                step_size * solver_.jacobian() *
                    (g61 * k1_ + g62 * k2_ + g63 * k3_ + g64 * k4_ + g65 * k5_),
            k6_);

        estimate = current +
            step_size *
                (c1 * k1_ + c2 * k2_ + c3 * k3_ + c4 * k4_ + c5 * k5_ +
                    c6 * k6_);
        error = step_size *
            (ce1 * k1_ + ce2 * k2_ + ce3 * k3_ + ce4 * k4_ + ce5 * k5_ +
                ce6 * k6_);
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

    //! Solver.
    equation_solver_type solver_{g};
};

/*!
 * \brief Class of solver using RODASP formula.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using rodasp_solver = embedded_solver<rodasp_formula<Problem>>;

}  // namespace num_collect::ode::rosenbrock
