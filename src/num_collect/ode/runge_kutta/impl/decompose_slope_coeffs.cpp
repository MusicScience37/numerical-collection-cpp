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
 * \brief Implementation of decompose_slope_coeffs function.
 */
#include "num_collect/ode/runge_kutta/impl/decompose_slope_coeffs.h"

#include <limits>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/LU>

#include "num_collect/base/exception.h"
#include "num_collect/logging/logging_macros.h"

namespace num_collect::ode::runge_kutta::impl {

void decompose_slope_coeffs_impl(int num_stages,
    const long double* slope_coeffs, long double* block_diagonal_matrix_coeffs,
    long double* eigenvectors_coeffs,
    long double* eigenvectors_inverse_coeffs) {
    using scalar_type = long double;
    using slope_coeff_matrix_type = Eigen::Matrix<scalar_type, Eigen::Dynamic,
        Eigen::Dynamic, Eigen::ColMajor>;

    const slope_coeff_matrix_type slope_coeffs_matrix =
        Eigen::Map<const slope_coeff_matrix_type>(
            slope_coeffs, num_stages, num_stages);

    Eigen::EigenSolver<slope_coeff_matrix_type> eigen_solver(
        slope_coeffs_matrix);
    const long double max_abs_eigenvalue =
        eigen_solver.eigenvalues().array().abs().maxCoeff();
    const long double min_abs_eigenvalue =
        eigen_solver.eigenvalues().array().abs().minCoeff();
    const long double reciprocal_condition_number =
        min_abs_eigenvalue / max_abs_eigenvalue;
    const bool is_coeffs_invertible = reciprocal_condition_number >
        std::numeric_limits<long double>::epsilon();
    if (!is_coeffs_invertible) {
        NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
            "Coefficients of intermediate slopes are not invertible.");
    }

    const slope_coeff_matrix_type block_diagonal_matrix =
        eigen_solver.pseudoEigenvalueMatrix();
    const slope_coeff_matrix_type& eigenvectors =
        eigen_solver.pseudoEigenvectors();

    Eigen::FullPivLU<slope_coeff_matrix_type> eigenvectors_lu(eigenvectors);
    if (!eigenvectors_lu.isInvertible()) {
        NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
            "Eigenvectors of coefficients of intermediate slopes are not "
            "invertible.");
    }
    const slope_coeff_matrix_type eigenvectors_inverse =
        eigenvectors_lu.inverse();

    Eigen::Map<slope_coeff_matrix_type>(block_diagonal_matrix_coeffs,
        num_stages, num_stages) = block_diagonal_matrix;
    Eigen::Map<slope_coeff_matrix_type>(
        eigenvectors_coeffs, num_stages, num_stages) = eigenvectors;
    Eigen::Map<slope_coeff_matrix_type>(eigenvectors_inverse_coeffs, num_stages,
        num_stages) = eigenvectors_inverse;
}

}  // namespace num_collect::ode::runge_kutta::impl
