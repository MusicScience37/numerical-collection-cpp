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
 * \note For coefficients, I referred to \cite Steinebach2022.
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

    /*!
     * \brief Order of lesser coefficients of this formula.
     *
     * \warning Exact information has not been found.
     */
    static constexpr index_type lesser_order = 3;

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
    static constexpr scalar_type a21 = coeff(0.75);
    static constexpr scalar_type a31 = coeff(8.6120400814152190e-2);
    static constexpr scalar_type a32 = coeff(0.1238795991858478);
    static constexpr scalar_type a41 = coeff(0.7749345355073236);
    static constexpr scalar_type a42 = coeff(0.1492651549508680);
    static constexpr scalar_type a43 = coeff(-0.2941996904581916);
    static constexpr scalar_type a51 = coeff(5.308746682646142);
    static constexpr scalar_type a52 = coeff(1.330892140037269);
    static constexpr scalar_type a53 = coeff(-5.374137811655562);
    static constexpr scalar_type a54 = coeff(-0.2655010110278497);
    static constexpr scalar_type a61 = coeff(-1.764437648774483);
    static constexpr scalar_type a62 = coeff(-0.4747565572063027);
    static constexpr scalar_type a63 = coeff(2.369691846915802);
    static constexpr scalar_type a64 = coeff(0.6195023590649829);
    static constexpr scalar_type a65 = coeff(0.25);

    static constexpr scalar_type b1 = coeff(0);
    static constexpr scalar_type b2 = a21;
    static constexpr scalar_type b3 = a31 + a32;
    static constexpr scalar_type b4 = a41 + a42 + a43;
    static constexpr scalar_type b5 = a51 + a52 + a53 + a54;
    static constexpr scalar_type b6 = a61 + a62 + a63 + a64 + a65;

    static constexpr scalar_type g21 = coeff(-0.75);
    static constexpr scalar_type g31 = coeff(-0.1355124008141522);
    static constexpr scalar_type g32 = coeff(-0.1379915991858478);
    static constexpr scalar_type g41 = coeff(-1.2569840048950798);
    static constexpr scalar_type g42 = coeff(-0.2501447105064236);
    static constexpr scalar_type g43 = coeff(1.2209287154015032);
    static constexpr scalar_type g51 = coeff(-7.073184331420625);
    static constexpr scalar_type g52 = coeff(-1.805648697243572);
    static constexpr scalar_type g53 = coeff(7.7438296585713635);
    static constexpr scalar_type g54 = coeff(0.8850033700928326);
    static constexpr scalar_type g61 = coeff(1.6840692779853665);
    static constexpr scalar_type g62 = coeff(0.41826594361385516);
    static constexpr scalar_type g63 = coeff(-1.8814062168730028);
    static constexpr scalar_type g64 = coeff(-0.11378614758336392);
    static constexpr scalar_type g65 = coeff(-0.3571428571428569);
    static constexpr scalar_type g = coeff(0.25);

    static constexpr scalar_type g1 = g;
    static constexpr scalar_type g2 = g21 + g;
    static constexpr scalar_type g3 = g31 + g32 + g;
    static constexpr scalar_type g4 = g41 + g42 + g43 + g;
    static constexpr scalar_type g5 = g51 + g52 + g53 + g54 + g;
    static constexpr scalar_type g6 = g61 + g62 + g63 + g64 + g65 + g;

    static constexpr scalar_type c1 = coeff(-8.0368370789113464e-2);
    static constexpr scalar_type c2 = coeff(-5.6490613592447572e-2);
    static constexpr scalar_type c3 = coeff(0.4882856300427991);
    static constexpr scalar_type c4 = coeff(0.5057162114816189);
    static constexpr scalar_type c5 = coeff(-0.1071428571428569);
    static constexpr scalar_type c6 = coeff(0.25);

    static constexpr scalar_type cw1 = coeff(-1.764437648774483);
    static constexpr scalar_type cw2 = coeff(-0.4747565572063027);
    static constexpr scalar_type cw3 = coeff(2.369691846915802);
    static constexpr scalar_type cw4 = coeff(0.6195023590649829);
    static constexpr scalar_type cw5 = coeff(0.25);

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
        solver_.evaluate_and_update_jacobian(
            problem(), time, step_size, current);

        // 1st stage
        temp_rhs_ = problem().diff_coeff();
        solver_.add_time_derivative_term(step_size, g1, temp_rhs_);
        solver_.solve(temp_rhs_, k1_);

        // 2nd stage
        temp_var_ = g21 * k1_;
        solver_.apply_jacobian(temp_var_, temp_rhs_);
        temp_rhs_ *= step_size;
        temp_var_ = current + step_size * (a21 * k1_);
        problem().evaluate_on(time + b2 * step_size, temp_var_,
            evaluation_type{.diff_coeff = true});
        temp_rhs_ += problem().diff_coeff();
        solver_.add_time_derivative_term(step_size, g2, temp_rhs_);
        solver_.solve(temp_rhs_, k2_);

        // 3rd stage
        temp_var_ = g31 * k1_ + g32 * k2_;
        solver_.apply_jacobian(temp_var_, temp_rhs_);
        temp_rhs_ *= step_size;
        temp_var_ = current + step_size * (a31 * k1_ + a32 * k2_);
        problem().evaluate_on(time + b3 * step_size, temp_var_,
            evaluation_type{.diff_coeff = true});
        temp_rhs_ += problem().diff_coeff();
        solver_.add_time_derivative_term(step_size, g3, temp_rhs_);
        solver_.solve(temp_rhs_, k3_);

        // 4th stage
        temp_var_ = g41 * k1_ + g42 * k2_ + g43 * k3_;
        solver_.apply_jacobian(temp_var_, temp_rhs_);
        temp_rhs_ *= step_size;
        temp_var_ = current + step_size * (a41 * k1_ + a42 * k2_ + a43 * k3_);
        problem().evaluate_on(time + b4 * step_size, temp_var_,
            evaluation_type{.diff_coeff = true});
        temp_rhs_ += problem().diff_coeff();
        solver_.add_time_derivative_term(step_size, g4, temp_rhs_);
        solver_.solve(temp_rhs_, k4_);

        // 5th stage
        temp_var_ = g51 * k1_ + g52 * k2_ + g53 * k3_ + g54 * k4_;
        solver_.apply_jacobian(temp_var_, temp_rhs_);
        temp_rhs_ *= step_size;
        temp_var_ = current +
            step_size * (a51 * k1_ + a52 * k2_ + a53 * k3_ + a54 * k4_);
        problem().evaluate_on(time + b5 * step_size, temp_var_,
            evaluation_type{.diff_coeff = true});
        temp_rhs_ += problem().diff_coeff();
        solver_.add_time_derivative_term(step_size, g5, temp_rhs_);
        solver_.solve(temp_rhs_, k5_);

        // 6th stage
        temp_var_ = g61 * k1_ + g62 * k2_ + g63 * k3_ + g64 * k4_ + g65 * k5_;
        solver_.apply_jacobian(temp_var_, temp_rhs_);
        temp_rhs_ *= step_size;
        temp_var_ = current +
            step_size *
                (a61 * k1_ + a62 * k2_ + a63 * k3_ + a64 * k4_ + a65 * k5_);
        problem().evaluate_on(time + b6 * step_size, temp_var_,
            evaluation_type{.diff_coeff = true});
        temp_rhs_ += problem().diff_coeff();
        solver_.add_time_derivative_term(step_size, g6, temp_rhs_);
        solver_.solve(temp_rhs_, k6_);

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

    //! Temporary variable.
    variable_type temp_var_{};

    //! Temporary right-hand-side vector.
    variable_type temp_rhs_{};

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
