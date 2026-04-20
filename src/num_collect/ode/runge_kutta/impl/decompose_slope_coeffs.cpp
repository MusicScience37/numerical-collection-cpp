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

void decompose_slope_coeffs_impl(
    const Eigen::MatrixX<long double>& slope_coeffs,
    Eigen::MatrixX<long double>& block_diagonal_matrix,
    Eigen::MatrixX<long double>& eigenvectors,
    Eigen::MatrixX<long double>& eigenvectors_inverse) {
    using scalar_type = long double;
    using slope_coeff_matrix_type = Eigen::MatrixX<scalar_type>;

    Eigen::EigenSolver<slope_coeff_matrix_type> eigen_solver(slope_coeffs);
    const bool is_coeffs_invertible =
        (eigen_solver.eigenvalues().array().abs() >
            std::numeric_limits<scalar_type>::epsilon())
            .all();
    if (!is_coeffs_invertible) {
        NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
            "Coefficients of intermidiate slopes are not invertible.");
    }

    block_diagonal_matrix = eigen_solver.pseudoEigenvalueMatrix();
    eigenvectors = eigen_solver.pseudoEigenvectors();

    Eigen::FullPivLU<slope_coeff_matrix_type> eigenvectors_lu(eigenvectors);
    if (!eigenvectors_lu.isInvertible()) {
        NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
            "Eigenvectors of coefficients of intermidiate slopes are not "
            "invertible.");
    }
    eigenvectors_inverse = eigenvectors_lu.inverse();
}

}  // namespace num_collect::ode::runge_kutta::impl
