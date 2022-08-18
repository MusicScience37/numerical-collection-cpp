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
 * \brief Definition of ros34pw3_formula class.
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
 * \brief Class of ROS34PW3 formula in \cite Rang2005 for Rosenbrock method.
 *
 * \tparam Problem Type of problem.
 * \tparam EquationSolver Type of class to solve equations in Rosenbrock
 * methods.
 */
template <concepts::problem Problem,
    concepts::rosenbrock_equation_solver EquationSolver =
        default_rosenbrock_equation_solver_t<Problem>>
class ros34pw3_formula
    : public formula_base<ros34pw3_formula<Problem, EquationSolver>, Problem> {
public:
    //! Type of base class.
    using base_type =
        formula_base<ros34pw3_formula<Problem, EquationSolver>, Problem>;

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
    static constexpr index_type stages = 4;

    //! Order of this formula.
    static constexpr index_type order = 4;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = 2;

    //! Log tag.
    static constexpr auto log_tag =
        logging::log_tag_view("num_collect::ode::rosenbrock::ros34pw3_formula");

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
    static constexpr scalar_type a21 = coeff(2.5155456020628817e+00);
    static constexpr scalar_type a31 = coeff(5.0777280103144085e-01);
    static constexpr scalar_type a32 = coeff(7.5000000000000000e-01);
    static constexpr scalar_type a41 = coeff(1.3959081404277204e-01);
    static constexpr scalar_type a42 = coeff(-3.3111001065419338e-01);
    static constexpr scalar_type a43 = coeff(8.2040559712714178e-01);

    static constexpr scalar_type b1 = coeff(0);
    static constexpr scalar_type b2 = a21;
    static constexpr scalar_type b3 = a31 + a32;
    static constexpr scalar_type b4 = a41 + a42 + a43;

    static constexpr scalar_type g21 = coeff(-2.5155456020628817e+00);
    static constexpr scalar_type g31 = coeff(-8.7991339217106512e-01);
    static constexpr scalar_type g32 = coeff(-9.6014187766190695e-01);
    static constexpr scalar_type g41 = coeff(-4.1731389379448741e-01);
    static constexpr scalar_type g42 = coeff(4.1091047035857703e-01);
    static constexpr scalar_type g43 = coeff(-1.3558873204765276e+00);
    static constexpr scalar_type g = coeff(1.0685790213016289e+00);

    static constexpr scalar_type g1 = g;
    static constexpr scalar_type g2 = g21 + g;
    static constexpr scalar_type g3 = g31 + g32 + g;
    static constexpr scalar_type g4 = g41 + g42 + g43 + g;

    static constexpr scalar_type c1 = coeff(2.2047681286931747e-01);
    static constexpr scalar_type c2 = coeff(2.7828278331185935e-03);
    static constexpr scalar_type c3 = coeff(7.1844787635140066e-03);
    static constexpr scalar_type c4 = coeff(7.6955588053404989e-01);

    static constexpr scalar_type cw1 = coeff(3.1300297285209688e-01);
    static constexpr scalar_type cw2 = coeff(-2.8946895245112692e-01);
    static constexpr scalar_type cw3 = coeff(9.7646597959903003e-01);

    static constexpr scalar_type ce1 = c1 - cw1;
    static constexpr scalar_type ce2 = c2 - cw2;
    static constexpr scalar_type ce3 = c3 - cw3;
    static constexpr scalar_type ce4 = c4;
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

        estimate =
            current + step_size * (c1 * k1_ + c2 * k2_ + c3 * k3_ + c4 * k4_);
        error = step_size * (ce1 * k1_ + ce2 * k2_ + ce3 * k3_ + ce4 * k4_);
    }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> ros34pw3_formula& {
        if constexpr (requires(equation_solver_type & solver,
                          const error_tolerances<variable_type>& val) {
                          solver.tolerances(val);
                      }) {
            solver_.tolerances(val);
        }
        return *this;
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
    ///@}

    //! Temporary variable.
    variable_type temp_var_{};

    //! Temporary right-hand-side vector.
    variable_type temp_rhs_{};

    //! Solver.
    equation_solver_type solver_{g};
};

/*!
 * \brief Class of solver using ROS34PW3 formula in \cite Rang2005.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using ros34pw3_solver = embedded_solver<ros34pw3_formula<Problem>>;

}  // namespace num_collect::ode::rosenbrock
