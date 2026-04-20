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
 * \brief Definition of inexact_newton_decomposed_full_equation_solver_data
 * class.
 */
#pragma once

#include <cmath>
#include <complex>
#include <concepts>
#include <cstdlib>
#include <limits>
#include <variant>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_collect/ode/concepts/differentiable_problem.h"
#include "num_collect/ode/runge_kutta/impl/decompose_slope_coeffs.h"
#include "num_collect/ode/runge_kutta/impl/inexact_newton_decomposed_jacobian_complex_eigen_solver.h"
#include "num_collect/ode/runge_kutta/impl/inexact_newton_decomposed_jacobian_real_eigen_solver.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/vector.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class to store data for
 * inexact_newton_decomposed_full_update_equation_solver,
 * inexact_newton_decomposed_full_slope_equation_solver classes.
 *
 * \tparam Scalar Type of scalar.
 * \tparam NumStages Number of stages of the formula.
 *
 * \note Creating an instance of this class takes some time
 * due to matrix decompositions in the constructor,
 * so it is recommended to reuse the instance when applicable.
 */
template <std::floating_point Scalar, int NumStages>
class inexact_newton_decomposed_full_equation_solver_data {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    //! Type of the matrix of coefficients of intermediate slopes.
    using slope_coeff_matrix_type =
        Eigen::Matrix<scalar_type, NumStages, NumStages>;

    //! Type of the vector of intermediate updates.
    using update_coeff_vector_type = Eigen::Vector<scalar_type, NumStages>;

    /*!
     * \brief Constructor.
     *
     * \param[in] slope_coeffs Coefficients of intermediate slopes in the
     * formula.
     * \param[in] time_coeffs Coefficients of time in the formula.
     * \param[in] block_diagonal_matrix Block-diagonal matrix in eigenvalue
     * decomposition.
     * \param[in] eigenvectors Eigenvectors in eigenvalue decomposition.
     * \param[in] eigenvectors_inverse Inverse of eigenvectors in eigenvalue
     * decomposition.
     */
    inexact_newton_decomposed_full_equation_solver_data(
        const slope_coeff_matrix_type& slope_coeffs,
        const update_coeff_vector_type& time_coeffs,
        const slope_coeff_matrix_type& block_diagonal_matrix,
        const slope_coeff_matrix_type& eigenvectors,
        const slope_coeff_matrix_type& eigenvectors_inverse)
        : slope_coeffs_(slope_coeffs),
          time_coeffs_(time_coeffs),
          block_diagonal_matrix_(block_diagonal_matrix),
          eigenvectors_(eigenvectors),
          eigenvectors_inverse_(eigenvectors_inverse) {}

    /*!
     * \brief Generate data from coefficients in Butcher tableau.
     *
     * \param[in] slope_coeffs Coefficients of intermediate slopes in the
     * formula.
     * \param[in] time_coeffs Coefficients of time in the formula.
     * \return Data.
     */
    static auto from_butcher_tableau(
        const slope_coeff_matrix_type& slope_coeffs,
        const update_coeff_vector_type& time_coeffs) {
        slope_coeff_matrix_type block_diagonal_matrix;
        slope_coeff_matrix_type eigenvectors;
        slope_coeff_matrix_type eigenvectors_inverse;
        impl::decompose_slope_coeffs(slope_coeffs, block_diagonal_matrix,
            eigenvectors, eigenvectors_inverse);

        return inexact_newton_decomposed_full_equation_solver_data(slope_coeffs,
            time_coeffs, block_diagonal_matrix, eigenvectors,
            eigenvectors_inverse);
    }

    /*!
     * \brief Get the coefficients of intermediate slopes in the formula.
     *
     * \return Coefficients of intermediate slopes in the formula.
     */
    [[nodiscard]] auto slope_coeffs() const -> const slope_coeff_matrix_type& {
        return slope_coeffs_;
    }

    /*!
     * \brief Get the coefficients of time.
     *
     * \return Coefficients of time.
     */
    [[nodiscard]] auto time_coeffs() const -> const update_coeff_vector_type& {
        return time_coeffs_;
    }

    /*!
     * \brief Get the block-diagonal matrix in eigenvalue decomposition.
     *
     * \return Block-diagonal matrix in eigenvalue decomposition.
     */
    [[nodiscard]] auto block_diagonal_matrix() const
        -> const slope_coeff_matrix_type& {
        return block_diagonal_matrix_;
    }

    /*!
     * \brief Get the eigenvectors in eigenvalue decomposition.
     *
     * \return Eigenvectors in eigenvalue decomposition.
     */
    [[nodiscard]] auto eigenvectors() const -> const slope_coeff_matrix_type& {
        return eigenvectors_;
    }

    /*!
     * \brief Get the inverse of eigenvectors in eigenvalue decomposition.
     *
     * \return Inverse of eigenvectors in eigenvalue decomposition.
     */
    [[nodiscard]] auto eigenvectors_inverse() const
        -> const slope_coeff_matrix_type& {
        return eigenvectors_inverse_;
    }

private:
    //! Coefficients of intermediate slopes in the formula.
    slope_coeff_matrix_type slope_coeffs_;

    //! Coefficients of time.
    update_coeff_vector_type time_coeffs_;

    //! Block-diagonal matrix in eigenvalue decomposition.
    slope_coeff_matrix_type block_diagonal_matrix_{};

    //! Eigenvectors in eigenvalue decomposition.
    slope_coeff_matrix_type eigenvectors_{};

    //! Inverse of eigenvectors in eigenvalue decomposition.
    slope_coeff_matrix_type eigenvectors_inverse_{};
};

namespace impl {

/*!
 * \brief Generate solvers of decomposed linear equations
 * for inexact_newton_decomposed_full_update_equation_solver class.
 *
 * \tparam Problem Type of the problem.
 * \tparam Scalar Type of scalars.
 * \tparam NumStages Number of stages of the formula.
 * \param[in] data Data for the solver.
 * \return Solvers of decomposed linear equations.
 */
template <concepts::differentiable_problem Problem, std::floating_point Scalar,
    int NumStages>
[[nodiscard]] auto generate_decomposed_solvers(
    const inexact_newton_decomposed_full_equation_solver_data<Scalar,
        NumStages>& data) {
    using real_eigen_decomposed_solver_type =
        impl::inexact_newton_decomposed_jacobian_real_eigen_solver<Problem>;
    using complex_eigen_decomposed_solver_type =
        impl::inexact_newton_decomposed_jacobian_complex_eigen_solver<Problem>;
    using decomposed_solver_type =
        std::variant<real_eigen_decomposed_solver_type,
            complex_eigen_decomposed_solver_type>;

    util::vector<decomposed_solver_type> decomposed_solvers;
    const auto& block_diagonal_matrix = data.block_diagonal_matrix();
    index_type diagonal_index = 0;
    while (diagonal_index < NumStages) {
        const bool is_real_eigenvalue = diagonal_index == NumStages - 1 ||
            std::abs(block_diagonal_matrix(diagonal_index + 1,
                diagonal_index)) < std::numeric_limits<Scalar>::epsilon();
        if (is_real_eigenvalue) {
            const Scalar eigenvalue =
                block_diagonal_matrix(diagonal_index, diagonal_index);
            decomposed_solvers.emplace_back(
                real_eigen_decomposed_solver_type(eigenvalue));
            ++diagonal_index;
        } else {
            NUM_COLLECT_DEBUG_ASSERT(
                std::abs(
                    block_diagonal_matrix(diagonal_index + 1, diagonal_index) +
                    block_diagonal_matrix(diagonal_index, diagonal_index + 1)) <
                std::numeric_limits<Scalar>::epsilon());

            const Scalar real_part =
                block_diagonal_matrix(diagonal_index, diagonal_index);
            const Scalar imag_part =
                block_diagonal_matrix(diagonal_index, diagonal_index + 1);
            const std::complex<Scalar> eigenvalue(real_part, imag_part);
            decomposed_solvers.emplace_back(
                complex_eigen_decomposed_solver_type(eigenvalue));
            diagonal_index += 2;
        }
    }
    return decomposed_solvers;
}

}  // namespace impl

}  // namespace num_collect::ode::runge_kutta
