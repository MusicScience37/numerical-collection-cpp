/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of inexact_newton_decomposed_jacobian_real_eigen_solver
 * class.
 */
#pragma once

#include <cmath>
#include <memory>
#include <type_traits>

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/LU>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/ode/concepts/differentiable_problem.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/multi_variate_differentiable_problem.h"
#include "num_collect/ode/concepts/single_variate_differentiable_problem.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode::runge_kutta::impl {

/*!
 * \brief Class to solve solve equations of decomposed Jacobians in inexact
 * Newton method of implicit Runge-Kutta methods for real eigenvalues.
 *
 * \tparam Problem Type of the problem.
 *
 * This class solves equations of the following coefficient matrix:
 *
 * \f[
 * h^{-1} \gamma^{-1} M - J
 * \f]
 *
 * where
 * - \f$h\f$ is the step size,
 * - \f$\gamma\f$ is the eigenvalue of the coefficients of intermidiate slopes,
 * - \f$M\f$ is the mass matrix (identity matrix if the problem is not a mass
 * problem),
 * - \f$J\f$ is the Jacobian matrix of the problem.
 */
template <concepts::differentiable_problem Problem>
class inexact_newton_decomposed_jacobian_real_eigen_solver;

/*!
 * \brief Class to solve solve equations of decomposed Jacobians in inexact
 * Newton method of implicit Runge-Kutta methods for real eigenvalues.
 *
 * \tparam Problem Type of the problem.
 *
 * This class solves equations of the following coefficient matrix:
 *
 * \f[
 * h^{-1} \gamma^{-1} M - J
 * \f]
 *
 * where
 * - \f$h\f$ is the step size,
 * - \f$\gamma\f$ is the eigenvalue of the coefficients of intermidiate slopes,
 * - \f$M\f$ is the mass matrix (identity matrix if the problem is not a mass
 * problem),
 * - \f$J\f$ is the Jacobian matrix of the problem.
 */
template <concepts::single_variate_differentiable_problem Problem>
class inexact_newton_decomposed_jacobian_real_eigen_solver<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(base::concepts::real_scalar<scalar_type>,
        "This class only supports real scalars.");

    // These conditions should be already satisfied when
    // single_variate_differentiable_problem concept is satisfied.
    static_assert(std::is_same_v<variable_type, scalar_type>);
    static_assert(std::is_same_v<jacobian_type, scalar_type>);

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] eigenvalue Eigenvalue of the coefficients of intermidiate
     * slopes.
     */
    explicit inexact_newton_decomposed_jacobian_real_eigen_solver(
        scalar_type eigenvalue)
        : eigenvalue_(eigenvalue) {}

    /*!
     * \brief Update Jacobian and internal parameters.
     *
     * \param[in] problem Problem.
     * \param[in] step_size Step size.
     *
     * \note Evaluation of the problem is assumed to be already done before
     * calling this function. This function simply uses the values in the
     * problem instance.
     */
    void update_jacobian(const problem_type& problem, scalar_type step_size) {
        NUM_COLLECT_DEBUG_ASSERT(step_size != static_cast<scalar_type>(0));

        scalar_type coeff =
            static_cast<scalar_type>(1) / (step_size * eigenvalue_);
        if constexpr (use_mass) {
            coeff *= problem.mass();
        }
        coeff -= problem.jacobian();

        coeff_inverse_ = static_cast<scalar_type>(1) / coeff;

        using std::isfinite;
        if (!isfinite(coeff_inverse_)) {
            NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
                "Failed to solve an equation. step_size={}.", step_size);
        }
    }

    /*!
     * \brief Solve an equation.
     *
     * \param[in] rhs Right-hand side of the equation.
     * \param[out] solution Solution.
     */
    void solve(scalar_type rhs, scalar_type& solution) const {
        solution = coeff_inverse_ * rhs;
    }

    /*!
     * \brief Apply the inverse of the eigenvalue.
     *
     * \param[in,out] target Target value. This is the input and the output.
     */
    void apply_eigenvalue_inverse(scalar_type& target) const {
        target /= eigenvalue_;
    }

    /*!
     * \brief Get the eigenvalue of the coefficients of intermidiate slopes.
     *
     * \return Eigenvalue.
     */
    [[nodiscard]] auto eigenvalue() const noexcept -> scalar_type {
        return eigenvalue_;
    }

private:
    //! Eigenvalue of the coefficients of intermidiate slopes.
    scalar_type eigenvalue_;

    //! Inverse of the coefficient of the linear equation.
    scalar_type coeff_inverse_{};
};

/*!
 * \brief Class to solve solve equations of decomposed Jacobians in inexact
 * Newton method of implicit Runge-Kutta methods for real eigenvalues.
 *
 * \tparam Problem Type of the problem.
 *
 * This class solves equations of the following coefficient matrix:
 *
 * \f[
 * h^{-1} \gamma^{-1} M - J
 * \f]
 *
 * where
 * - \f$h\f$ is the step size,
 * - \f$\gamma\f$ is the eigenvalue of the coefficients of intermidiate slopes,
 * - \f$M\f$ is the mass matrix (identity matrix if the problem is not a mass
 * problem),
 * - \f$J\f$ is the Jacobian matrix of the problem.
 */
template <concepts::multi_variate_differentiable_problem Problem>
    requires base::concepts::dense_matrix<typename Problem::jacobian_type>
class inexact_newton_decomposed_jacobian_real_eigen_solver<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(base::concepts::real_scalar<scalar_type>,
        "This class only supports real scalars.");

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] eigenvalue Eigenvalue of the coefficients of intermidiate
     * slopes.
     */
    explicit inexact_newton_decomposed_jacobian_real_eigen_solver(
        scalar_type eigenvalue)
        : eigenvalue_(eigenvalue) {}

    /*!
     * \brief Update Jacobian and internal parameters.
     *
     * \param[in] problem Problem.
     * \param[in] step_size Step size.
     *
     * \note Evaluation of the problem is assumed to be already done before
     * calling this function. This function simply uses the values in the
     * problem instance.
     */
    void update_jacobian(const problem_type& problem, scalar_type step_size) {
        NUM_COLLECT_DEBUG_ASSERT(step_size != static_cast<scalar_type>(0));

        const index_type dimension = problem.jacobian().rows();
        if constexpr (use_mass) {
            coeff_matrix_ = static_cast<scalar_type>(1) /
                (step_size * eigenvalue_) * problem.mass();
        } else {
            coeff_matrix_ = static_cast<scalar_type>(1) /
                (step_size * eigenvalue_) *
                jacobian_type::Identity(dimension, dimension);
        }
        coeff_matrix_ -= problem.jacobian();

        solver_.compute(coeff_matrix_);
    }

    /*!
     * \brief Solve an equation.
     *
     * \tparam Rhs Type of the right-hand side.
     * \tparam Solution Type of the solution.
     * \param[in] rhs Right-hand side of the equation.
     * \param[out] solution_in Solution.
     *
     * \note Vectors must have 2 elements.
     */
    template <typename Rhs, typename Solution>
    void solve(const Eigen::DenseBase<Rhs>& rhs,
        const Eigen::DenseBase<Solution>& solution_in) {
        // NOLINTNEXTLINE: Eigen's expression requires non-const lvalue for output.
        auto& solution = const_cast<Eigen::DenseBase<Solution>&>(solution_in);

        rhs_buffer_ = rhs;
        solution_buffer_ = solver_.solve(rhs_buffer_);
        if (!solution_buffer_.allFinite()) {
            NUM_COLLECT_LOG_AND_THROW(
                algorithm_failure, "Failed to solve an equation.");
        }
        solution = solution_buffer_;
    }

    /*!
     * \brief Apply the inverse of the eigenvalue.
     *
     * \tparam Target Type of the target values.
     * \param[in,out] target_in Target values. This is the input and the output.
     */
    template <typename Target>
    void apply_eigenvalue_inverse(
        const Eigen::DenseBase<Target>& target_in) const {
        // NOLINTNEXTLINE: Eigen's expression requires non-const lvalue for output.
        auto& target = const_cast<Eigen::DenseBase<Target>&>(target_in);

        target /= eigenvalue_;
    }

    /*!
     * \brief Get the eigenvalue of the coefficients of intermidiate slopes.
     *
     * \return Eigenvalue.
     */
    [[nodiscard]] auto eigenvalue() const noexcept -> scalar_type {
        return eigenvalue_;
    }

private:
    //! Eigenvalue of the coefficients of intermidiate slopes.
    scalar_type eigenvalue_;

    //! Buffer of the coefficient matrix.
    jacobian_type coeff_matrix_{};

    //! Solver of the current coefficient matrix.
    Eigen::PartialPivLU<jacobian_type> solver_{};

    //! Buffer of right-hand-side vector for iterative solvers.
    variable_type rhs_buffer_{};

    //! Buffer of solution vector for iterative solvers.
    variable_type solution_buffer_{};
};

/*!
 * \brief Class to solve solve equations of decomposed Jacobians in inexact
 * Newton method of implicit Runge-Kutta methods for real eigenvalues.
 *
 * \tparam Problem Type of the problem.
 *
 * This class solves equations of the following coefficient matrix:
 *
 * \f[
 * h^{-1} \gamma^{-1} M - J
 * \f]
 *
 * where
 * - \f$h\f$ is the step size,
 * - \f$\gamma\f$ is the eigenvalue of the coefficients of intermidiate slopes,
 * - \f$M\f$ is the mass matrix (identity matrix if the problem is not a mass
 * problem),
 * - \f$J\f$ is the Jacobian matrix of the problem.
 */
template <concepts::multi_variate_differentiable_problem Problem>
    requires base::concepts::sparse_matrix<typename Problem::jacobian_type>
class inexact_newton_decomposed_jacobian_real_eigen_solver<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(base::concepts::real_scalar<scalar_type>,
        "This class only supports real scalars.");

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] eigenvalue Eigenvalue of the coefficients of intermidiate
     * slopes.
     */
    explicit inexact_newton_decomposed_jacobian_real_eigen_solver(
        scalar_type eigenvalue)
        : eigenvalue_(eigenvalue) {}

    /*!
     * \brief Update Jacobian and internal parameters.
     *
     * \param[in] problem Problem.
     * \param[in] step_size Step size.
     *
     * \note Evaluation of the problem is assumed to be already done before
     * calling this function. This function simply uses the values in the
     * problem instance.
     */
    void update_jacobian(const problem_type& problem, scalar_type step_size) {
        NUM_COLLECT_DEBUG_ASSERT(step_size != static_cast<scalar_type>(0));

        const index_type dimension = problem.jacobian().rows();
        if constexpr (use_mass) {
            coeff_matrix_ = static_cast<scalar_type>(1) /
                (step_size * eigenvalue_) * problem.mass();
        } else {
            coeff_matrix_.resize(dimension, dimension);
            coeff_matrix_.setIdentity();
            coeff_matrix_ *=
                static_cast<scalar_type>(1) / (step_size * eigenvalue_);
        }
        coeff_matrix_ -= problem.jacobian();

        solver_->compute(coeff_matrix_);
    }

    /*!
     * \brief Solve an equation.
     *
     * \tparam Rhs Type of the right-hand side.
     * \tparam Solution Type of the solution.
     * \param[in] rhs Right-hand side of the equation.
     * \param[out] solution_in Solution.
     *
     * \note Vectors must have 2 elements.
     */
    template <typename Rhs, typename Solution>
    void solve(const Eigen::DenseBase<Rhs>& rhs,
        const Eigen::DenseBase<Solution>& solution_in) {
        // NOLINTNEXTLINE: Eigen's expression requires non-const lvalue for output.
        auto& solution = const_cast<Eigen::DenseBase<Solution>&>(solution_in);

        rhs_buffer_ = rhs;
        solution_buffer_ = solver_->solve(rhs_buffer_);
        if (!solution_buffer_.allFinite()) {
            NUM_COLLECT_LOG_AND_THROW(
                algorithm_failure, "Failed to solve an equation.");
        }
        solution = solution_buffer_;
    }

    /*!
     * \brief Apply the inverse of the eigenvalue.
     *
     * \tparam Target Type of the target values.
     * \param[in,out] target_in Target values. This is the input and the output.
     */
    template <typename Target>
    void apply_eigenvalue_inverse(
        const Eigen::DenseBase<Target>& target_in) const {
        // NOLINTNEXTLINE: Eigen's expression requires non-const lvalue for output.
        auto& target = const_cast<Eigen::DenseBase<Target>&>(target_in);
        target /= eigenvalue_;
    }

    /*!
     * \brief Get the eigenvalue of the coefficients of intermidiate slopes.
     *
     * \return Eigenvalue.
     */
    [[nodiscard]] auto eigenvalue() const noexcept -> scalar_type {
        return eigenvalue_;
    }

private:
    //! Eigenvalue of the coefficients of intermidiate slopes.
    scalar_type eigenvalue_;

    //! Buffer of the coefficient matrix.
    jacobian_type coeff_matrix_{};

    //! Solver of the current coefficient matrix.
    std::unique_ptr<Eigen::BiCGSTAB<jacobian_type>> solver_{
        std::make_unique<Eigen::BiCGSTAB<jacobian_type>>()};

    //! Buffer of right-hand-side vector for iterative solvers.
    variable_type rhs_buffer_{};

    //! Buffer of solution vector for iterative solvers.
    variable_type solution_buffer_{};
};

}  // namespace num_collect::ode::runge_kutta::impl
