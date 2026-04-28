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
 * \brief Definition of inexact_newton_decomposed_jacobian_complex_eigen_solver
 * class.
 */
#pragma once

#include <cmath>
#include <complex>
#include <concepts>
#include <memory>
#include <type_traits>

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/LU>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/concepts/differentiable_problem.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/multi_variate_differentiable_problem.h"
#include "num_collect/ode/concepts/single_variate_differentiable_problem.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode::runge_kutta::impl {

/*!
 * \brief Class to solve equations of decomposed Jacobians in inexact
 * Newton method of implicit Runge-Kutta methods for complex eigenvalues.
 *
 * \tparam Problem Type of the problem.
 *
 * This class solves equations of the following coefficient matrix:
 *
 * \f[
 * \begin{pmatrix}
 * h^{-1} \hat{\alpha} M - J & h^{-1} \hat{\beta} M      \\
 * -h^{-1} \hat{\beta} M     & h^{-1} \hat{\alpha} M - J
 * \end{pmatrix}
 * \f]
 *
 * where
 * - \f$h\f$ is the step size,
 * - \f$\hat{\alpha} + i \hat{\beta}\f$ is the inverse of the eigenvalue of the
 * coefficients of intermediate slopes,
 * - \f$M\f$ is the mass matrix (identity matrix if the problem is not a mass
 * problem),
 * - \f$J\f$ is the Jacobian matrix of the problem.
 *
 * This class uses the method with complex numbers to solve the equations as in
 * \cite Hairer1991.
 */
template <concepts::differentiable_problem Problem>
class inexact_newton_decomposed_jacobian_complex_eigen_solver;

/*!
 * \brief Class to solve equations of decomposed Jacobians in inexact
 * Newton method of implicit Runge-Kutta methods for complex eigenvalues.
 *
 * \tparam Problem Type of the problem.
 *
 * This class solves equations of the following coefficient matrix:
 *
 * \f[
 * \begin{pmatrix}
 * h^{-1} \hat{\alpha} M - J & h^{-1} \hat{\beta} M      \\
 * -h^{-1} \hat{\beta} M     & h^{-1} \hat{\alpha} M - J
 * \end{pmatrix}
 * \f]
 *
 * where
 * - \f$h\f$ is the step size,
 * - \f$\hat{\alpha} + i \hat{\beta}\f$ is the inverse of the eigenvalue of the
 * coefficients of intermediate slopes,
 * - \f$M\f$ is the mass matrix (identity matrix if the problem is not a mass
 * problem),
 * - \f$J\f$ is the Jacobian matrix of the problem.
 *
 * This class uses the method with complex numbers to solve the equations as in
 * \cite Hairer1991.
 */
template <concepts::single_variate_differentiable_problem Problem>
class inexact_newton_decomposed_jacobian_complex_eigen_solver<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(std::floating_point<scalar_type>,
        "This class only supports floating-point numbers in C++ standard.");

    // These conditions should be already satisfied when
    // single_variate_differentiable_problem concept is satisfied.
    static_assert(std::is_same_v<variable_type, scalar_type>);
    static_assert(std::is_same_v<jacobian_type, scalar_type>);

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    //! Type of complex scalars.
    using complex_scalar_type = std::complex<scalar_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] eigenvalue Eigenvalue of the coefficients of intermediate
     * slopes.
     */
    explicit inexact_newton_decomposed_jacobian_complex_eigen_solver(
        complex_scalar_type eigenvalue)
        : eigenvalue_(eigenvalue),
          eigenvalue_inverse_conjugate_(std::conj(
              static_cast<complex_scalar_type>(static_cast<scalar_type>(1)) /
              eigenvalue)) {}

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

        complex_scalar_type coeff = static_cast<scalar_type>(1) /
            (step_size)*eigenvalue_inverse_conjugate_;
        if constexpr (use_mass) {
            coeff *= problem.mass();
        }
        coeff -= problem.jacobian();

        coeff_inverse_ = static_cast<scalar_type>(1) / coeff;
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
        const Eigen::DenseBase<Solution>& solution_in) const {
        // NOLINTNEXTLINE: Eigen's expression requires non-const lvalue for output.
        auto& solution = const_cast<Eigen::DenseBase<Solution>&>(solution_in);

        NUM_COLLECT_DEBUG_ASSERT(rhs.size() == 2);
        NUM_COLLECT_DEBUG_ASSERT(solution.size() == 2);

        const complex_scalar_type rhs_as_complex =
            complex_scalar_type(rhs[0], rhs[1]);
        const complex_scalar_type solution_as_complex =
            coeff_inverse_ * rhs_as_complex;
        solution[0] = solution_as_complex.real();
        solution[1] = solution_as_complex.imag();
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

        NUM_COLLECT_DEBUG_ASSERT(target.size() == 2);

        const scalar_type first = target[0];
        const scalar_type second = target[1];
        target[0] = eigenvalue_inverse_conjugate_.real() * first -
            eigenvalue_inverse_conjugate_.imag() * second;
        target[1] = eigenvalue_inverse_conjugate_.imag() * first +
            eigenvalue_inverse_conjugate_.real() * second;
    }

    /*!
     * \brief Get the eigenvalue of the coefficients of intermediate slopes.
     *
     * \return Eigenvalue.
     */
    [[nodiscard]] auto eigenvalue() const noexcept -> complex_scalar_type {
        return eigenvalue_;
    }

private:
    //! Eigenvalue of the coefficients of intermediate slopes.
    complex_scalar_type eigenvalue_;

    //! Conjugate of the inverse of the eigenvalue of the coefficients of intermediate slopes.
    complex_scalar_type eigenvalue_inverse_conjugate_;

    //! Inverse of the coefficient of the linear equation.
    complex_scalar_type coeff_inverse_{};
};

/*!
 * \brief Class to solve equations of decomposed Jacobians in inexact
 * Newton method of implicit Runge-Kutta methods for complex eigenvalues.
 *
 * \tparam Problem Type of the problem.
 *
 * This class solves equations of the following coefficient matrix:
 *
 * \f[
 * \begin{pmatrix}
 * h^{-1} \hat{\alpha} M - J & h^{-1} \hat{\beta} M      \\
 * -h^{-1} \hat{\beta} M     & h^{-1} \hat{\alpha} M - J
 * \end{pmatrix}
 * \f]
 *
 * where
 * - \f$h\f$ is the step size,
 * - \f$\hat{\alpha} + i \hat{\beta}\f$ is the inverse of the eigenvalue of the
 * coefficients of intermediate slopes,
 * - \f$M\f$ is the mass matrix (identity matrix if the problem is not a mass
 * problem),
 * - \f$J\f$ is the Jacobian matrix of the problem.
 *
 * This class uses the method with complex numbers to solve the equations as in
 * \cite Hairer1991.
 */
template <concepts::multi_variate_differentiable_problem Problem>
    requires base::concepts::dense_matrix<typename Problem::jacobian_type>
class inexact_newton_decomposed_jacobian_complex_eigen_solver<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(std::floating_point<scalar_type>,
        "This class only supports floating-point numbers in C++ standard.");

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    //! Type of complex scalars.
    using complex_scalar_type = std::complex<scalar_type>;

    //! Type of complex variables.
    using complex_variable_type = Eigen::Matrix<complex_scalar_type,
        variable_type::RowsAtCompileTime, variable_type::ColsAtCompileTime,
        variable_type::Options, variable_type::MaxRowsAtCompileTime,
        variable_type::MaxColsAtCompileTime>;

    //! Type of complex Jacobian.
    using complex_jacobian_type = Eigen::Matrix<complex_scalar_type,
        jacobian_type::RowsAtCompileTime, jacobian_type::ColsAtCompileTime,
        jacobian_type::Options, jacobian_type::MaxRowsAtCompileTime,
        jacobian_type::MaxColsAtCompileTime>;

    /*!
     * \brief Constructor.
     *
     * \param[in] eigenvalue Eigenvalue of the coefficients of intermediate
     * slopes.
     */
    explicit inexact_newton_decomposed_jacobian_complex_eigen_solver(
        complex_scalar_type eigenvalue)
        : eigenvalue_(eigenvalue),
          eigenvalue_inverse_conjugate_(std::conj(
              static_cast<complex_scalar_type>(static_cast<scalar_type>(1)) /
              eigenvalue)) {}

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

        dimension_ = problem.jacobian().rows();
        if constexpr (use_mass) {
            coeff_matrix_ = static_cast<scalar_type>(1) /
                (step_size)*eigenvalue_inverse_conjugate_ * problem.mass();
        } else {
            coeff_matrix_ = static_cast<scalar_type>(1) /
                (step_size)*eigenvalue_inverse_conjugate_ *
                jacobian_type::Identity(dimension_, dimension_);
        }
        coeff_matrix_ -= problem.jacobian();

        solver_.compute(coeff_matrix_);

        complex_rhs_.resize(dimension_);
        complex_solution_.resize(dimension_);
    }

    /*!
     * \brief Solve an equation.
     *
     * \tparam Rhs Type of the right-hand side.
     * \tparam Solution Type of the solution.
     * \param[in] rhs Right-hand side of the equation.
     * \param[out] solution_in Solution.
     *
     * \note Vectors must have 2 elements per dimensions.
     */
    template <typename Rhs, typename Solution>
    void solve(const Eigen::DenseBase<Rhs>& rhs,
        const Eigen::DenseBase<Solution>& solution_in) {
        // NOLINTNEXTLINE: Eigen's expression requires non-const lvalue for output.
        auto& solution = const_cast<Eigen::DenseBase<Solution>&>(solution_in);

        NUM_COLLECT_DEBUG_ASSERT(rhs.size() == 2 * dimension_);
        NUM_COLLECT_DEBUG_ASSERT(solution.size() == 2 * dimension_);

        complex_rhs_.real() = rhs.head(dimension_);
        complex_rhs_.imag() = rhs.tail(dimension_);

        complex_solution_ = solver_.solve(complex_rhs_);

        solution.head(dimension_) = complex_solution_.real();
        solution.tail(dimension_) = complex_solution_.imag();
    }

    /*!
     * \brief Apply the inverse of the eigenvalue.
     *
     * \tparam Target Type of the target values.
     * \param[in,out] target_in Target values. This is the input and the output.
     */
    template <typename Target>
    void apply_eigenvalue_inverse(const Eigen::DenseBase<Target>& target_in) {
        // NOLINTNEXTLINE: Eigen's expression requires non-const lvalue for output.
        auto& target = const_cast<Eigen::DenseBase<Target>&>(target_in);

        NUM_COLLECT_DEBUG_ASSERT(target.size() == 2 * dimension_);

        complex_solution_.real() = target.head(dimension_);
        complex_solution_.imag() = target.tail(dimension_);
        complex_solution_ *= eigenvalue_inverse_conjugate_;
        target.head(dimension_) = complex_solution_.real();
        target.tail(dimension_) = complex_solution_.imag();
    }

    /*!
     * \brief Get the eigenvalue of the coefficients of intermediate slopes.
     *
     * \return Eigenvalue.
     */
    [[nodiscard]] auto eigenvalue() const noexcept -> complex_scalar_type {
        return eigenvalue_;
    }

private:
    //! Eigenvalue of the coefficients of intermediate slopes.
    complex_scalar_type eigenvalue_;

    //! Conjugate of the inverse of the eigenvalue of the coefficients of intermediate slopes.
    complex_scalar_type eigenvalue_inverse_conjugate_;

    //! Dimension of the problem.
    index_type dimension_{};

    //! Buffer of the coefficient matrix.
    complex_jacobian_type coeff_matrix_{};

    //! Buffer of the right-hand side of the linear equation in complex numbers.
    complex_variable_type complex_rhs_{};

    //! Buffer of the solution of the linear equation in complex numbers.
    complex_variable_type complex_solution_{};

    //! Solver of the current coefficient matrix.
    Eigen::PartialPivLU<complex_jacobian_type> solver_{};
};

/*!
 * \brief Class to solve equations of decomposed Jacobians in inexact
 * Newton method of implicit Runge-Kutta methods for complex eigenvalues.
 *
 * \tparam Problem Type of the problem.
 *
 * This class solves equations of the following coefficient matrix:
 *
 * \f[
 * \begin{pmatrix}
 * h^{-1} \hat{\alpha} M - J & h^{-1} \hat{\beta} M      \\
 * -h^{-1} \hat{\beta} M     & h^{-1} \hat{\alpha} M - J
 * \end{pmatrix}
 * \f]
 *
 * where
 * - \f$h\f$ is the step size,
 * - \f$\hat{\alpha} + i \hat{\beta}\f$ is the inverse of the eigenvalue of the
 * coefficients of intermediate slopes,
 * - \f$M\f$ is the mass matrix (identity matrix if the problem is not a mass
 * problem),
 * - \f$J\f$ is the Jacobian matrix of the problem.
 *
 * This class uses the method with complex numbers to solve the equations as in
 * \cite Hairer1991.
 */
template <concepts::multi_variate_differentiable_problem Problem>
    requires base::concepts::sparse_matrix<typename Problem::jacobian_type>
class inexact_newton_decomposed_jacobian_complex_eigen_solver<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(std::floating_point<scalar_type>,
        "This class only supports floating-point numbers in C++ standard.");

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    //! Type of complex scalars.
    using complex_scalar_type = std::complex<scalar_type>;

    //! Type of complex variables.
    using complex_variable_type = Eigen::Matrix<complex_scalar_type,
        variable_type::RowsAtCompileTime, variable_type::ColsAtCompileTime,
        variable_type::Options, variable_type::MaxRowsAtCompileTime,
        variable_type::MaxColsAtCompileTime>;

    //! Type of complex Jacobian.
    using complex_jacobian_type = Eigen::SparseMatrix<complex_scalar_type,
        jacobian_type::Options, typename jacobian_type::StorageIndex>;

    /*!
     * \brief Constructor.
     *
     * \param[in] eigenvalue Eigenvalue of the coefficients of intermediate
     * slopes.
     */
    explicit inexact_newton_decomposed_jacobian_complex_eigen_solver(
        complex_scalar_type eigenvalue)
        : eigenvalue_(eigenvalue),
          eigenvalue_inverse_conjugate_(std::conj(
              static_cast<complex_scalar_type>(static_cast<scalar_type>(1)) /
              eigenvalue)) {}

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

        dimension_ = problem.jacobian().rows();
        if constexpr (use_mass) {
            coeff_matrix_ = static_cast<scalar_type>(1) /
                (step_size)*eigenvalue_inverse_conjugate_ * problem.mass();
        } else {
            coeff_matrix_.resize(dimension_, dimension_);
            coeff_matrix_.setIdentity();
            coeff_matrix_ *= static_cast<scalar_type>(1) /
                (step_size)*eigenvalue_inverse_conjugate_;
        }
        coeff_matrix_ -=
            problem.jacobian().template cast<complex_scalar_type>();

        solver_->compute(coeff_matrix_);

        complex_rhs_.resize(dimension_);
        complex_solution_.resize(dimension_);
    }

    /*!
     * \brief Solve an equation.
     *
     * \tparam Rhs Type of the right-hand side.
     * \tparam Solution Type of the solution.
     * \param[in] rhs Right-hand side of the equation.
     * \param[out] solution_in Solution.
     *
     * \note Vectors must have 2 elements per dimensions.
     */
    template <typename Rhs, typename Solution>
    void solve(const Eigen::DenseBase<Rhs>& rhs,
        const Eigen::DenseBase<Solution>& solution_in) {
        // NOLINTNEXTLINE: Eigen's expression requires non-const lvalue for output.
        auto& solution = const_cast<Eigen::DenseBase<Solution>&>(solution_in);

        NUM_COLLECT_DEBUG_ASSERT(rhs.size() == 2 * dimension_);
        NUM_COLLECT_DEBUG_ASSERT(solution.size() == 2 * dimension_);

        complex_rhs_.real() = rhs.head(dimension_);
        complex_rhs_.imag() = rhs.tail(dimension_);

        complex_solution_ = solver_->solve(complex_rhs_);

        solution.head(dimension_) = complex_solution_.real();
        solution.tail(dimension_) = complex_solution_.imag();
    }

    /*!
     * \brief Apply the inverse of the eigenvalue.
     *
     * \tparam Target Type of the target values.
     * \param[in,out] target_in Target values. This is the input and the output.
     */
    template <typename Target>
    void apply_eigenvalue_inverse(const Eigen::DenseBase<Target>& target_in) {
        // NOLINTNEXTLINE: Eigen's expression requires non-const lvalue for output.
        auto& target = const_cast<Eigen::DenseBase<Target>&>(target_in);

        NUM_COLLECT_DEBUG_ASSERT(target.size() == 2 * dimension_);

        complex_solution_.real() = target.head(dimension_);
        complex_solution_.imag() = target.tail(dimension_);
        complex_solution_ *= eigenvalue_inverse_conjugate_;
        target.head(dimension_) = complex_solution_.real();
        target.tail(dimension_) = complex_solution_.imag();
    }

    /*!
     * \brief Get the eigenvalue of the coefficients of intermediate slopes.
     *
     * \return Eigenvalue.
     */
    [[nodiscard]] auto eigenvalue() const noexcept -> complex_scalar_type {
        return eigenvalue_;
    }

private:
    //! Eigenvalue of the coefficients of intermediate slopes.
    complex_scalar_type eigenvalue_;

    //! Conjugate of the inverse of the eigenvalue of the coefficients of intermediate slopes.
    complex_scalar_type eigenvalue_inverse_conjugate_;

    //! Dimension of the problem.
    index_type dimension_{};

    //! Buffer of the coefficient matrix.
    complex_jacobian_type coeff_matrix_{};

    //! Buffer of the right-hand side of the linear equation in complex numbers.
    complex_variable_type complex_rhs_{};

    //! Buffer of the solution of the linear equation in complex numbers.
    complex_variable_type complex_solution_{};

    //! Solver of the current coefficient matrix.
    std::unique_ptr<Eigen::BiCGSTAB<complex_jacobian_type>> solver_{
        std::make_unique<Eigen::BiCGSTAB<complex_jacobian_type>>()};
};

}  // namespace num_collect::ode::runge_kutta::impl
