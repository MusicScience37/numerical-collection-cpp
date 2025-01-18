/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of iterative_solver_base class.
 */
#pragma once

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCore>

#include "num_collect/base/concepts/dense_vector_of.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/util/assert.h"

namespace num_collect::linear {

namespace impl {

/*!
 * \brief Traits of iterative solvers.
 *
 * \tparam Solver Type of the solver.
 */
template <typename Solver, typename /*reserved for SFINAE*/ = void>
struct iterative_solver_traits;  // IWYU pragma: keep

}  // namespace impl

/*!
 * \brief Base class of iterative solvers.
 *
 * \tparam Derived Derived class.
 */
template <typename Derived>
class iterative_solver_base : public Eigen::SparseSolverBase<Derived> {
protected:
    //! Base class.
    using Base = Eigen::SparseSolverBase<Derived>;
    using Base::m_isInitialized;

public:
    //! Type of matrices. (For Eigen library.)
    using MatrixType =
        typename impl::iterative_solver_traits<Derived>::matrix_type;
    //! Type of scalars. (For Eigen library.)
    using Scalar = typename MatrixType::Scalar;
    //! Type of indices in storages. (For Eigen library.)
    using StorageIndex = typename MatrixType::StorageIndex;
    //! Type of real scalars. (For Eigen library.)
    using RealScalar = typename MatrixType::RealScalar;

    //! Type of matrices.
    using matrix_type = MatrixType;
    //! Type of scalars.
    using scalar_type = Scalar;
    //! Type of indices in storages.
    using storage_index_type = StorageIndex;
    //! Type of real scalars.
    using real_scalar_type = RealScalar;

    enum {  // NOLINT(performance-enum-size): Preserve the same implementation as Eigen library.
        //! Number of columns at compile time. (For Eigen library.)
        ColsAtCompileTime = MatrixType::ColsAtCompileTime,  // NOLINT
        //! Maximum number of columns at compile time. (For Eigen library.)
        MaxColsAtCompileTime = MatrixType::MaxColsAtCompileTime  // NOLINT
    };

    using Base::derived;

    /*!
     * \brief Constructor.
     */
    iterative_solver_base() { m_isInitialized = false; }

    /*!
     * \brief Initialize this solver using a coefficient matrix.
     *
     * \note This function will save the pointer to the argument.
     *
     * \param[in] coeff Coefficient matrix.
     * \return This.
     */
    auto compute(const matrix_type& coeff) -> Derived& {
        coeff_ = &coeff;
        m_isInitialized = true;
        return derived();
    }

    /*!
     * \brief Get the number of rows.
     *
     * \return Number of rows.
     */
    [[nodiscard]] auto rows() const noexcept -> Eigen::Index {
        return coeff().rows();
    }

    /*!
     * \brief Get the number of columns.
     *
     * \return Number of columns.
     */
    [[nodiscard]] auto cols() const noexcept -> Eigen::Index {
        return coeff().cols();
    }

    /*!
     * \brief Get the tolerance of rate of residual.
     *
     * \return Tolerance of rate of residual.
     */
    [[nodiscard]] auto tolerance() const noexcept -> real_scalar_type {
        return tolerance_;
    }

    /*!
     * \brief Set the tolerance of rate of residual.
     *
     * \param[in] val Tolerance of rate of residual.
     * \return This.
     */
    auto tolerance(const real_scalar_type& val) -> Derived& {
        NUM_COLLECT_PRECONDITION(val > static_cast<real_scalar_type>(0),
            "Tolerance of rate of residual must be positive value.");
        tolerance_ = val;
        return derived();
    }

    /*!
     * \brief Get the maximum number of iterations.
     *
     * \return Maximum number of iterations.
     */
    [[nodiscard]] auto max_iterations() const noexcept -> index_type {
        return max_iterations_;
    }

    /*!
     * \brief Set the maximum number of iterations.
     *
     * \param[in] val Maximum number of iterations.
     * \return This.
     */
    auto max_iterations(index_type val) -> Derived& {
        NUM_COLLECT_PRECONDITION(val > 0,
            "Maximum number of iterations must be a positive integer.");
        max_iterations_ = val;
        return derived();
    }

    /*!
     * \brief Solve a linear equation.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \param[in] right Right-hand-side vector.
     * \return Expression to solve the linear equation.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right>
    [[nodiscard]] auto solve(const Right& right) const
        -> Eigen::Solve<Derived, Right> {
        // TODO: Version for matrices.
        return Eigen::Solve<Derived, Right>(derived(), right);
    }

    /*!
     * \brief Solve a linear equation with a guess of the solution.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] right Right-hand-side vector.
     * \param[in] solution Solution vector.
     * \return Expression to solve the linear equation.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    [[nodiscard]] auto solve_with_guess(
        const Right& right, const Solution& solution) const
        -> Eigen::SolveWithGuess<Derived, Right, Solution> {
        // TODO: Version for matrices.
        return Eigen::SolveWithGuess<Derived, Right, Solution>(
            derived(), right, solution);
    }

    /*!
     * \brief Internal function to solve for a right-hand-side vector.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] right Right-hand-side vector.
     * \param[in,out] solution Solution vector.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    void _solve_impl(  // NOLINT(readability-identifier-naming
                       // name required by Eigen.
        const Right& right, Solution& solution) const {
        // TODO: Version for matrices.
        solution.setZero();
        derived()._solve_with_guess_impl(right, solution);
    }

    /*!
     * \brief Internal function to solve for a right-hand-side vector.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] right Right-hand-side vector.
     * \param[in,out] solution Solution vector.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    void _solve_with_guess_impl(  // NOLINT(readability-identifier-naming
                                  // name required by Eigen.
        const Right& right, Solution& solution) const {
        // TODO: Version for matrices.
        derived().solve_vector_in_place(right, solution);
    }

protected:
    /*!
     * \brief Get the coefficient matrix.
     *
     * \return Coefficient matrix.
     */
    [[nodiscard]] auto coeff() const noexcept -> const matrix_type& {
        NUM_COLLECT_ASSERT(static_cast<const void*>(coeff_) != nullptr);
        return *coeff_;
    }

private:
    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 10000;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};

    //! Default tolerance of rate of residual.
    static constexpr auto default_tolerance =
        Eigen::NumTraits<real_scalar_type>::dummy_precision();

    //! Tolerance of rate of residual.
    real_scalar_type tolerance_{default_tolerance};

    //! Coefficient matrix.
    const matrix_type* coeff_{nullptr};
};

}  // namespace num_collect::linear
