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
#include <type_traits>

#include "num_collect/base/concepts/dense_vector_of.h"
#include "num_collect/base/exception.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/ode/concepts/differentiable_problem.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/single_variate_differentiable_problem.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode::runge_kutta::impl {

/*!
 * \brief Class to solve solve equations of decomposed Jacobians in inexact
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
 * coefficients of intermidiate slopes,
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
 * \brief Class to solve solve equations of decomposed Jacobians in inexact
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
 * coefficients of intermidiate slopes,
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
     * \param[in] eigenvalue Eigenvalue of the coefficients of intermidiate
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

        using std::isfinite;
        if (!isfinite(coeff_inverse_.real()) ||
            !isfinite(coeff_inverse_.imag())) {
            NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
                "Failed to solve an equation. step_size={}.", step_size);
        }
    }

    /*!
     * \brief Solve an equation.
     *
     * \tparam Rhs Type of the right-hand side.
     * \tparam Solution Type of the solution.
     * \param[in] rhs Right-hand side of the equation.
     * \param[out] solution Solution.
     *
     * \note Vectors must have 2 elements.
     */
    template <base::concepts::dense_vector_of<scalar_type> Rhs,
        base::concepts::dense_vector_of<scalar_type> Solution>
    void solve(const Rhs& rhs, Solution& solution) const {
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
     * \brief Get the eigenvalue of the coefficients of intermidiate slopes.
     *
     * \return Eigenvalue.
     */
    [[nodiscard]] auto eigenvalue() const noexcept -> complex_scalar_type {
        return eigenvalue_;
    }

private:
    //! Eigenvalue of the coefficients of intermidiate slopes.
    complex_scalar_type eigenvalue_;

    //! Conjugate of the inverse of the eigenvalue of the coefficients of intermidiate slopes.
    complex_scalar_type eigenvalue_inverse_conjugate_;

    //! Inverse of the coefficient of the linear equation.
    complex_scalar_type coeff_inverse_{};
};

}  // namespace num_collect::ode::runge_kutta::impl
