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
 * \brief Definition of inexact_newton_stage_equation_solver class.
 */
#pragma once

#include <cmath>
#include <optional>

#include <Eigen/Core>
#include <Eigen/LU>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/iterative_solver_base.h"
#include "num_collect/logging/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/multi_variate_differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/single_variate_differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode {

//! Log tag.
constexpr auto inexact_newton_stage_equation_solver_tag = logging::log_tag_view(
    "num_collect::ode::inexact_newton_stage_equation_solver");

/*!
 * \brief Class to solve equations of implicit stages using inexact Newton
 * method.
 *
 * \note This solver uses stop criterion written in \cite Hairer1991.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::differentiable_problem Problem>
class inexact_newton_stage_equation_solver;

/*!
 * \brief Class to solve equations of implicit stages using inexact Newton
 * method.
 *
 * \note This solver uses stop criterion written in \cite Hairer1991.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::single_variate_differentiable_problem Problem>
class inexact_newton_stage_equation_solver<Problem>
    : public iterative_solver_base<
          inexact_newton_stage_equation_solver<Problem>> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(!problem_type::allowed_evaluations.mass,
        "Mass matrix is not supported.");
    // TODO: Actually, support is not difficult, but I want to test after
    // implementation, so I postpone the implementation.

    //! Constructor.
    inexact_newton_stage_equation_solver()
        : iterative_solver_base<inexact_newton_stage_equation_solver<Problem>>(
              inexact_newton_stage_equation_solver_tag) {}

    /*!
     * \brief Update Jacobian and internal parameters.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variable.
     * \param[in] solution_coeff Coefficient to multiply to solution in the
     * equation.
     */
    void update_jacobian(problem_type& problem, scalar_type time,
        scalar_type step_size, const variable_type& variable,
        scalar_type solution_coeff) {
        problem_ = &problem;
        time_ = time;
        step_size_ = step_size;
        variable_ = variable;
        solution_coeff_ = solution_coeff;

        problem_->evaluate_on(time_, variable_,
            evaluation_type{.diff_coeff = true, .jacobian = true});

        coeff_inverse_ = static_cast<jacobian_type>(1) /
            (static_cast<jacobian_type>(1) -
                step_size_ * solution_coeff_ * problem_->jacobian());
        using std::isfinite;
        if (!isfinite(coeff_inverse_)) {
            throw algorithm_failure("Failed to calculate inverse.");
        }
    }

    /*!
     * \brief Initialize for solving an equation.
     *
     * \param[in,out] solution Solution.
     */
    void init(variable_type& solution) {
        solution_ = &solution;
        update_norm_.reset();
        update_reduction_rate_.reset();
        iterations_ = 0;
    }

    /*!
     * \brief Initialize for solving an equation.
     *
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variable.
     * \param[in,out] solution Solution.
     */
    void init(scalar_type time, scalar_type step_size,
        const variable_type& variable, variable_type& solution) {
        time_ = time;
        step_size_ = step_size;
        variable_ = variable;
        init(solution);
    }

    /*!
     * \brief Iterate the algorithm once.
     *
     * \warning Any required initializations (with update_jacobian, init
     * functions) are assumed to have been done.
     */
    void iterate() {
        if (problem_ == nullptr || solution_ == nullptr) {
            throw precondition_not_satisfied("Initialization is not done yet.");
        }

        temp_variable_ =
            variable_ + step_size_ * solution_coeff_ * (*solution_);

        problem_->evaluate_on(time_,
            variable_ + step_size_ * solution_coeff_ * (*solution_),
            evaluation_type{.diff_coeff = true});
        residual_ = (*solution_) - problem_->diff_coeff();
        update_ = -coeff_inverse_ * residual_;
        *solution_ += update_;

        const scalar_type update_norm =
            tolerances().calc_norm(variable_, update_);
        if (update_norm_) {
            update_reduction_rate_ = update_norm / (*update_norm_);
        }
        update_norm_ = update_norm;

        ++iterations_;
    }

    /*!
     * \brief Determine if stopping criteria of the algorithm are satisfied.
     *
     * \return If stopping criteria of the algorithm are satisfied.
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        bool converged = false;
        if (update_norm_ && update_reduction_rate_ &&
            *update_reduction_rate_ < static_cast<scalar_type>(1)) {
            converged =
                (*update_reduction_rate_ /
                    (static_cast<scalar_type>(1) - *update_reduction_rate_)) *
                    (*update_norm_) <=
                tolerance_rate_;
        }

        constexpr index_type max_iterations = 100;  // safe guard
        return converged || (iterations_ > max_iterations);
    }

    /*!
     * \brief Configure an iteration logger.
     *
     * \param[in] iteration_logger Iteration logger.
     */
    void configure_iteration_logger(
        logging::iteration_logger& iteration_logger) const {
        iteration_logger.append<index_type>(
            "Iter.", [this] { return iterations(); });
        iteration_logger.append<scalar_type>(
            "Update", [this] { return update_norm(); });
    }

    /*!
     * \brief Get the norm of update.
     *
     * \return Norm of update.
     */
    [[nodiscard]] auto update_norm() const -> scalar_type {
        if (!update_norm_) {
            return static_cast<scalar_type>(0);
        }
        return *update_norm_;
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const -> index_type { return iterations_; }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> inexact_newton_stage_equation_solver& {
        tolerances_ = val;
        return *this;
    }

    /*!
     * \brief Get the error tolerances.
     *
     * \return Error tolerances.
     */
    [[nodiscard]] auto tolerances() const
        -> const error_tolerances<variable_type>& {
        return tolerances_;
    }

private:
    //! Pointer to the problem.
    problem_type* problem_{nullptr};

    //! Time.
    scalar_type time_{};

    //! Step size.
    scalar_type step_size_{};

    //! Coefficient to multiply to solution in the equation.
    scalar_type solution_coeff_{};

    //! Variable.
    variable_type variable_{};

    //! Solution.
    variable_type* solution_{nullptr};

    //! Inverse of the current coefficient.
    variable_type coeff_inverse_{};

    //! Temporary variable.
    variable_type temp_variable_{};

    //! Residual vector.
    variable_type residual_{};

    //! Update vector.
    variable_type update_{};

    //! Norm of update.
    std::optional<scalar_type> update_norm_{};

    //! Rate in which update is reduced from the previous step.
    std::optional<scalar_type> update_reduction_rate_{};

    //! Default rate of tolerance in this solver.
    static constexpr auto default_tolerance_rate =
        static_cast<scalar_type>(1e-2);

    //! Rate of tolerance in this solver.
    scalar_type tolerance_rate_{default_tolerance_rate};

    //! Number of iterations.
    index_type iterations_{0};

    //! Error tolerances.
    error_tolerances<variable_type> tolerances_{};
};

/*!
 * \brief Class to solve equations of implicit stages using inexact Newton
 * method.
 *
 * \note This solver uses stop criterion written in \cite Hairer1991.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::multi_variate_differentiable_problem Problem>
class inexact_newton_stage_equation_solver<Problem>
    : public iterative_solver_base<
          inexact_newton_stage_equation_solver<Problem>> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(!problem_type::allowed_evaluations.mass,
        "Mass matrix is not supported.");
    // TODO: Actually, support is not difficult, but I want to test after
    // implementation, so I postpone the implementation.

    //! Constructor.
    inexact_newton_stage_equation_solver()
        : iterative_solver_base<inexact_newton_stage_equation_solver<Problem>>(
              inexact_newton_stage_equation_solver_tag) {}

    /*!
     * \brief Update Jacobian and internal parameters.
     *
     * \tparam VariableExpression Type of the matrix expression of the variable.
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variable.
     * \param[in] solution_coeff Coefficient to multiply to solution in the
     * equation.
     */
    template <typename VariableExpression>
    void update_jacobian(problem_type& problem, scalar_type time,
        scalar_type step_size,
        const Eigen::MatrixBase<VariableExpression>& variable,
        scalar_type solution_coeff) {
        problem_ = &problem;
        time_ = time;
        step_size_ = step_size;
        variable_ = variable;
        solution_coeff_ = solution_coeff;

        problem_->evaluate_on(time_, variable_,
            evaluation_type{.diff_coeff = true, .jacobian = true});

        const index_type dim = variable_.size();
        lu_.compute(jacobian_type::Identity(dim, dim) -
            step_size_ * solution_coeff_ * problem_->jacobian());
    }

    /*!
     * \brief Initialize for solving an equation.
     *
     * \param[in,out] solution Solution.
     */
    void init(variable_type& solution) {
        solution_ = &solution;
        update_norm_.reset();
        update_reduction_rate_.reset();
        iterations_ = 0;
    }

    /*!
     * \brief Initialize for solving an equation.
     *
     * \tparam VariableExpression Type of the matrix expression of the variable.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variable.
     * \param[in,out] solution Solution.
     */
    template <typename VariableExpression>
    void init(scalar_type time, scalar_type step_size,
        const Eigen::MatrixBase<VariableExpression>& variable,
        variable_type& solution) {
        time_ = time;
        step_size_ = step_size;
        variable_ = variable;
        init(solution);
    }

    /*!
     * \brief Iterate the algorithm once.
     *
     * \warning Any required initializations (with update_jacobian, init
     * functions) are assumed to have been done.
     */
    void iterate() {
        if (problem_ == nullptr || solution_ == nullptr) {
            throw precondition_not_satisfied("Initialization is not done yet.");
        }

        temp_variable_ =
            variable_ + step_size_ * solution_coeff_ * (*solution_);

        problem_->evaluate_on(time_,
            variable_ + step_size_ * solution_coeff_ * (*solution_),
            evaluation_type{.diff_coeff = true});
        residual_ = (*solution_) - problem_->diff_coeff();
        update_ = -lu_.solve(residual_);
        *solution_ += update_;

        const scalar_type update_norm =
            tolerances().calc_norm(variable_, update_);
        if (update_norm_) {
            update_reduction_rate_ = update_norm / (*update_norm_);
        }
        update_norm_ = update_norm;

        ++iterations_;
    }

    /*!
     * \brief Determine if stopping criteria of the algorithm are satisfied.
     *
     * \return If stopping criteria of the algorithm are satisfied.
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        bool converged = false;
        if (update_norm_ && update_reduction_rate_ &&
            *update_reduction_rate_ < static_cast<scalar_type>(1)) {
            converged =
                (*update_reduction_rate_ /
                    (static_cast<scalar_type>(1) - *update_reduction_rate_)) *
                    (*update_norm_) <=
                tolerance_rate_;
        }

        constexpr index_type max_iterations = 100;  // safe guard
        return converged || (iterations_ > max_iterations);
    }

    /*!
     * \brief Configure an iteration logger.
     *
     * \param[in] iteration_logger Iteration logger.
     */
    void configure_iteration_logger(
        logging::iteration_logger& iteration_logger) const {
        iteration_logger.append<index_type>(
            "Iter.", [this] { return iterations(); });
        iteration_logger.append<scalar_type>(
            "Update", [this] { return update_norm(); });
    }

    /*!
     * \brief Get the norm of update.
     *
     * \return Norm of update.
     */
    [[nodiscard]] auto update_norm() const -> scalar_type {
        if (!update_norm_) {
            return static_cast<scalar_type>(0);
        }
        return *update_norm_;
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const -> index_type { return iterations_; }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> inexact_newton_stage_equation_solver& {
        tolerances_ = val;
        return *this;
    }

    /*!
     * \brief Get the error tolerances.
     *
     * \return Error tolerances.
     */
    [[nodiscard]] auto tolerances() const
        -> const error_tolerances<variable_type>& {
        return tolerances_;
    }

private:
    //! Pointer to the problem.
    problem_type* problem_{nullptr};

    //! Time.
    scalar_type time_{};

    //! Step size.
    scalar_type step_size_{};

    //! Coefficient to multiply to solution in the equation.
    scalar_type solution_coeff_{};

    //! Variable.
    variable_type variable_{};

    //! Solution.
    variable_type* solution_{nullptr};

    //! LU decomposition of the current coefficient matrix.
    Eigen::PartialPivLU<jacobian_type> lu_{};

    //! Temporary variable.
    variable_type temp_variable_{};

    //! Residual vector.
    variable_type residual_{};

    //! Update vector.
    variable_type update_{};

    //! Norm of update.
    std::optional<scalar_type> update_norm_{};

    //! Rate in which update is reduced from the previous step.
    std::optional<scalar_type> update_reduction_rate_{};

    //! Default rate of tolerance in this solver.
    static constexpr auto default_tolerance_rate =
        static_cast<scalar_type>(1e-2);

    //! Rate of tolerance in this solver.
    scalar_type tolerance_rate_{default_tolerance_rate};

    //! Number of iterations.
    index_type iterations_{0};

    //! Error tolerances.
    error_tolerances<variable_type> tolerances_{};
};

}  // namespace num_collect::ode
