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
 * \brief Definition of mixed_broyden_rosenbrock_equation_solver class.
 */
#pragma once

#include <limits>

#include <Eigen/Core>  // IWYU pragma: keep
#include <Eigen/LU>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/ode/concepts/multi_variate_differentiable_problem.h"
#include "num_collect/ode/evaluation_type.h"

namespace num_collect::ode::rosenbrock {

//! Log tag.
constexpr auto mixed_broyden_rosenbrock_equation_solver_tag =
    logging::log_tag_view(
        "num_collect::ode::rosenbrock::mixed_broyden_rosenbrock_equation_"
        "solver");

/*!
 * \brief Class to solve equations in Rosenbrock methods using Broyden's update
 * in \cite Novati2008.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::multi_variate_differentiable_problem Problem>
class mixed_broyden_rosenbrock_equation_solver : public logging::logging_mixin {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobians.
    using jacobian_type = typename problem_type::jacobian_type;

    //! Type of the LU solver.
    using lu_solver_type = Eigen::PartialPivLU<jacobian_type>;

    static_assert(!problem_type::allowed_evaluations.mass,
        "Mass matrix is not supported.");

    /*!
     * \brief Constructor.
     *
     * \param[in] inverted_jacobian_coeff Coefficient multiplied to Jacobian
     * matrices in inverted matrices.
     */
    explicit mixed_broyden_rosenbrock_equation_solver(
        const scalar_type& inverted_jacobian_coeff)
        : logging::logging_mixin(mixed_broyden_rosenbrock_equation_solver_tag),
          inverted_jacobian_coeff_(inverted_jacobian_coeff) {}

    /*!
     * \brief Update Jacobian matrix and internal parameters.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variable.
     */
    void evaluate_and_update_jacobian(problem_type& problem,
        const scalar_type& time, const scalar_type& step_size,
        const variable_type& variable) {
        if (!evaluated_once_) {
            evaluate_exactly(problem, time, step_size, variable);
            return;
        }
        if (time <= time_) {
            evaluate_exactly(problem, time, step_size, variable);
            return;
        }

        s_ = variable - variable_;
        if (s_.norm() <
            variable_.norm() * std::numeric_limits<scalar_type>::epsilon()) {
            evaluate_exactly(problem, time, step_size, variable);
            return;
        }

        problem.evaluate_on(
            time, variable, evaluation_type{.diff_coeff = true});
        q_ = problem.diff_coeff() - diff_coeff_;
        v_ = s_ - step_size * inverted_jacobian_coeff_ * q_;
        if (v_.norm() <
            variable_.norm() * std::numeric_limits<scalar_type>::epsilon()) {
            evaluate_exactly(problem, time, step_size, variable);
            return;
        }

        // Update of Jacobian.
        update_ = (step_size / step_size_ * q_ - jacobian_ * s_) *
            s_.transpose() / s_.squaredNorm();
        jacobian_ += update_;
        jacobian_ *= step_size_ / step_size;

        // Update of inverse.
        update_ = (s_ - inverse_ * v_) * v_.transpose() / v_.squaredNorm();
        inverse_ += update_;

        time_ = time;
        step_size_ = step_size;
        variable_ = variable;
        diff_coeff_ = problem.diff_coeff();

        NUM_COLLECT_LOG_TRACE(this->logger(), "Using approximate Jacobian.");
    }

    /*!
     * \brief Multiply Jacobian matrix to a vector.
     *
     * \param[in] target Target.
     * \param[out] result Result.
     */
    void apply_jacobian(const variable_type& target, variable_type& result) {
        result = jacobian_ * target;
    }

    /*!
     * \brief Add a term of partial derivative with respect to time.
     *
     * \param[in] step_size Step size.
     * \param[in] coeff Coefficient in formula.
     * \param[in,out] target Target variable.
     */
    void add_time_derivative_term(const scalar_type& step_size,
        const scalar_type& coeff, variable_type& target) {
        // Ignore this term always in this class.
        (void)step_size;
        (void)coeff;
        (void)target;
    }

    /*!
     * \brief Solve a linear equation.
     *
     * \param[in] rhs Right-hand-side value.
     * \param[out] result Result.
     */
    void solve(const variable_type& rhs, variable_type& result) {
        result = inverse_ * rhs;
    }

private:
    /*!
     * \brief Update Jacobian matrix and internal parameters using exact values
     * from the problem.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variable.
     */
    void evaluate_exactly(problem_type& problem, const scalar_type& time,
        const scalar_type& step_size, const variable_type& variable) {
        problem.evaluate_on(time, variable,
            evaluation_type{.diff_coeff = true, .jacobian = true});
        jacobian_ = problem.jacobian();

        const index_type variable_size = variable.size();
        lu_.compute(jacobian_type::Identity(variable_size, variable_size) -
            step_size * inverted_jacobian_coeff_ * jacobian_);

        inverse_ = lu_.inverse();
        if (!inverse_.array().isFinite().all()) {
            NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
                "Failed to solve an equation. step_size={}, cond={}.",
                step_size_, lu_.rcond());
        }

        time_ = time;
        step_size_ = step_size;
        variable_ = variable;
        diff_coeff_ = problem.diff_coeff();
        evaluated_once_ = true;

        NUM_COLLECT_LOG_TRACE(this->logger(), "Using exact Jacobian.");
    }

    //! Whether evaluated a Jacobian.
    bool evaluated_once_{false};

    //! Jacobian matrix.
    jacobian_type jacobian_{};

    //! LU solver.
    lu_solver_type lu_{};

    //! Inverse of the current coefficient matrix.
    jacobian_type inverse_{};

    //! Time.
    scalar_type time_{};

    //! Step size.
    scalar_type step_size_{};

    //! Variable.
    variable_type variable_{};

    //! Differential coefficients.
    variable_type diff_coeff_{};

    /*!
     * \name Intermediate variables.
     */
    ///@{
    //! Intermediate variable.
    variable_type s_{};
    variable_type q_{};
    variable_type v_{};
    jacobian_type update_{};
    ///@}

    //! Coefficient multiplied to Jacobian matrices in inverted matrices.
    scalar_type inverted_jacobian_coeff_{static_cast<scalar_type>(1)};
};

}  // namespace num_collect::ode::rosenbrock
